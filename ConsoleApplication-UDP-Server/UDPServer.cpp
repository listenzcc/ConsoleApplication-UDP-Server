// UDPServer.cpp
#include "UDPServer.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>  // 必须包含这个头文件

UDPServer::UDPServer(int port) : serverSocket(INVALID_SOCKET), clientAddrLen(sizeof(clientAddr)) {
    // 初始化Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }

    // 创建socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    // 设置服务器地址
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // 绑定socket
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

// 添加辅助函数
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
        // 使用现代的方式获取客户端地址信息
        std::string clientInfo = getClientAddress(clientAddr);

        // 接收数据
        clientAddrLen = sizeof(clientAddr);
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0,
            reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrLen);

        // 获取当前时间戳字符串
		std::string time_string = getLocalTimeString();

        if (bytesReceived == SOCKET_ERROR) {
            int error = WSAGetLastError();

            // 特别处理10054错误
            if (error == 10054) {
                // 这是UDP中的正常现象，客户端可能已经关闭
                // 可以选择记录或完全忽略
                continue;
            }
            else {
                std::cerr << "Receive failed: " << error << std::endl;
            }
            continue;
        }

        // 确保字符串以null结尾
        buffer[bytesReceived] = '\0';

        //std::cout << "Received from " << clientInfo << " - " << buffer << std::endl;
        // 打印时间戳和接收信息
        std::cout << time_string << " Received " << bytesReceived << " bytes from " << clientInfo << " - " << buffer << std::endl;

        // 构建响应
        std::string response = "Server received: ";
        response += buffer;

        // 发送响应
        if (sendto(serverSocket, response.c_str(), static_cast<int>(response.length()), 0,
            reinterpret_cast<SOCKADDR*>(&clientAddr), clientAddrLen) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        }
    }
}