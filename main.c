#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "spellchecker.h"

const char* DEFUALT_DIRECTORY = "words.txt";
const int DEFAULT_PORT = 3207;

int main(int argc, char *argv[]) {
    // arg1 = dict file, arg2 = port
    dictionary dict;
    if (argc >= 2) {
        dict = create_dict(argv[1]);
    } else {
        dict = create_dict(DEFUALT_DIRECTORY);
    }
    if (dict.size == 0) {
        printf("Could not build dictionary!\n");
        return 1;
    }

    int socket_desc, new_socket, optval, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket!\n");
        return 1;
    }
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)); // Dont hang onto socket

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    if (argc == 3) {
        char *end;
        server.sin_port = htons((int)strtol(argv[2], &end, 10));
    } else {
        server.sin_port = htons(DEFAULT_PORT);
    }

    if (bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Bind failed!\n");
        return 1;
    }

    listen(socket_desc, 3);

    printf("accepting connections\n");
    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c))) {
        printf("connection accepted\n");
        char* message = "hello!\n";
        write(new_socket, message, strlen(message));
        int len;
        char buffer[256];
        while ((len = recv(new_socket, buffer, 256, 0)) > 0) {
            printf("read from client: %s\n", buffer);
        }
    }

}