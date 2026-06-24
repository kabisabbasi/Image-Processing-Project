#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "NetworkTransfer.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <fstream>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool InitializeWinsock()
{
    WSADATA data;

    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void CleanupWinsock()
{
    WSACleanup();
}

bool SendFile(const char* filename, int port)
{
    if (!InitializeWinsock())
        return false;

    ifstream file(filename, ios::binary);

    if (!file.is_open())
    {
        CleanupWinsock();
        return false;
    }

    file.seekg(0, ios::end);

    long long fileSize = file.tellg();

    file.seekg(0, ios::beg);

    SOCKET serverSocket =
        socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
        CleanupWinsock();
        return false;
    }

    sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (
        bind(
            serverSocket,
            (sockaddr*)&serverAddr,
            sizeof(serverAddr)
        ) == SOCKET_ERROR
    )
    {
        closesocket(serverSocket);
        CleanupWinsock();
        return false;
    }

    listen(serverSocket, 1);

    cout << "Waiting for receiver...\n";

    SOCKET clientSocket =
        accept(serverSocket, nullptr, nullptr);

    if (clientSocket == INVALID_SOCKET)
    {
        closesocket(serverSocket);
        CleanupWinsock();
        return false;
    }

    send(
        clientSocket,
        (char*)&fileSize,
        sizeof(fileSize),
        0
    );

    const int BUFFER_SIZE = 4096;

    char buffer[BUFFER_SIZE];

    while (!file.eof())
    {
        file.read(buffer, BUFFER_SIZE);

        int bytesRead = file.gcount();

        if (bytesRead > 0)
        {
            send(
                clientSocket,
                buffer,
                bytesRead,
                0
            );
        }
    }

    cout << "File sent successfully.\n";

    file.close();

    closesocket(clientSocket);
    closesocket(serverSocket);

    CleanupWinsock();

    return true;
}

bool ReceiveFile(const char* outputFile, const char* ip, int port)
{
    if (!InitializeWinsock())
        return false;

    SOCKET clientSocket =
        socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET)
    {
        CleanupWinsock();
        return false;
    }

    sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    inet_pton(
        AF_INET,
        ip,
        &serverAddr.sin_addr
    );

    cout << "Connecting to sender...\n";

    if (
        connect(
            clientSocket,
            (sockaddr*)&serverAddr,
            sizeof(serverAddr)
        ) == SOCKET_ERROR
    )
    {
        closesocket(clientSocket);
        CleanupWinsock();
        return false;
    }

    long long fileSize = 0;

    recv(
        clientSocket,
        (char*)&fileSize,
        sizeof(fileSize),
        0
    );

    ofstream file(outputFile, ios::binary);

    if (!file.is_open())
    {
        closesocket(clientSocket);
        CleanupWinsock();
        return false;
    }

    const int BUFFER_SIZE = 4096;

    char buffer[BUFFER_SIZE];

    long long received = 0;

    while (received < fileSize)
    {
        int bytesReceived =
            recv(
                clientSocket,
                buffer,
                BUFFER_SIZE,
                0
            );

        if (bytesReceived <= 0)
            break;

        file.write(buffer, bytesReceived);

        received += bytesReceived;
    }

    cout << "File received successfully.\n";

    file.close();

    closesocket(clientSocket);

    CleanupWinsock();

    return true;
}
