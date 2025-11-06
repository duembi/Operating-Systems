Shannon Capacity Calculator over TCP Socket in C


🧠 Overview
This project is a simple client-server application developed in C using the POSIX socket API.

Its main purpose is to calculate the Shannon Capacity Limit for a series of network channels:
C = B \times \log_{2}(1 + S/N)

⚙️ Architecture & How It Works
The project consists of two independent programs:

**Server (server)**
* Listens on a specified TCP port and accepts incoming connections.
* Receives Bandwidth (B) and Signal-to-Noise Ratio (S/N) values from the client.
* Calculates the channel capacity (C) using the Shannon formula.
* Sends the calculated result back to the client.

**Client (client)**
* Establishes a TCP connection to the server.
* Reads B and S/N values line by line from standard input (`stdin`) — typically redirected from a file (`<`).
* Sends this data to the server.
* Receives the calculated capacity values and prints them to standard output (`stdout`) — typically redirected to a file (`>`).
* Determines the most efficient channel by selecting the one with the highest capacity among all channels.

💡 Purpose
This application demonstrates:

* Basic network programming concepts using sockets.
* Use of I/O redirection (`<`, `>`) in a Linux environment.

🚀 How to Run
1️⃣ **Compile the programs**
```bash
gcc server.c -o server -lm
gcc client.c -o client -lm

🚀 How to Run
1️⃣ Compile the programs
gcc server.c -o server
gcc client.c -o client

2️⃣ Run the server
./server 8080

3️⃣ Run the client (example)
./client 127.0.0.1 8080 < input.txt > output.txt


input.txt should contain Bandwidth (B) and SNR (S/N) values line by line.

output.txt will store the calculated Shannon capacities.

