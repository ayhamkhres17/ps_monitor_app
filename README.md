# ps_monitor_app
# publisher ayham_khres

# Description
c++ program that monitors running proccesses on the operationg system over time, saves results to a CSV file , and stores summarized results in a database
i used sqlite3 for database.

# Output
proccess info in each line of the CSV , contains: pid, p_name, cpu_usage, memory_usage and timestamp
in database: monitor cycle number, average cpu_usage , average memory usage (summary of the monitor cycle)

# installations needed:
in terminal:
sudo apt install sqlite3

to see the database's content:
sudo apt install sqlitebrowser
then after installation run:
sqlitebrowser (in terminal)

# how to compile and rnu
 g++ -std=c++17 -pedantic-errors -Wall -Wextra -DNDEBUG -O3 src/logger.cpp src/configure_info.cpp src/database.cpp  src/os_ps_monitor.cpp  ps_monitor_app.cpp -Iinclude -lsqlite3 -o ps_monitor_app 
 ./ps_monitor_app

# how to exit gracefully:
  CTRL+C

# or use the executable 
./ps_monitor_app


# AI usage

used chatgpt to implement the function that extracts proccess information from the os , it gave me a long while loop which i modified and divided to functions , added the saves to the map, and writing to the file at the end
https://chatgpt.com/share/68737a67-2c1c-8000-aabc-677e20dd2177

used copilot.com to explain to me how to read from a csv file for TestCSVWrite function copied the code and modified it for usage in my function

<img width="771" height="738" alt="Screenshot from 2025-07-13 08-59-07" src="https://github.com/user-attachments/assets/38a1f355-2a5d-4bc7-802b-b055071ea605" />


