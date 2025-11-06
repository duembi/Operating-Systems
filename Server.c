#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     

#include <unistd.h>     // POSIX os functions
#include <sys/socket.h> // socet programming base func and struct (socket, bind, listen, accept, sockaddr_in)
#include <netinet/in.h> // For internet adress(INADDR_ANY, htons vb.)
#include <arpa/inet.h>

#include <math.h>      
#include <getopt.h>     // To process command line arguments (like -p) (getopt)


void error(const char *msg) {
    perror(msg); 
    exit(1);     
}


void handle_client(int sock) {
    char buffer[256]; // A buffer for reading data from the client
    double B, SN, C;  // C: Channel capacity (bit/s) B: Bandwidth (Hz) SN: Signal-to-noise ratio (SNR)


    FILE *socket_stream = fdopen(sock, "r+"); // "r+": use to read(r) and write (+)
    if (socket_stream == NULL) {
        close(sock);
        error("ERROR: fdopen failed");
    }
    
    setvbuf(socket_stream, NULL, _IONBF, 0);

    
    while (fgets(buffer, 255, socket_stream) != NULL) {

        if (strncmp(buffer, "finish", 6) == 0) {
            break; 
        }

        
        if (sscanf(buffer, "%lf %lf", &B, &SN) == 2) { 
          
            C = B * log2(1.0 + SN); // It calculates Channel capacity

           
            //[cite_start]
            fprintf(socket_stream, "%.2f\n", C);
        } 
       
    }

    fclose(socket_stream);
    printf("server : The client conn is closed.\n"); // It prints in the terminal 
}


int main(int argc, char *argv[]) {
    int sockfd, newsockfd; // sockfd: server listening soc, newsockfd: Client talk soc
    int portno;            // port num
    socklen_t clilen;      //size of sockaddr_in
    struct sockaddr_in serv_addr, cli_addr; // Client ve server adresses info
    int opt;           
    
    portno = -1; // 1 to indicate the port is not set
    while ((opt = getopt(argc, argv, "p:")) != -1) { 
        switch (opt) {
        	 case 'k': //                                                                          !!!!!!!!!!!!!!!!!                      !!!!!!!!!!!!!!!!!!
                portno = atoi(optarg); 
                break;
            case 'p':
                portno = atoi(optarg); // convert -p string(optarg) to integer (atoi)
                break;
            case 'j'://                                                                      !!!!!!!!!!!!!!!!!                       !!!!!!!!!!!!!!!!!!
                portno = atoi(optarg); 
                break;
            default: // enter without to -p 
                fprintf(stderr, "Usage: %s -p < port_number >\n", argv[0]);
                
                exit(EXIT_FAILURE); // faile
                
        }
    }

    if (portno <= 0 || portno > 65535) { // Standart port interval (1-65535)
    
        fprintf(stderr, " ERROR: A valid port number (-p) please set in (1-65535).\n\n");
        exit(1); 
        
    }

    // SERVER SOCKET INSTALLATION STEPS

    // stp 1: Create SOKET (socket) 
    // AF_INET: Address family (IPv4)
	// SOCK_STREAM: Socket type (TCP for reliable streaming)
	// 0: Protocol (0 for TCP)
	
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
    	
        error("Error : socket creation failed "); // socket() -1 fail
        
    }

    //reset serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Set the adrss and port info to server will listen
    serv_addr.sin_family = AF_INET;                // fam : IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;        // IP Adrs: Listen all ip (ex: localhost, 192.168.1.5 vb.)
    serv_addr.sin_port = htons(portno);            // Port Num: Change 'portno' "Host Byte Order" ---> "Network Byte Order" (htons)

    // ----- stp 2: CONN SOCKET ---> PORT (bind) -----
  
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    	
        close(sockfd); // close sock when see error
        
        error("ERROR: Could not bind socket to port (port may be in use)");
        
    }

    [cite_start]// ----- ADIM 3: Conn Listen ----- [cite: 711]
    // Soketi pasif listening moduna. Connections can accept from this port
    // 5: Conn queue size (how many clients can wait in line to connect at the same time)
    if (listen(sockfd, 5) < 0) {
    	
       close(sockfd); // close socket when has error
       
       error("ERROR: Could not listen to socket (listen error !!)");
    }
    clilen = sizeof(cli_addr); 

    printf("Server started : port %d, it waiting client connection ...\n", portno);

    // -- step 4: Conn Accept ---
    // the program (blocks) until a client connects. When a client connects, it creates a socket  for communication with client.
   
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    if (newsockfd < 0) {
    	
        close(sockfd); 
        
        error("Accept Error");
        
    }

    printf("Server: Client connected.\n"); 

    // 	STEP 5: COMMUNICATION

    // Delegate communication with the client (receiving/calculating/sending data) to the 'handle_client' function
    // Communication is via 'newsockfd'
    
    handle_client(newsockfd);    
    // ----- STEP 6: CLOSING -----
	// Close the main listening socket (sockfd) finishing 
	// newsockfd is closed within the handle_client function with fclose.
    close(sockfd);
    printf("The server is closing.\n");
    return 0;
}
