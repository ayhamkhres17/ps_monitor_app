#ifndef __CONINFO__
#define __CONINFO__

#include <string> //std::string
#include <set> //std::set
#include "json.hpp"
#include "logger.hpp"

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

    friend class MonitorApp; //for access to private members

    Logger info_logger;
    // wer'e using sqlite3 , which does'nt need host,port, name ,user, password we need a filepath
    std::string db_file_path;
    std::string csv_file_path;

    std::set<size_t>  excluded_processes; //by pid

};


#endif