
#include <fstream>


#include "configure_info.hpp"

using json = nlohmann::json;

ConfigureInfo::ConfigureInfo(const std::string& json_file_path):monitoring_interval(0), monitoring_duration(0),info_logger("./info_logger.txt")
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
        info_logger.write("[error] in configuration");
        throw; 
    }
}

