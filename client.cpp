#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void receiver(SOCKET sock) {
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    while (true) {
        int len = recv(sock, buf, BUF_SIZE - 1, 0);
        if (len > 0) {
            buf[len] = '\0';
            cout << "\n" << string(buf, len) << "\n> " << flush;
        }
        else if (len == 0) {
            cout << "\nServer closed connection.\n";
            break;
        }
        else {
            cerr << "\nrecv failed: " << WSAGetLastError() << "\n";
            break;
        }
    }
}

int main() {
    const char* DEFAULT_PORT = "54000";
    const char* DEFAULT_ADDR = "127.0.0.1";

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cerr << "WSAStartup failed: " << iResult << "\n";
        return 1;
    }

    addrinfo hints = {}, * res = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(DEFAULT_ADDR, DEFAULT_PORT, &hints, &res);
    if (iResult != 0) {
        cerr << "getaddrinfo failed: " << iResult << "\n";
        WSACleanup();
        return 1;
    }

    SOCKET connectSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connectSock == INVALID_SOCKET) {
        cerr << "socket failed: " << WSAGetLastError() << "\n";
        freeaddrinfo(res);
        WSACleanup();
        return 1;
    }

    iResult = connect(connectSock, res->ai_addr, (int)res->ai_addrlen);
    freeaddrinfo(res);
    if (iResult == SOCKET_ERROR) {
        cerr << "connect failed: " << WSAGetLastError() << "\n";
        closesocket(connectSock);
        WSACleanup();
        return 1;
    }

    // Отримати нікнейм від користувача
    string nickname;
    cout << "Enter your nickname: ";
    getline(cin, nickname);

    // Надіслати нікнейм на сервер
    send(connectSock, nickname.c_str(), (int)nickname.size(), 0);

    cout << "Connected to server " << DEFAULT_ADDR << ":" << DEFAULT_PORT << " as " << nickname << "\n";
    cout << "Type 'quit' or 'exit' to leave the chat\n\n";

    thread recv_thread(receiver, connectSock);

    string line;
    while (true) {
        cout << "> " << flush;
        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        int sent = send(connectSock, line.c_str(), (int)line.size(), 0);
        if (sent == SOCKET_ERROR) {
            cerr << "send failed: " << WSAGetLastError() << "\n";
            break;
        }
        if (line == "quit" || line == "exit") break;
    }

    shutdown(connectSock, SD_SEND);
    recv_thread.join();
    closesocket(connectSock);
    WSACleanup();
    return 0;
}