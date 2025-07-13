#ifndef __LOGGER__
#define __LOGGER__

#include <fstream> //std::ofstream

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

#endif