# ps_monitor_app
#publisher ayham_khres

# Description
c++ program that monitors running proccesses on the operationg system over time, saves results to a CSV file , and stores summarized results in a database
i used sqlite3 for database.

# Output
proccess info in each line of the CSV , contains: pid, p_name, cpu_usage, memory_usage and timestamp
in database: monitor cycle number, average cpu_usage , average memory usage (summary of the monitor cycle)

# AI usage

used chatgpt to implement the function that extracts proccess information from the os , it gave me a long while loop which i divided to functions , added the saves to the map, and writing to the file at the end

<img width="1590" height="148" alt="Screenshot 2025-07-10 at 15-45-10 Process Monitoring Program" src="https://github.com/user-attachments/assets/e6838065-68b6-414b-8b17-2de2330faba2" />

<img width="791" height="440" alt="Screenshot 2025-07-10 at 15-46-39 Process Monitoring Program" src="https://github.com/user-attachments/assets/4a7b4def-b505-433c-bd0d-20ce0d18425a" />

<img width="794" height="827" alt="Screenshot 2025-07-10 at 15-47-18 Process Monitoring Program" src="https://github.com/user-attachments/assets/ce0681c8-0c92-42bf-9f99-fb76e7359b9d" />

used copilot to explain to me how to read from a csv file for TestCSVWrite function copied the code and modified it for usage in my function

<img width="771" height="738" alt="Screenshot from 2025-07-13 08-59-07" src="https://github.com/user-attachments/assets/38a1f355-2a5d-4bc7-802b-b055071ea605" />


