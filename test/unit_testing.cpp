#include <iostream>
#include "os_ps_monitor.hpp"


int TestConfigLoad();
int TestDatabaseInsert();
int TestCsvWrite();

int main()
{
    
    if(FAIL == TestConfigLoad())
    {
        std::cerr << "TestConfigLoad failed." << std::endl;
        return 0;

    }

    if(FAIL == TestDatabaseInsert())
    {
        std::cerr << "TestDatabaseInsert failed." << std::endl;
        return 0;

    }

    if(FAIL == TestCsvWrite())
    {
        std::cerr << "TestCsvWrite failed." << std::endl;
        return 0;
    }

    std::cout << "all tests passed" << std::endl;

    return 0;
}



int TestConfigLoad()
{
    try
    {
        ConfigureInfo config_info("./test.json");
        
        if(3 != config_info.GetMonitoringInterval() )
        {
            std::cerr << "error in monitoring interval loading" << std::endl;
            return FAIL;
        }
        if(8 != config_info.GetMonitoringDuration() )
        {
            std::cerr << "error in monitoring duration loading" << std::endl;
            return FAIL;

        }
        if(config_info.GetDBFilePath()!= "test.db")
        {
            std::cerr << "error in DB file path loading" << std::endl;
            return FAIL;
        }

        if(config_info.GetCSVFilePath() != "test.csv")
        {
            std::cerr << "error in CSV file path loading" << std::endl;
            return FAIL;
        }
        if(config_info.GetExcludedProcesses().size() != 2)
        {
            std::cerr << "error in excluded processes loading" << std::endl;
            return FAIL;
        }

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
    
    return SUCCESS;


}    


int TestDatabaseInsert()
{
    try
    {
        Database db("./test.db");
        if(FAIL == db.InsertAvgInfo(11, 3.4, 5.6))
        {
            std::cerr << "insert failed." << std::endl;
            return FAIL;
        }

        //check data integrity
        sqlite3_stmt *stmt;
        sqlite3 *db_ptr = db.GetDb();

        if(!db_ptr)
        {
            std::cerr << "Database pointer is null." << std::endl;
            return FAIL;
        }

        const char *sql = "SELECT monitor_num, avg_cpu_usage, avg_memory_usage FROM avg_info WHERE monitor_num = 11;";
        int rc = sqlite3_prepare_v2(db_ptr, sql, -1, &stmt, nullptr);
        
        if(rc != SQLITE_OK)
        {
            std::cerr << "error"  << sqlite3_errmsg(db_ptr) << std::endl;
            return FAIL;
        }   

        rc = sqlite3_step(stmt);
        if(rc != SQLITE_ROW)
        {
            std::cerr << "No data found for monitor_num 11" << std::endl;
            sqlite3_finalize(stmt);
            stmt = nullptr;

            return FAIL;
        }   

        int monitor_num = sqlite3_column_int(stmt, 0);
        double avg_cpu_usage = sqlite3_column_double(stmt, 1);
        double avg_memory_usage = sqlite3_column_double(stmt, 2);
        
        sqlite3_finalize(stmt);
        stmt = nullptr;

        if(11 != monitor_num)
        {
            std::cerr << "monitor number wrong: instead of 11 got: " << monitor_num << std::endl;
            return FAIL;
        }
        if(3.4 != avg_cpu_usage)
        {
            std::cerr << " CPU usage wrong: instead of 3.4 got: " << avg_cpu_usage << std::endl;
            return FAIL;
        }
        if(5.6 != avg_memory_usage)
        {
            std::cerr << " memory usage wrong: instead of 5.6 got: " << avg_memory_usage << std::endl;
            return FAIL;
        }

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        
        return FAIL;

    }
    return SUCCESS;
}

int TestCsvWrite()
{
    Logger* CSVLogger = new Logger("./test.csv");
    ProccesInfo p_info(1,"test", 0, 0);

    CSVLogger->write(p_info.ToString());
    delete CSVLogger; //FOR CLOSING

    //read input
    std::ifstream file("./test.csv");
    std::string line;
   

    if (file.is_open()) 
    {
        if (std::getline(file, line)) 
        {

            if(line != p_info.ToString())
            {
                std::cout << "read line: " << line  << " expected: " <<  p_info.ToString() << std::endl;

            }
        }
        else 
        {
            std::cerr << "unable to read line" << std::endl;
        }

        file.close();
    }
    else 
    {
        std::cerr << "open file failed" << std::endl;
    }

    return SUCCESS;
}
