#include <ctime> //time_t
#include <string> //std::string
#include <vector> //std::vector
#include <fstream> //std::ofstream
#include <sqlite3.h> //sqlite3
#include <unordered_map>  //std::unordered_map
#include <set> //std::set


enum Status
{
    SUCCESS = 0,
    FAIL = 1
};

class ProccesInfo
{
public:    
    size_t pid;
    std::string p_name;
    double cpu_usage;    
    double memory_usage; 
    time_t timestamp;
    
    explicit ProccesInfo(size_t pid = 0, const std::string& p_name = "", double cpu_usage = 0, double memory_usage = 0, time_t timestamp = time(nullptr))
        : pid(pid), p_name(p_name), cpu_usage(cpu_usage), memory_usage(memory_usage), timestamp(timestamp) {}

    ProccesInfo(const ProccesInfo& other) = default;
    ProccesInfo& operator=(const ProccesInfo& other) = default;
   
    bool operator==(const ProccesInfo& other) const
    {
        return pid == other.pid;

    } //for std::find
    
    std::string ToString() const
    {
        char time_string[std::size("yyyy-mm-dd hh:mm:ss")];
        std::strftime(std::data(time_string), std::size(time_string),
                "%F %T", std::gmtime(&timestamp)); 

        return std::to_string(pid) + "," + p_name + "," + std::to_string(cpu_usage) + "," + std::to_string(memory_usage) + "," + time_string;
    }
    

};

class ConfigureInfo //configuration information , responsibility:to load config information from the JSON file
{

public:
    explicit ConfigureInfo(const std::string& json_file_path);

    ~ConfigureInfo() noexcept = default;

    ConfigureInfo(const ConfigureInfo& other) = delete;
    ConfigureInfo& operator=(const ConfigureInfo& other) = delete;

    size_t  GetMonitoringInterval() const { return monitoring_interval; }  
    size_t  GetMonitoringDuration() const { return monitoring_duration; } //for testing purposes
    const std::string&  GetDBFilePath() const { return db_file_path; } //for testing purposes
    const std::string&  GetCSVFilePath() const { return csv_file_path; } //for testing purposes
    const std::set<size_t>& GetExcludedProcesses() const { return excluded_processes; }  //for testing purposes

 
private:
    size_t monitoring_interval;
    size_t monitoring_duration;

    // std::string dbHost;
    // std::string dbPort;
    // std::string dbName;
    // std::string dbUser;
    // std::string dbPassword;

    friend class MonitorApp; 


    
    // wer'e using sqlite3 , which does'nt need host,port, name ,user, password we need a filepath
    std::string db_file_path;
    std::string csv_file_path;

    std::set<size_t>  excluded_processes; //by pid

};


class Database
{
public: 
    explicit Database(const std::string& db_file_path);
    ~Database() noexcept;
    
    Database(const Database& other) = delete;
    Database& operator=(const Database& other) = delete;

    Status InsertAvgInfo(int monitor_num, double avg_cpu_usage, double avg_memory_usage);
    sqlite3* GetDb() const { return db; } //for testing
private:
    sqlite3 *db;

};


class Logger 
{

public:

    explicit Logger(const std::string& filepath);
    ~Logger() noexcept;


    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;

    void write(const std::string str); 

private:
    std::ofstream target_file;
    
};





class MonitorApp
{
public:
    explicit MonitorApp(const ConfigureInfo& config_info);
    ~MonitorApp() noexcept = default;

    MonitorApp(const MonitorApp& other) = delete;
    MonitorApp& operator=(const MonitorApp& other) = delete;
    
    void StartMonitoring();
    void StopMonitoring();

private:
    
    volatile bool m_should_stop;

    Logger CSV_file;

    Database m_db;
    std::unordered_map<size_t, ProccesInfo> m_p_info_map;

    
    size_t m_monitoring_interval;
    size_t m_monitoring_duration;
    std::set<size_t> excluded_pids; 



    void UpdateMap(const ProccesInfo& p_info);
    void CollectProccesses(const std::set<size_t>& excluded_pids);

};


