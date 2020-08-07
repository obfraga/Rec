#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int sock = WSAStartup(ver, &wsData);
	if (sock != 0) {
		cerr << "Can't initialize WinSock! Quitting!" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting!" << endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		cout << host << " connected on port " << service << endl;
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	closesocket(listening);

	//While loop: accept and echo message back to client
	char buf[4096];

	while (true) {
		ZeroMemory(buf, 4096);
		// Wait for a client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv(). Quitting!" << endl;
			break;
		}

		if (bytesReceived == 0) {
			cout << "Client disconnected " << endl;
			break;
		}

		//Echo message back to client;
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// Close the socket
	closesocket(clientSocket);
	// Shutdown winsock
	WSACleanup();
}