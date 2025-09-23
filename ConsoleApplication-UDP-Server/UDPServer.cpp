// UDPServer.cpp
#include "UDPServer.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>  // ����������ͷ�ļ�

UDPServer::UDPServer(int port) : serverSocket(INVALID_SOCKET), clientAddrLen(sizeof(clientAddr)) {
    // ��ʼ��Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }

    // ����socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    // ���÷�������ַ
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // ��socket
    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }

    std::cout << "UDP Server started on port " << port << std::endl;
}

UDPServer::~UDPServer() {
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
    std::cout << "Server stopped." << std::endl;
}

// ��Ӹ�������
static std::string getClientAddress(const sockaddr_in& addr) {
    char ipStr[INET_ADDRSTRLEN];
    const char* result = inet_ntop(AF_INET, &addr.sin_addr, ipStr, sizeof(ipStr));
    if (result == nullptr) {
        return "Unknown";
    }
    return std::string(ipStr) + ":" + std::to_string(ntohs(addr.sin_port));
}

// Get localtime and formulate the string.
static std::string getLocalTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm time_info;
    localtime_s(&time_info, &time_t_now);

    std::ostringstream oss;
    oss << "[" << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << milliseconds.count()
        << "]";

    std::string time_string = oss.str();
    return time_string;

}

void UDPServer::start() {
    char buffer[1024];

    std::cout << "Server is listening for messages..." << std::endl;

    while (true) {
        // ʹ���ִ��ķ�ʽ��ȡ�ͻ��˵�ַ��Ϣ
        std::string clientInfo = getClientAddress(clientAddr);

        // ��������
        clientAddrLen = sizeof(clientAddr);
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0,
            reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrLen);

        // ��ȡ��ǰʱ����ַ���
		std::string time_string = getLocalTimeString();

        if (bytesReceived == SOCKET_ERROR) {
            int error = WSAGetLastError();

            // �ر���10054����
            if (error == 10054) {
                // ����UDP�е��������󣬿ͻ��˿����Ѿ��ر�
                // ����ѡ���¼����ȫ����
                continue;
            }
            else {
                std::cerr << "Receive failed: " << error << std::endl;
            }
            continue;
        }

        // ȷ���ַ�����null��β
        buffer[bytesReceived] = '\0';

        //std::cout << "Received from " << clientInfo << " - " << buffer << std::endl;
        // ��ӡʱ����ͽ�����Ϣ
        std::cout << time_string << " Received " << bytesReceived << " bytes from " << clientInfo << " - " << buffer << std::endl;

        // ������Ӧ
        std::string response = "Server received: ";
        response += buffer;

        // ������Ӧ
        if (sendto(serverSocket, response.c_str(), static_cast<int>(response.length()), 0,
            reinterpret_cast<SOCKADDR*>(&clientAddr), clientAddrLen) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        }
    }
}