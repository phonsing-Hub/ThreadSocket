//64028780
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;
int main() {
    const int PORT = 8081;
    const char* SERVER_IP = "127.0.0.1";
    const int BUFFER_SIZE = 1024;

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error creating socket\n";
        return -1;
    }

    // Set up the server address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddress.sin_port = htons(PORT);

    // Connect to the server
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        cerr << "Error connecting to the server\n";
        close(clientSocket);
        return -1;
    }

    // Receive and print the assigned ID from the server
    char buffer[BUFFER_SIZE];
    char clientID[BUFFER_SIZE];

    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead == -1) 
        cerr <<"Error receiving ID message\n";
     else if (bytesRead == 0) 
        cout << "Server disconnected\n";
	else {
        buffer[bytesRead] = '\0';  // Null-terminate the received data
        // Display the client ID when connected
        string clientIDStr = buffer;
        try {
            int clientID = stoi(clientIDStr);
            cout << "[" << clientID << "] You are connected!\n";
            while (true) {
                // Read input from the keyboard
                string userInput;
                cout << "[" << clientID << "] Enter message: ";
                getline(cin, userInput);

                // Send the message to the server
                ssize_t bytesSent = send(clientSocket, userInput.c_str(), userInput.length(), 0);
                if (bytesSent == -1) {
                    cerr << "Error sending message\n";
                    break;
                }
                else 
                    cout << "[" << clientID << "] send: \"" 
                    << userInput << "\" ("<< userInput.length() + 1<< " bytes)"<<endl;

                // Receive and print the response from the server
                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead == -1) {
                    cerr << "Error receiving response\n";
                    break;
                } else if (bytesRead == 0) {
                    cout << "Server disconnected\n";
                    break;
                } else {
                    buffer[bytesRead] = '\0';  // Null-terminate the received data
                     cout << "[" << clientID << "] recv: \"" 
                    << buffer << "\" ("<< strlen(buffer) + 1<< " bytes)"<<endl;
                }
                
                if (userInput == "exit") 
                    break;
                
            }
        } catch (const invalid_argument& e) {
            cerr << "Error converting client ID to integer: " << e.what() << "\n";
        } catch (const out_of_range& e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }

    // Close the socket
    close(clientSocket);

    return 0;
}