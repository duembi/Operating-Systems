
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     

#include <unistd.h>         //  POSIX os functions
#include <sys/socket.h>    // socet programming base func and struct (socket, bind, listen, accept, sockaddr_in)
#include <netinet/in.h>   //For internet adress(INADDR_ANY, htons vb.)
#include <arpa/inet.h>  

#include <netdb.h>      // Að veritabaný iþlemleri (gethostbyname, hostent) [Hostname'i IP'ye çevirmek için]
#include <getopt.h>    // To process command line arguments (like -p) (getopt)

void error(const char *msg) {
    perror(msg); 
    exit(1);     
}


int main(int argc, char *argv[]) {
    int sockfd;            // sockfd: server listening soc, newsockfd: Client talk soc
    int portno;            // port num
    struct sockaddr_in serv_addr; // Server adrs info (IP, port) 
    struct hostent *server; // hostname IP adresss
    int opt;               // getopt() 
    char *hostname = NULL; // connect server name (like "localhost", "127.0.0.1")

   
    
    
    portno = -1; 
    
    while ((opt = getopt(argc, argv, "h:p:")) != -1) {
    	
    	
        switch (opt) {
        	 case 'k': //                                                                          !!!!!!!!!!!!!!!!!                      !!!!!!!!!!!!!!!!!!
                portno = atoi(optarg); 
                break;
                
            case 'h':
                hostname = optarg; 
                break;
                
                
            case 'p':
                portno = atoi(optarg); 
                break;
                
                
            case 'j'://                                                                      !!!!!!!!!!!!!!!!!                       !!!!!!!!!!!!!!!!!!
                portno = atoi(optarg); 
                break;    
                
            default: //enter without to -p 
                fprintf(stderr, "Kullaným: %s -h <hostname> -p <port_num>\n", argv[0]);
                exit(EXIT_FAILURE); 
        }
    }

    if (portno <= 0 || portno > 65535 || hostname == NULL) { // Standart port interval (1-65535)
    
        fprintf(stderr, "ERROR: The arguments -h <hostname> and valid -p <port_number> (1-65535) are required\n");
        
        exit(1); 
    }

    // CLIENT SOCKET INSTALLATION STEPS

    // ----- ADIM 1: CREAT SOCKET -----
    // AF_INET: Adres Fam (IPv4)
    // SOCK_STREAM: Soket typ (TCP)
    // 0: Prtcl (TCP for 0)
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
    	
        error("Error : socket creation failed"); // if socket() -1 is failed
        
    }

    // ---- STP 2: gethostbyname ----
    // "localhost", "127.0.0.1" ---> IP 
    server = gethostbyname(hostname);
    
    if (server == NULL) {
    	
        fprintf(stderr, "HATA: Belirtilen sunucu bulunamadý (%s)\n", hostname);
        
        close(sockfd); // if there is an error close the socket
        exit(1);
        
    }

    // resset serv_addr
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET; // fam : IPv4

    // IP form gethostbyname  ----> 'serv_addr'
    if (server->h_addrtype == AF_INET && server->h_length == sizeof(struct in_addr)) {
    	
         bcopy((char *)server->h_addr_list[0], //Use first ip from DNS
         
              (char *)&serv_addr.sin_addr.s_addr,
              
              server->h_length);
    } else {
    	
        fprintf(stderr, "ERROR: The serr address couldnt be resolved  IPv4 format.\n");
        
        close(sockfd);
        
        exit(1);
    }

    //Convert pnumber Host Byte Order  ---> Network Byte Order
    serv_addr.sin_port = htons(portno);

    // ----- STEP 3: connect to server -----
    // try sockfd ---> serv_addr
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    	
        close(sockfd); //close socket when there is error
        
        error("ERROR: Failed to connect to server"); //print reason
    }
    fprintf(stderr, "Client: Connected to server (%s:%d).\n", hostname, portno);

    // ----- ADIM 4: DATA COMUNICATION -----

    // Convert (int) ---> (FILE*) 
    
    // 'fgets'/'fprintf' we can use them
    
    FILE *socket_stream = fdopen(sockfd, "r+"); //"r+": use to read(r) and write (+)
    if (socket_stream == NULL) {
    	
        close(sockfd); //close socket if there is error
        
        error("ERROR: fdopen failed");
        
    }
    
    
    // Turn off buffering. Allows data to be sent and received instantly easly.
    setvbuf(socket_stream, NULL, _IONBF, 0);

    char line_buffer[256]; // from input file, Buffer that holds lines read from stdin 
    char recv_buffer[256]; // Buffer that holds the response from the server 
    double max_capacity = -1.0; // ---> max capacity 
    
    int best_channel = 0;       // the order of the best channe
    int channel_count = 1;      // Counter numbor of chanels

    
    // from commend line '< input.txt' --->'stdin' 
   
    while (fgets(line_buffer, sizeof(line_buffer), stdin) != NULL) {

        // check that is line commen line or not
        char first_char;
        
        // use 'sscanf' for read the frst char
        if (sscanf(line_buffer, " %c", &first_char) != 1 || first_char == '#') {
        	
             continue;
             
        }

        // Send the read line to the server
        fprintf(socket_stream, "%s", line_buffer);

        // if the command is finish, exit the loop
        if (strncmp(line_buffer, "finish", 6) == 0) {
        	
            
            fprintf(stderr, "Client: 'finish' command sent.\n");
            break;
            
        }

        // Read calculated capacity from server
        if (fgets(recv_buffer, sizeof(recv_buffer), socket_stream) == NULL) {
        	
            
            fprintf(stderr, "ERROR: No response from server or connection closed.\n");
            break; 
            
        }

        printf("%s", recv_buffer);

        // string ---> atof and flow the best channel
        double current_capacity = atof(recv_buffer);
        if (current_capacity > max_capacity) {
        	
            max_capacity = current_capacity;
            best_channel = channel_count;
            
        }
        channel_count++; 
    }

    printf("Selected Channel: %d\n", best_channel);

    // ----- stp 5: closing -----
    
    // 
    fclose(socket_stream);
    
    //
    fprintf(stderr, "Client: Connection closed.\n");

    return 0; 
}
