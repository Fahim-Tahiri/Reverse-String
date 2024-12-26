#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>


#define PORT 8080
#define BUFFER_SIZE 1024

void *client_thread(void *socket_desc);
void reverse(char *str);

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error initializing Winsock\n");
        return 1;
    }

    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size = sizeof(struct sockaddr_in);

    // socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // address
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("setsockopt failed with error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }


    // address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // listening for connections
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is waiting for connections...\n");

    while (1) {
        // client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket == INVALID_SOCKET) {
            printf("Client connection failed\n");
            continue;
        }

        printf("New client connected\n");

        pthread_t thread;
        if (pthread_create(&thread, NULL, client_thread, (void *)&client_socket) != 0) {
            printf("Error creating thread\n");
        }
        pthread_detach(thread);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

void *client_thread(void *socket_desc) {
    SOCKET sock = *(SOCKET *)socket_desc;
    char buffer[BUFFER_SIZE];
    int read_size;

    // messages from client
    while ((read_size = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[read_size] = '\0'; // remove null
        printf("Received: %s\n", buffer);

        // reverse function
        reverse(buffer);

        // send result
        if (send(sock, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Send failed\n");
        } else {
            printf("Sent: %s\n", buffer);
        }
    }

    printf("Client disconnected\n");
    closesocket(sock);
    return NULL;
}

void reverse(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}
