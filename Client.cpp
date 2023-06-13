#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#define BUF_LEN 1024

int main() {
	// Initialize Winsock
	WSADATA wsa_data;
	int op_code = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (op_code != 0) {
		printf("WSAStartup failed with error: %d\n", op_code);
		return -1;
	}

	SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO::IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) {
		std::cout << "Create socket error: " << WSAGetLastError() << '\n';
		return -1;
	}

	sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(8080);

	// connect to server
	op_code = connect(_socket, reinterpret_cast<sockaddr*>(&server), sizeof(server));
	if (op_code == SOCKET_ERROR) {
		std::cout << "Connect error " << WSAGetLastError() << '\n';
		return -1;
	}

	std::string hello("Hello from client!");
	op_code = send(_socket, hello.data(), (int)hello.size(), 0);
	if (op_code == SOCKET_ERROR) {
		std::cout << "Send failed with error: " << WSAGetLastError() << '\n';
	}
	
	// shutdown the connection since no more data will be sent
	op_code = shutdown(_socket, SD_SEND);
	if (op_code == SOCKET_ERROR) {
		std::cout << "Shutdown error " << WSAGetLastError() << '\n';
		return -1;
	}

	char buffer[BUF_LEN] = {};
	op_code = recv(_socket, buffer, BUF_LEN, 0);
	if (op_code > 0) {
		std::cout << "Bytes from server received: " << buffer << '\n';
	}
	else if (op_code == 0) {
		std::cout << "Connection closed\n";
	}
	else { 
		std::cout << "Recv failed with error " << WSAGetLastError() << '\n';
	}
	// cleanup
	closesocket(_socket);
	WSACleanup();
}