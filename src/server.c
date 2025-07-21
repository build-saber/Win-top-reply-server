#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE + 1]; // 多1字节用于\0
    int recvLen;

    // 1. 初始化Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed!\n");
        return 1;
    }

    // 2. 创建socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed!\n");
        WSACleanup();
        return 1;
    }

    // 3. 填充地址结构体
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // 4. 绑定
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed!\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 5. 监听
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        printf("Listen failed!\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    printf("服务器已启动，等待客户端连接...\n");

    // 6. 接受连接（一次服务一主）
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed!\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    printf("客户端已连接。\n");

    // 7. 接收和回复消息
    while (1) {
        recvLen = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (recvLen <= 0) {
            printf("客户端已断开连接。\n");
            break;
        }
        // 手动添加\0结尾
        if (recvLen < BUFFER_SIZE) buffer[recvLen] = '\0';
        else buffer[BUFFER_SIZE] = '\0';
        printf("收到消息: %s\n", buffer);

        // 回复消息
        char reply[BUFFER_SIZE + 32];
        snprintf(reply, sizeof(reply), "服务器已收到: %s", buffer);
        send(clientSocket, reply, (int)strlen(reply), 0);
    }

    // 8. 关闭socket和清理
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
