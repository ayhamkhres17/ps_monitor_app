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
sudo apt install nlohmann-json3-dev

sudo apt install sqlite3

sudo apt-get install libsqlite3-dev

to see the database's content:
sudo apt install sqlitebrowser
then after installation run:
sqlitebrowser (in terminal)

# how to compile and rnu
# in terminal
make

./ps_monitor_app

# how to exit gracefully:
  CTRL+C


# AI usage

used chatgpt to implement the function that extracts proccess information from the os , it gave me a long while loop which i modified and divided to functions , added the saves to the map, and writing to the file at the end
https://chatgpt.com/share/68737a67-2c1c-8000-aabc-677e20dd2177

used copilot.com to explain to me how to read from a csv file for TestCSVWrite function copied the code and modified it for usage in my function

<img width="771" height="738" alt="Screenshot from 2025-07-13 08-59-07" src="https://github.com/user-attachments/assets/38a1f355-2a5d-4bc7-802b-b055071ea605" />

makefile is also made with AI


