// UDPServer.h
#ifndef UDPSERVER_H
#define UDPSERVER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class UDPServer {
private:
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    int clientAddrLen;

public:
    explicit UDPServer(int port);
    void start();
    ~UDPServer();

    // ½ûÖ¹¿½±´
    UDPServer(const UDPServer&) = delete;
    UDPServer& operator=(const UDPServer&) = delete;
};

#endif // UDPSERVER_H