#include <iostream>
#include <unistd.h> //sleep
#include <exception> //std::exception

#include <fstream> //  std::ifstream, std::ofstream
#include <chrono> //std::chrono


#include "os_ps_monitor.hpp"
#include "json.hpp" 


using json = nlohmann::json;
Logger info_logger("./info_logger.txt");

ConfigureInfo::ConfigureInfo(const std::string& json_file_path):monitoring_interval(0), monitoring_duration(0) 
{
    try 
    {

        std::ifstream config_file(json_file_path);
        
        if (!config_file.is_open()) 
        {
            throw std::runtime_error("Unable to open config file: " + json_file_path);
        }

        json config_json;
        config_file >> config_json;

        monitoring_interval = config_json.at("monitoring_interval").get<size_t>(); 
        if(0 == monitoring_interval )
        {
            info_logger.write("[error] invalid monitoring_interval in config file: " + json_file_path);
            throw std::runtime_error("invalid monitoring_interval in config file: " + json_file_path);
        }

        monitoring_duration = config_json.at("monitoring_duration").get<size_t>();
        if(0 == monitoring_duration )
        {
            info_logger.write("[error] invalid monitoring_duration in config file: " + json_file_path);
            throw std::runtime_error("invalid monitoring_duration in config file: " + json_file_path);
        }

        db_file_path = config_json.at("db_file_path").get<std::string>();
        if(db_file_path.empty())
        {
            info_logger.write("[error] invalid db_file_path in config file: " + json_file_path);
            throw std::runtime_error("invalid db_file_path in config file: " + json_file_path);
        }

        csv_file_path = config_json.at("csv_file_path").get<std::string>();
        if(csv_file_path.empty())
        {
            info_logger.write("[error] invalid csv_file_path in config file: " + json_file_path);
            throw std::runtime_error("invalid csv_file_path in config file: " + json_file_path);
        }

        excluded_processes = config_json.at("excluded_processes").get<std::set<size_t>>();
        
        info_logger.write("[info] configuration loaded successfully from: " + json_file_path);
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "error in configuration: " << e.what() << std::endl;
        throw; 
    }
}



Database::Database(const std::string &db_file_path)
{
    int rc = sqlite3_open(db_file_path.c_str(), &db);
    
    if (rc) 
    {
        info_logger.write("[error] failed to open database: " + std::string(sqlite3_errmsg(db)));
        throw std::runtime_error("failed to open database, ");
    }

    std::string create_table_command = 
        "CREATE TABLE IF NOT EXISTS avg_info ("
        "monitor_num INTEGER PRIMARY KEY, "
        "avg_cpu_usage REAL, "
        "avg_memory_usage REAL);";    
    
    char *err_msg = nullptr;
    
    rc = sqlite3_exec(db, create_table_command.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK)
    {
        info_logger.write("[error] failed to create table in database: " + std::string(sqlite3_errmsg(db)));            
        sqlite3_free(err_msg); 

        throw std::runtime_error("create table in database FAILED, ");           
    }    
    

    std::string clear_table_command = "DELETE FROM avg_info;"; //delete previous results
    rc = sqlite3_exec(db, clear_table_command.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK)
    {

        info_logger.write("[error] failed to clear table in database: " + std::string(sqlite3_errmsg(db)));
        sqlite3_free(err_msg); 
        
        throw std::runtime_error("failed to clear table in database: " + std::string(sqlite3_errmsg(db)));
    }

    info_logger.write("[info] database opened and table created successfully ");
}


Database::~Database() noexcept
{
    if (db) 
    {
        sqlite3_close(db);
        db = nullptr;
    }

    info_logger.write("[info] database closed");
}

Status Database::InsertAvgInfo(int monitor_num, double avg_cpu_usage, double avg_memory_usage) 
{
    std::string insert_command = 
        "INSERT INTO avg_info (monitor_num, avg_cpu_usage, avg_memory_usage) "
        "VALUES (" + std::to_string(monitor_num) + ", " + std::to_string(avg_cpu_usage) + ", " +
        std::to_string(avg_memory_usage) + ");"; 

    char *err_msg = nullptr;
    
    int rc = sqlite3_exec(db, insert_command.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) 
    {
        info_logger.write("[error] failed to insert average info into database: " + std::string(sqlite3_errmsg(db)));
        sqlite3_free(err_msg); 

        return FAIL;
    }

    info_logger.write("[info] average info inserted into database successfully for monitor num: " + std::to_string(monitor_num));

    return SUCCESS;
}



Logger::Logger(const std::string& filepath)
{
    target_file.open(filepath, std::ios::out | std::ios::trunc); // trunc to clear previous sessions
    
    if (!target_file.is_open()) 
    {
        info_logger.write("[error] open CSV file failed: " + filepath);
        throw std::runtime_error("open CSV file failed " + filepath);
    }

}  

Logger::~Logger() noexcept
{
    if (target_file.is_open()) 
    {
        target_file.close();
    }

}




void Logger::write(const std::string str)
{
    if (!target_file.is_open()) 
    {
        throw std::runtime_error("CSV file is not open: ");
    }

    target_file << str << "\n"; 
    
}






MonitorApp::MonitorApp(const ConfigureInfo &config_info_):m_should_stop(false),CSV_file(config_info_.csv_file_path),m_db(config_info_.db_file_path)
            ,m_monitoring_interval(config_info_.monitoring_interval),m_monitoring_duration(config_info_.monitoring_duration)
            ,excluded_pids(config_info_.excluded_processes) 
{
    if(m_monitoring_interval == 0 || m_monitoring_duration == 0) 
    {
        info_logger.write("[error] invalid monitoring interval or duration");
        throw std::runtime_error("invalid monitoring interval/duration");
    }

    info_logger.write("[info] monitoring app created successfully with interval: " + std::to_string(m_monitoring_interval) + 
                "s and duration: " + std::to_string(m_monitoring_duration) + "s");

}


ProccesInfo LoadProccessInfo(size_t pid, const std::filesystem::path& proc_path) 
{
    std::string p_name;
    double memory_usage = 0;
    double cpu_usage = 0;

    //name extraction
    std::ifstream comm_file(proc_path / "comm");
    if (comm_file.is_open()) 
    {
        std::getline(comm_file, p_name);
    }
    else
    {
        info_logger.write("[warning] comm_file is not open for proccess with pid: " + std::to_string(pid));
        
    }
    //memory usage calculation
    std::ifstream status_file(proc_path / "status");
    std::string line;
    
    while (std::getline(status_file, line)) 
    {
        if (line.rfind("VmRSS:", 0) == 0) 
        {
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;
            memory_usage = std::stod(value) / 1024.0; //(MB)
            break;
        }
    }

    // CPU usage calculation
    std::ifstream stat_file(proc_path / "stat");
    if (stat_file.is_open()) 
    {
        std::string stat_line;
        std::getline(stat_file, stat_line);
        std::istringstream iss(stat_line);
        std::vector<std::string> stats;
        std::string stat;

        while (iss >> stat) 
        {
            stats.push_back(stat);
        }
        if (stats.size() >= 15)
        {
            long utime = std::stol(stats[13]);
            long stime = std::stol(stats[14]);
            cpu_usage = static_cast<double>(utime + stime) / sysconf(_SC_CLK_TCK);
        }
    }
    else
    {
        info_logger.write("[warning] stat_file is not open for proccess with pid: " + std::to_string(pid));

    }

    return ProccesInfo(pid, p_name, cpu_usage, memory_usage, std::time(nullptr));
}

//insert/update p_info
void MonitorApp::UpdateMap(const ProccesInfo& p_info) 
{
    auto it = m_p_info_map.find(p_info.pid);
    
    if (it != m_p_info_map.end()) 
    {
        info_logger.write("[info] updating process info for pid: " + std::to_string(p_info.pid));
        it->second = p_info; 
    }
    else 
    {
        info_logger.write("[info] inserting new process info for pid: " + std::to_string(p_info.pid));
        m_p_info_map[p_info.pid] = p_info;
    }
}


//collect proccesses from proc update the map 
void MonitorApp::CollectProccesses(const std::set<size_t>& excluded_pids)
{
    for (const auto& entry : std::filesystem::directory_iterator("/proc")) 
    {
        if (!entry.is_directory())
        {
            continue;
        } 

        std::string filename = entry.path().filename();
        
        if (!std::all_of(filename.begin(), filename.end(), ::isdigit))
        {
            continue;
        } 

        size_t pid = std::stoul(filename);

        if (excluded_pids.find(pid) != excluded_pids.end())
        {
            continue;
        } 

        ProccesInfo p_info = LoadProccessInfo(pid, entry.path());
        UpdateMap(p_info);
    }
}


void MonitorApp::StartMonitoring() 
{
    using namespace std::chrono;

    while(!m_should_stop)
    {
        static size_t monitor_num = 0; 

        m_p_info_map.clear(); 

        monitor_num++; 
        info_logger.write("[info] monitor num" + std::to_string(monitor_num) + " started");


        sleep(m_monitoring_interval); 
        
        auto start_time = steady_clock::now();
        while (duration_cast<seconds>(steady_clock::now() - start_time).count() < m_monitoring_duration) 
        {
            CollectProccesses(excluded_pids);
        }    
        

        // write updated map to CSV file
        CSV_file.write("monitor_num: " + std::to_string(monitor_num)); 
        CSV_file.write("pid,p_name,cpu_usage,memory_usage,timestamp");
        
        float avg_cpu_usage = 0;
        float avg_memory_usage = 0;
        size_t process_count = 0;

        for(auto map_member : m_p_info_map)
        {
            CSV_file.write(map_member.second.ToString()); //enter p_info string
            
            
            // calculate average CPU and memory usage
            avg_cpu_usage += map_member.second.cpu_usage;
            avg_memory_usage += map_member.second.memory_usage;
            process_count++;
            
        }
        
        info_logger.write("[info] process info written to CSV file for monitor num: " + std::to_string(monitor_num));

        avg_cpu_usage /= process_count; 
        avg_memory_usage /= process_count; 

        //enter averages to database
        if (FAIL == m_db.InsertAvgInfo(monitor_num, avg_cpu_usage, avg_memory_usage) ) 
        {
            info_logger.write("[error] failed to insert average info into database for monitor num: " + std::to_string(monitor_num));
            throw std::runtime_error("failed to insert average info into database");
        }
        

    }
    
}


void MonitorApp::StopMonitoring()
{
    m_should_stop = true;
    info_logger.write("[info] monitoring stopped");    
}


