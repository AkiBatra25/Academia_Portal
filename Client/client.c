#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[2048];  // Bigger buffer to capture multi-line server responses

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("📡 Connected to Academia Portal server on port %d\n", PORT);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, sizeof(buffer) - 1);
        if (valread <= 0) {
            printf("Client closed the connection.\n");
            break;
        }

        buffer[valread] = '\0';
        printf("%s", buffer);

        // ✅ Detect session end clearly
        if (strstr(buffer, "Enter your choice:") || 
    strstr(buffer, "Enter username:") ||
    strstr(buffer, "Enter password:") ||
    strstr(buffer, "Enter new username:") ||
    strstr(buffer, "Enter new password:") ||
    strstr(buffer, "Enter student ID") || 
    strstr(buffer, "Enter user ID to modify:") ||
    buffer[valread - 1] == ':') {
        
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    write(sock, buffer, strlen(buffer));
}

    }

    close(sock);
    return 0;
}
