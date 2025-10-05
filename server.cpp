#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <map>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

map<SOCKET, string> clients;
mutex clients_mutex;

void broadcast_message(const string& msg, SOCKET sender) {
    lock_guard<mutex> lock(clients_mutex);
    for (auto& client : clients) {
        if (client.first != sender) {
            send(client.first, msg.c_str(), (int)msg.size(), 0);
        }
    }
}

void handle_client(SOCKET clientSock) {
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];

    // Отримати нікнейм клієнта
    string nickname;
    int len = recv(clientSock, buf, BUF_SIZE - 1, 0);
    if (len > 0) {
        buf[len] = '\0';
        nickname = string(buf, len);

        // Додати клієнта до списку
        {
            lock_guard<mutex> lock(clients_mutex);
            clients[clientSock] = nickname;
        }

        string join_msg = nickname + " joined the chat!";
        cout << join_msg << endl;
        broadcast_message(join_msg, clientSock);
    }
    else {
        closesocket(clientSock);
        return;
    }

    while (true) {
        int len = recv(clientSock, buf, BUF_SIZE - 1, 0);
        if (len > 0) {
            buf[len] = '\0';
            string msg = string(buf, len);

            if (msg == "quit" || msg == "exit") {
                string leave_msg = nickname + " left the chat!";
                cout << leave_msg << endl;
                broadcast_message(leave_msg, clientSock);
                break;
            }

            string formatted = nickname + ": " + msg;
            cout << formatted << endl;
            broadcast_message(formatted, clientSock);
        }
        else {
            string leave_msg = nickname + " disconnected!";
            cout << leave_msg << endl;
            broadcast_message(leave_msg, clientSock);
            break;
        }
    }

    // Видалити клієнта зі списку
    {
        lock_guard<mutex> lock(clients_mutex);
        clients.erase(clientSock);
    }
    closesocket(clientSock);
}

int main() {
    const char* DEFAULT_PORT = "54000";

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
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &res);
    if (iResult != 0) {
        cerr << "getaddrinfo failed: " << iResult << "\n";
        WSACleanup();
        return 1;
    }

    SOCKET listenSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listenSock == INVALID_SOCKET) {
        cerr << "socket failed: " << WSAGetLastError() << "\n";
        freeaddrinfo(res);
        WSACleanup();
        return 1;
    }

    iResult = bind(listenSock, res->ai_addr, (int)res->ai_addrlen);
    freeaddrinfo(res);
    if (iResult == SOCKET_ERROR) {
        cerr << "bind failed: " << WSAGetLastError() << "\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "listen failed: " << WSAGetLastError() << "\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    cout << "Server started on port " << DEFAULT_PORT << ". Waiting for clients...\n";

    while (true) {
        SOCKET clientSock = accept(listenSock, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET) {
            cerr << "accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        cout << "New client connected. Total clients: " << clients.size() + 1 << endl;
        thread(handle_client, clientSock).detach();
    }

    closesocket(listenSock);
    WSACleanup();
    return 0;
}