//64028780
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>    // std::find
using namespace std;
vector<int> clientSockets;
mutex clientMutex;

void handleClient(int clientID, int clientSocket) {
    const int BUFFER_SIZE = 1024;

    while (true) {
        // Receive and print the message from the client
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (bytesRead == -1) {
            cerr << "Error receiving message from client " << clientID << "\n";
            break;
        
        } else {
            buffer[bytesRead] = '\0';  // Null-terminate the received data
             cout << "[" << clientID << "] recv: \"" 
             << buffer << "\" ("<< strlen(buffer) + 1<< " bytes)"<<endl;

            ssize_t bytesSent = send(clientSocket, buffer, bytesRead, 0);
            if (bytesSent == -1) {
                cerr << "Error sending response to client " << clientID << "\n";
                break;
            }
           cout << "[" << clientID << "] send: \"" 
            << buffer << "\" ("<< strlen(buffer) + 1<< " bytes)"<<endl;

            if (strcmp(buffer, "exit") == 0) break;
            
        }
            
    }

    // Close the client socket
    close(clientSocket);

    // Remove the client ID from the list
    lock_guard<mutex> lock(clientMutex);
    auto it = find(clientSockets.begin(), clientSockets.end(), clientID);
    if (it != clientSockets.end()) 
        clientSockets.erase(it);
     
}

int main() {
    const int PORT = 8081;
    const int BACKLOG = 5;

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error creating socket\n";
        return -1;
    }

    // Set up the server address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to the specified address and port
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        cerr << "Error binding socket\n";
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, BACKLOG) == -1) {
        cerr << "Error listening for connections\n";
        close(serverSocket);
        return -1;
    }

    cout << "Server is listening on port " << PORT << "...\n";

    int clientID = 0;

    while (true) {
        // Accept a connection
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
        if (clientSocket == -1) {
            cerr << "Error accepting connection\n";
            continue;  // Continue listening for the next connection
        }

        // Assign a unique ID to the client
        lock_guard<mutex> lock(clientMutex);
        clientSockets.push_back(clientID);

        // Start a thread to handle the client
        thread(handleClient, clientID, clientSocket).detach();

        // Send the assigned ID to the client
        string idMessage = to_string(clientID);
        ssize_t bytesSent = send(clientSocket, idMessage.c_str(), idMessage.length(), 0);
        if (bytesSent == -1) {
            cerr << "Error sending ID message to client " << clientID << "\n";
            close(clientSocket);
        }
        
        cout<<"["<<clientID<<"] send: \""
        <<idMessage<<"\" ("<< idMessage.length() + 1<<" bytes)"<<endl;
        
        // Increment the client ID for the next client
        ++clientID;
    }

    // The server will never reach this point in this example
    close(serverSocket);

    return 0;
}
