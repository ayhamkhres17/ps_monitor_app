#include <ctime> //time_t
#include <unordered_map>  //std::unordered_map

#include "database.hpp"
#include "logger.hpp"
#include "configure_info.hpp"

 
class ProccesInfo //info for each proccess
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


class MonitorApp //app
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


