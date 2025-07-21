#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define BUFFER_SIZE 512

int main() {
    // 3. 初始化Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed!\n");
        return 1;
    }

    // 4. 创建监听套接字
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Socket creation failed!\n");
        WSACleanup();
        return 1;
    }

    // 5. 填充地址结构体
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 6. 绑定
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed!\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 7. 监听
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed!\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Server is listening on port %d...\n", PORT);

    // 9. 等待客户端连接
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed!\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Client connected.\n");

    // 11. 接收消息
    char recvBuf[BUFFER_SIZE+1];
    int bytesReceived = recv(clientSocket, recvBuf, BUFFER_SIZE, 0);
    if (bytesReceived > 0) {
        recvBuf[bytesReceived] = '\0'; // 手动添加字符串结束符
        printf("Received: %s\n", recvBuf);
        char sendBuf[1024];
        sprintf(sendBuf, "%s reply from server", recvBuf); // 构造回复字符串
        send(clientSocket, sendBuf, (int)strlen(sendBuf), 0);
    }

    // 19. 关闭socket
    closesocket(clientSocket);
    closesocket(listenSocket);
    // 20. 清理
    WSACleanup();
    return 0;
}
