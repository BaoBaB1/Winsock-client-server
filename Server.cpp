#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

#define BUF_LEN 1024

std::string get_socket_address(SOCKET s, bool peer) {
	sockaddr_in info;
	int sz = sizeof(info);
	// getpeername is used to retrieve the address and port of the remote peer that socket is connected to.  
	// getsockname is used to retrieve the local address and port assigned to socket
	peer ? getpeername(s, (sockaddr*)&info, &sz) : getsockname(s, (sockaddr*)&info, &sz);
	std::string address;
	address.resize(64);
	inet_ntop(info.sin_family, &info.sin_addr, (char*)address.data(), (int)address.size());
	// for pretty address format XXX.XXX.X.XXX:YYYY
	auto iter = std::find(address.begin(), address.end(), '\0');
	size_t n = std::distance(address.begin(), iter);
	address.insert(n, std::string(":" + std::to_string(info.sin_port)));
	return address;
}

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

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);

	op_code = bind(_socket, (sockaddr*)&addr, (int)sizeof(addr));
	if (op_code == SOCKET_ERROR) {
		std::cout << "Bind socket error: " << WSAGetLastError() << '\n';
		return -1;
	}

	op_code = listen(_socket, SOMAXCONN);
	if (op_code == SOCKET_ERROR) {
		std::cout << "Socket listen error: " << WSAGetLastError() << '\n';
		return -1;
	}

	SOCKET client_socket = accept(_socket, nullptr, nullptr);
	if (client_socket == INVALID_SOCKET) {
		std::cout << "Socket accept error: " << WSAGetLastError() << '\n';
		return -1;
	}

	std::string local_client_address = get_socket_address(client_socket, false);
	std::string client_address_peer = get_socket_address(client_socket, true);
	printf("Client socket local address %s.\nClient socket peer address %s.\n", local_client_address.c_str(), client_address_peer.c_str());
	char buffer[BUF_LEN] = {};
	std::string hello("Hello from server!");
	do {
		op_code = recv(client_socket, buffer, BUF_LEN, 0);
		if (op_code > 0) {
			std::cout << "Bytes from client: " << buffer << '\n';
			op_code = send(client_socket, hello.data(), (int)hello.size(), 0);
			if (op_code == SOCKET_ERROR) {
				printf("Send failed on client socket %I64u with error %d\n", client_socket, WSAGetLastError());
				closesocket(client_socket);
				op_code = -1;
			}
		}
		else if (op_code == 0) {
			std::cout << "Client closed connection with socket " << client_socket << '\n';
			closesocket(client_socket);
		}
		else {
			std::cout << "Error receiving bytes from client socket " << client_socket << '\n';
			closesocket(client_socket);
		}
	} while (op_code > 0);
	// cleanup
	closesocket(_socket);
	WSACleanup();
}
