
#include "database.hpp"

Database::Database(const std::string &db_file_path):info_logger("./info_logger.txt")
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


