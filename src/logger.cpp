#include "logger.hpp"

Logger::Logger(const std::string& filepath)
{
    target_file.open(filepath, std::ios::out | std::ios::trunc); // trunc to clear previous sessions
    
    if (!target_file.is_open()) 
    {
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



