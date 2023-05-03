// Libraries
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <ctime>
#include <netdb.h>
#include <map>
#include <thread>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <vector>
 

int total=0;

// maximum number of connections
#define MAX_CONNECTIONS 50

// message buffer size
#define BUFFER_SIZE 1024

// broadcast time limit in seconds
#define BROADCAST_TIME_LIMIT 1800
std::vector<std::thread> threads;
std::vector<int> Num_clients;


// client information struct
struct client_info {
    std::string username;
    std::string password;
    std::string ip_address;
    int tcp_socket;
};

std::vector<client_info> clients;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// UDP socket descriptor
int udp_socket;

// function prototypes
void handle_new_client(int client_socket);

std::vector<std::string>IP_Retrieve()
{
char hostbuffer[256];
std::vector<std::string> g1;
	struct hostent *host_entry;
	int hostname;
	struct in_addr **addr_list;
	sockaddr_in addr {};
	socklen_t len = sizeof(addr);
	hostname = gethostname(hostbuffer, sizeof(hostbuffer));
	host_entry = gethostbyname(hostbuffer);
	if (host_entry == NULL) {
		perror("gethostbyname error");
		exit(1);
	}
	addr_list = (struct in_addr **)host_entry->h_addr_list;
	        for(int i=0;i<total;i++){
		 g1.push_back("Client "+ std::to_string(i+1) + ":" + inet_ntoa(*addr_list[0]));
		}
		std::cout << "Port number: " << ntohs(addr.sin_port) << std::endl;
return g1;
}
void accept_clients(int tcp_socket) {
    while (true) {
        int client_socket, client_addr_len;
        struct sockaddr_in client_address;

        // Accept new client connection
        client_addr_len = sizeof(client_address);
        if ((client_socket = accept(tcp_socket, (struct sockaddr*)&client_address, (socklen_t*)&client_addr_len)) < 0) {
            std::cerr << "Error accepting new connection." << std::endl;
            continue;
        }
	std::cout << "Port number: " << ntohs(client_address.sin_port) << ":"<<inet_ntoa(client_address.sin_addr)<<std::endl;
        // Create a new thread to handle the new client connection
        std::thread client_thread(handle_new_client, client_socket);
        client_thread.detach(); // detach thread to run independently
        Num_clients.push_back(client_socket);
    }
}

void send_p2p_request(int client_socket, const vector<client_info> &clients, int target_port)
{
    auto target = find_if(clients.begin(), clients.end(), [target_port](const client_info &client)
                          { return client.port == target_port; });
    if (target != clients.end() && target->authenticated && !target->in_p2p_session)
    {
        writeString(target->fd, "P2PREQUEST");
        writeString(target->fd, to_string(client_socket));
    }
    else
    {
        writeString(client_socket, "P2P request failed. Target client not available.");
    }
}

void accept_p2p_request(int client_socket, int target_port, vector<client_info> &clients)
{
    auto target = find_if(clients.begin(), clients.end(), [target_port](const client_info &client)
                          { return client.port == target_port; });
    if (target != clients.end() && target->authenticated && !target->in_p2p_session)
    {
        target->in_p2p_session = true;
        auto current_client = find_if(clients.begin(), clients.end(), [client_socket](const client_info &client)
                                      { return client.fd == client_socket; });
        current_client->in_p2p_session = true;
        writeString(client_socket, "P2P session established.");

        // Start peer-to-peer messaging between the two clients.
        thread p2p_thread1(handle_peer_to_peer, client_socket, target->fd, ref(clients));
        thread p2p_thread2(handle_peer_to_peer, target->fd, client_socket, ref(clients));
        p2p_thread1.join();
        p2p_thread2.join();

        current_client->in_p2p_session = false;
        target->in_p2p_session = false;
    }
    else
    {
        writeString(client_socket, "P2P session failed. Target client not available.");
    }
}


using namespace std;
int main() {
    int tcp_socket;
    int temp=0;

    struct sockaddr_in tcp_address;
    // Create TCP socket
    if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error creating TCP socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind socket to address
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_address.sin_port = htons(4070); // use port 8080
    if (bind(tcp_socket, (struct sockaddr*)&tcp_address, sizeof(tcp_address)) < 0) {
        std::cerr << "Error binding TCP socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(tcp_socket, MAX_CONNECTIONS) < 0) {
        std::cerr << "Error listening for connections." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections in separate threads
    
    
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        threads.push_back(std::thread(accept_clients, tcp_socket));
    }

    // Join threads (never reached in this example)
    for (auto& t : threads) {
        t.join();
    }

    if (temp==1) {
            while (true)
    {
        int num_clients = clients.size();
        if (num_clients >= max_clients)
        {
            // Reject new connections if the maximum number of clients allowed is reached
            cout << "Maximum number of clients reached. Rejecting new connection.\n";
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
            if (client_socket == -1)
            {
                perror("Failed to accept client connection");
                return 1;
            }
            writeString(client_socket, "Maximum number of clients reached. Try again later.\n");
            close(client_socket);
            continue;
        }
        read_fds = active_fds;
        if (select(FD_SETSIZE, &read_fds, nullptr, nullptr, nullptr) == -1)
        {
            perror("Failed to select sockets");
            return 1;
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_socket)
                {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
                    if (client_socket == -1)
                    {
                        perror("Failed to accept client connection");
                        return 1;
                    }
                    FD_SET(client_socket, &active_fds);
                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                    int client_port = ntohs(client_addr.sin_port);
                    printf("New connection from %s:%d\n", client_ip, client_port);

                    clients.push_back({client_socket, client_ip, client_port, false});
                    sendClientList(client_socket, clients);

                    writeString(client_socket, "Welcome to the chat server. Please enter your choice:\n1. Login\n 2. Sign up\n");
                }
                else
                {
                    string message = readString(i);
                    if (message.empty())
                    { // The client has disconnected.
                        // Remove the disconnected client from the list.
                        for (auto it = clients.begin(); it != clients.end(); ++it)
                        {
                            if (it->fd == i)
                            {
                                clients.erase(it);
                                break;
                            }
                        }
                        // Close the socket and remove it from the active file descriptor set.
                        close(i);
                        FD_CLR(i, &active_fds);
                    }
                    else
                    {

                        auto client_it = find_if(clients.begin(), clients.end(), [i](const client_info &client)
                                                 { return client.fd == i; });

                        // If the client is not authenticated, process the login or signup request.
                        if (client_it != clients.end() && !client_it->authenticated)
                        {
                            int choice = stoi(message);
                            if (choice == 1)
                            {
                                string username = readString(i);
                                string password = readString(i);

                                if (authenticateUser(username, password))
                                {
                                    cout << "User " << username << " logged in" << endl;
                                    writeString(i, "Login successful");
                                    client_it->authenticated = true;
                                    sendClientList(i, clients);
                                }
                                else
                                {
                                    cerr << "Failed to authenticate user " << username << endl;
                                    writeString(i, "Login failed");
                                }
                            }
                            else if (choice == 2)
                            {
                                string username = readString(i);
                                string password = readString(i);

                                if (addUser(username, password))
                                {
                                    cout << "User " << username << " added to the database" << endl;
                                    writeString(i, "User added to database");

                                    // Prompt the client to choose between login and signup again after a successful signup.
                                    writeString(i, "Welcome to the chat server. Please enter your choice:\n1. Login\n2. Sign up\n");
                                }
                                else
                                {
                                    cerr << "Failed to add user " << username << " to the database" << endl;
                                    writeString(i, "Failed to add user to database");
                                }
                            }

                            else
                            {
                                writeString(i, "Invalid choice. Please try again.");
                            }
                        }
                        else
                        {
                            // If the client is authenticated, broadcast the message.
                            for (const client_info &client : clients)
                            {
                                if (client.fd != server_socket && client.fd != i && client.authenticated)
                                {
                                    writeString(client.fd, message);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    }

    return 0;
}
void handle_new_client(int client_socket) {
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, 1024, 0);
    std::string request(buffer, bytes_received);

    // Respond with login/signup prompt
    std::string response = "Please choose an option: login or signup\n";
    send(client_socket, response.c_str(), response.length(), 0);

    while (true) {
        bytes_received = recv(client_socket, buffer, 1024, 0);
        std::string choice(buffer, bytes_received);

        if (choice == "login") {
            // Ask for username and password
            response = "Enter username:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                // error handling
                return;
            }
            std::string username(buffer, bytes_received);

            response = "Enter password:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                // error handling
                return;
            }
            std::string password(buffer, bytes_received);

            // authentication
             // Check authentication from CSV file
            std::ifstream file("users.csv");
            std::string line;
            bool authenticated = false;
            while (std::getline(file, line)) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    std::string file_username = line.substr(0, pos);
                    std::string file_password = line.substr(pos+1);
                    if (file_username == username && file_password == password) {
                        authenticated = true;
                        break;
                    }
                }
            }
            file.close();

            if (authenticated) {
                total++;
                response = "Login successful!\n";
                
                send(client_socket, response.c_str(), response.length(), 0);
                
                 // Show options to the authenticated client
        response = "You are authenticated. Choose an option:\n1. List of active clients\n2. Broadcast a message\n3. Direct message to client";
        send(client_socket, response.c_str(), response.length(), 0);

        bytes_received = recv(client_socket, buffer, 1024, 0);
        if (bytes_received < 0) {
            // error handling
            return;
        }
        std::string option(buffer, bytes_received);

        if (option == "1") {
           vector<string> a=IP_Retrieve();
      
           
            // List active clients
            std::string active_clients_list = "Active Clients:\n";
            for (const auto& client : a) {
                active_clients_list +=   client +"\n";
            }
            send(client_socket, active_clients_list.c_str(), active_clients_list.length(), 0);
            bytes_received = recv(client_socket, buffer, 1024, 0);
        if (bytes_received < 0) {
            // error handling
            return;
        }
        std::string ipadd(buffer, bytes_received);
        

        } else if (option == "2") {
            // Broadcast a message
            response = "Enter broadcast message:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                // error handling
                return;
            }
            std::string msg(buffer, bytes_received);
            std::string broadcast_message = "Message Broadcasted " + std::to_string(client_socket-2) + "\nMessage ::\n"+msg; 
             for (int client_socket : Num_clients) {
                    std::cout << " client no. is : " <<  client_socket << "\n";
            send(client_socket, broadcast_message.c_str(), broadcast_message.size(), 0);
                }
            response = "broadcasting successfull!\n";
            send(client_socket,response.c_str(), response.length(), 0);
} 
else if (option == "3") {
            //peer_to_peer_communicationelse()
            }else{
// Invalid option, terminate client's connection
response = "Invalid option. Terminating connection...\n";
send(client_socket, response.c_str(), response.length(), 0);
close(client_socket);
return;
}

            } else {
                response = "Login failed. Invalid username or password.\n";
                send(client_socket, response.c_str(), response.length(), 0);
            }
        } 


            else if (choice == "signup") {
            // Ask for new username and password
            response = "Enter new username:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                // error handling
                return;
            }
            std::string new_username(buffer, bytes_received);

            // Check if username already exists in CSV file
            std::ifstream file("users.csv");
            std::string line;
            bool username_exists = false;
            while (std::getline(file, line)) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                std::string username_from_file = line.substr(0, pos);
                if (new_username == username_from_file) {
                username_exists = true;
                    break;
                    }
                    }
                        }
                file.close();

                 if (username_exists) {
            response = "Username already exists. choose another username.\n";
            send(client_socket, response.c_str(), response.length(), 0);
        } else {
            response = "Enter new password:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                // error handling
                return;
            }
            std::string new_password(buffer, bytes_received);

            response = "re-enter your password:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                // error handling
                return;
            }
            std::string confirmed_password(buffer, bytes_received);

            if (new_password != confirmed_password) {
                response = "Password incorrect. try again.\n";
                send(client_socket, response.c_str(), response.length(), 0);
            } else {
                // Write username and password to CSV file
                std::ofstream outfile;
                outfile.open("users.csv", std::ios_base::app);
                outfile << new_username << "," << confirmed_password << std::endl;
                outfile.close();

                response = "Signup successful. login to continue.\n";
                send(client_socket, response.c_str(), response.length(), 0);
            }
        }

        } else {
            // Invalid choice
            response = "Invalid option. choose login or signup\n";
            send(client_socket, response.c_str(), response.length(), 0);
        }
    } 
}
