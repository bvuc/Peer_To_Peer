# Peer_To_Peer Messagin and Broadcasting

# Objective
this project is to develop a client-server application that allows peer-to-peer messaging and broadcast messaging. The client application should be able to connect to the server, authenticate itself with a password, and see a list of active clients. The client can choose an active client to connect with, and the remote client will be prompted to approve the connection. Once the connection is established, clients can communicate with each other without the server's involvement.

# Features
1. Message Broadcasting.
2. List of active clients.
3. Peer to peer messaging.
4. User loginn or signup.
5. Password hashing.
6. Require login after 30min.

# Prerequisites
1. C++ compiler is required.
2. need to import many libraries.

# Execution Procedure
1. Open the terminal in the file location.
2. Compile the server code: g++ server.cpp -o server pthread
3. run the server: ./server
4. The server will start listening.
5. open new tereminal and compile client code: g++ client.cpp -o client pthread
6. run the client: ./client

Now you will have several options to decide. I am attaching some screenshots of a case where there list of client and broadcasting and peer to peer messagin is done.
https://github.com/bvuc/Peer_To_Peer/blob/main/1.jpg
