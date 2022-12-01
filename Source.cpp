#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <windows.h>
#include <string>
//wsa version
#define SCK_VERSION2 0x0202 //version 2
#define DEFAULT_PORT 55555;

//Save wsa information
WSADATA DATA;
SOCKET ServerSocket;
SOCKADDR_IN ServerInfo;


int main() {
	//iniatilsing wsa
	long SUCCESSFULL = WSAStartup(SCK_VERSION2, &DATA);
	
	if (SUCCESSFULL != 0) {
		std::cout << "WSA Exited with error: " << SUCCESSFULL;
	}
	else {
		std::cout << "WSA Initialised" << std::endl;
	}

	ServerSocket = INVALID_SOCKET;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		std::cout << "Socket error: "<<WSAGetLastError()<<std::endl;
		WSACleanup();
		return EXIT_SUCCESS;
	}
	else {
		std::cout << "Socket has been initialised" << std::endl;
	}

	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_port = htons(55555);
	InetPton(AF_INET, _T("127.0.0.1"), &ServerInfo.sin_addr.s_addr);

	if (bind(ServerSocket, (SOCKADDR*)&ServerInfo, sizeof(ServerInfo))) {
		std::cout << "Error binding: " << WSAGetLastError() << std::endl;
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Succefully bound socket" << std::endl;
	}

	if (listen(ServerSocket, 30) != 0) {
		std::cout << "Error listening: " << WSAGetLastError() << std::endl;
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Succefully listening socket" << std::endl;
	}

	fd_set Master;
	FD_ZERO(&Master);
	FD_SET(ServerSocket, &Master);
	int id = 0;
	while (true) {
		fd_set copy = Master;


		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);


		for (int i = 0; i < socketCount; i++) {
			SOCKET socket = copy.fd_array[i];
			if (socket == ServerSocket) {
				SOCKET client = accept(ServerSocket, nullptr, nullptr);
				FD_SET(client, &Master);
				std::string res = "The server has accepted your request, id:";
				
				res.append(std::to_string(id));
				res.append(";");

				send(client, res.c_str(), res.size(), +1);
			}
			else {
				char buffer[4096];
				ZeroMemory(buffer, 4096);



				int BytesRecieved = recv(socket, buffer, 4096, 0);
				if (BytesRecieved <= 0) {
					closesocket(socket);
					FD_CLR(socket, &Master);
				}
				else {
					//Recieved message from client, processdata and send an update back
					for (int i = 0; i < Master.fd_count; i++) {
						SOCKET SockOut = Master.fd_array[i];
						if (SockOut != ServerSocket && SockOut != socket) {
							send(SockOut, buffer, BytesRecieved, 0); 
						}
					}
				}
			}
		}
	}

	//closing the socket
	closesocket(ServerSocket);
	//cleaning the dll up
	WSACleanup();
	return EXIT_SUCCESS;
}