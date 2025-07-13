#ifndef __DB__
#define __DB__

#include <string>
#include <sqlite3.h> //sqlite3

#include "logger.hpp"

enum Status
{
    SUCCESS = 0,
    FAIL = 1
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
    Logger info_logger;

};

#endif 