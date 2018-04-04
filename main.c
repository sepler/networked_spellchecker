#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "spellchecker.h"
#include "queue.h"

#define NUM_WORKERS 8

const char* DEFUALT_DIRECTORY = "words.txt";
const int DEFAULT_PORT = 3207;

pthread_t tid[NUM_WORKERS+1];
pthread_mutex_t lock_log, lock_socket;
pthread_cond_t cond_log, cond_socket;
dictionary dict;
queue* queue_log;
queue* queue_socket;

char* num_to_str(int);
void* worker_func();
void* logger_func();

int main(int argc, char *argv[]) {
    // arg1 = dict file, arg2 = port
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

    queue_log = create_queue();
    queue_socket = create_queue();

    if (pthread_mutex_init(&lock_log, NULL) != 0) {
        printf("log mutex init failed\n");
        exit (1);
    }
    if (pthread_mutex_init(&lock_socket, NULL) != 0) {
        printf("socket mutex init failed\n");
        exit (1);
    }

    int i;
    for (i = 0; i < NUM_WORKERS; i++) {
        int err = pthread_create(&(tid[i]), NULL, &worker_func, NULL);
        if (err != 0) {
            printf("cant create thread: %s", strerror(err));
        }
    }

    int err = pthread_create(&(tid[NUM_WORKERS]), NULL, &logger_func, NULL);
    if (err != 0) {
        printf("cant create logger thread thread: %s", strerror(err));
        exit(1);
    }    

    listen(socket_desc, 3);

    printf("accepting connections\n");
    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c))) {
        pthread_mutex_lock(&lock_socket); // Lock socket mutex
        push_queue(queue_socket, num_to_str(new_socket));
        pthread_mutex_unlock(&lock_socket); // Unlock socket mutex
        pthread_cond_signal(&cond_socket); // Signal blocked threads to wakeup
    }
    
}

char* num_to_str(int val) {
    char* result = malloc(256 * sizeof(result));
    sprintf(result, "%d", val);
    return result;
}

void* worker_func() {
    while (1) {
        pthread_mutex_lock(&lock_socket); // Lock socket mutex
        pthread_cond_wait(&cond_socket, &lock_socket); // Unlock socket mutex, wait for condition, then relock
        if (queue_socket->size < 1) { // Is there still something in queue?
            pthread_mutex_unlock(&lock_socket); // Empty, release mutex
            continue;
        }
        char* end;
        int new_socket = strtol(pop_queue(queue_socket), &end, 10);
        pthread_mutex_unlock(&lock_socket); // Unlock socket mutex
        int len;
        char buffer[256];
        while ((len = recv(new_socket, buffer, 256, 0)) > 0) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            int result = spellcheck(dict, buffer);
            char message[256];
            strcpy(message, num_to_str(new_socket));
            strcat(message, " - ");
            strcat(message, buffer);
            if (result == 1) {
                strcat(message, " OK\n");
            } else {
                strcat(message, " MISSPELLED\n");
            }
            write(new_socket, message, strlen(message));
            pthread_mutex_lock(&lock_log); // Lock log mutex
            push_queue(queue_log, message);
            pthread_mutex_unlock(&lock_log); // Unlock log mutex
            pthread_cond_signal(&cond_log); // Signal blocked threads to wakeup
        }
    }
}

void* logger_func() {
    while (1) {
        pthread_mutex_lock(&lock_log); // Lock log mutex
        pthread_cond_wait(&cond_log, &lock_log); // Unlock log mutex, wait for condition, then relock
        while (queue_log->size > 0) {
            FILE* fp;
            fp = fopen("server.log", "a");
            char* message = pop_queue(queue_log);
            printf("%s", message);
            fputs(message, fp);
            fclose(fp);
        }
        pthread_mutex_unlock(&lock_log); // Unlock socket mutex
    }
}