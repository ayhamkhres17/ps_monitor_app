#include <iostream>
#include "os_ps_monitor.hpp"

#include <csignal>
#include <atomic>

MonitorApp* g_monitor_ptr = nullptr;

void signal_handler(int signal);


int main()
{
    try 
    {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        
        ConfigureInfo config_info("./config.json");
        g_monitor_ptr = new MonitorApp(config_info);
        g_monitor_ptr->StartMonitoring();
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "error: " << e.what() << std::endl;
        return 1; // Return a non-zero value to indicate failure
    }

    return 0;
}

void signal_handler(int signal) 
{
    if (signal == SIGINT) 
    {
        g_monitor_ptr->StopMonitoring();
        
        if(g_monitor_ptr) 
        {
            delete g_monitor_ptr;
            g_monitor_ptr = nullptr;
        }
        
        exit(0);
    }
}

