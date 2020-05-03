#include "rpc_server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <signal.h>

#define PORT 1312

#define FLOAT_TO_STRING_LEN 20

#define AVG     1
#define MINMAX  2
#define PROD    3
#define EXIT    4

void handle_sigint(int sig);
void serve(int sd, char *host);
void server_prog_1(char *host, int sd, int choice, int lenY, int *y, float a);

// global so the signal handler can close the socket
int lsd;

int main (int argc, char *argv[]) {
    char *host;
    struct sockaddr_in sin;
    int sin_size = sizeof(sin);
    int sd;
    int pid;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s server_host\n", argv[0]);
        exit (1);
    }

    host = argv[1];

    signal(SIGINT, handle_sigint);

    // Create listening socket
    if ((lsd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create listening socket: ");
        exit(1);
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(lsd, (struct sockaddr *) &sin, sin_size) < 0) {
        perror("Cannot bind listening socket: ");
        exit(2);
    }

    // Initiate a listen queue
    if (listen(lsd, 5) < 0) {
        perror("Cannot listen on socket: ");
        exit(3);
    }

    while (1) {
        if ((sd = accept(lsd, (struct sockaddr *) &sin, &sin_size)) < 0) {
            perror("Cannot accept a new connect request from socket: ");
            exit(4);
        }

        if ((pid = fork()) < 0) {
            perror("Cannot create a new child to serve request: ");
            exit(5);
        }

        if (pid == 0) { // child process
            serve(sd, host);
            close(sd);
            exit(0);
        }
    }

    exit(0);
}

void handle_sigint(int sig) {
    close(lsd);
    exit(0);
}

void serve(int sd, char *host) {
    short choice;
    int lenY;
    int *y;
    float a = 0.0f;
    char a_str[FLOAT_TO_STRING_LEN];
    int i;

    while (1) {
        recv(sd, &choice, sizeof(choice), 0);
        choice = ntohs(choice);
        if (choice == EXIT) {
            return;
        }
        recv(sd, &lenY, sizeof(lenY), 0);
        lenY = ntohl(lenY);
        y = malloc(lenY * sizeof(int));
        recv(sd, y, lenY * sizeof(y[0]), 0);

        for (i = 0; i < lenY; i++) {
            y[i] = ntohl(y[i]);
        }

        if (choice == PROD) {
            recv(sd, &a_str, sizeof(a_str), 0);
            a = atof(a_str);
        }

        server_prog_1(host, sd, choice, lenY, y, a);
        free(y);
    }
}

void server_prog_1(char *host, int sd, int choice, int lenY, int *y, float a) {
    CLIENT *clnt;
    args arguments;
    float *avg;
    char avg_str[FLOAT_TO_STRING_LEN];
    min_max *min_max_struct;
    int min_max_array[2];
    float_array *product;
    char *product_strs;
    int i;

#ifndef DEBUG
    clnt = clnt_create (host, SERVER_PROG, SERVER_VERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror (host);
        exit (1);
    }
#endif  /* DEBUG */

    arguments.array.array_len = lenY;
    arguments.array.array_val = y;
    arguments.a = a;

    switch (choice) {
        case AVG:
            avg = average_1(&arguments, clnt);
            if (avg == (float *) NULL) {
                clnt_perror (clnt, "call failed");
            }
            else {
                sprintf(avg_str, "%f", *avg);
                send(sd, avg_str, sizeof(avg_str), 0);
            }
            break;
        case MINMAX:
            min_max_struct = min_max_1(&arguments, clnt);
            if (min_max_struct == (min_max *) NULL) {
                clnt_perror (clnt, "call failed");
            }
            else {
                min_max_array[0] = htonl(min_max_struct->min);
                min_max_array[1] = htonl(min_max_struct->max);

                send(sd, min_max_array, sizeof(min_max_array), 0);
            }
            break;
        case PROD:
            product = product_1(&arguments, clnt);
            if (product == (float_array *) NULL) {
                clnt_perror (clnt, "call failed");
            }
            else {
                product_strs = malloc(product->array.array_len * FLOAT_TO_STRING_LEN);
                for (i = 0; i < product->array.array_len; i++) {
                    // conver the ith element to string
                    // sprintf adds the null terminating character
                    sprintf(product_strs + i * FLOAT_TO_STRING_LEN, "%f", product->array.array_val[i]);
                }
                send(sd, product_strs, FLOAT_TO_STRING_LEN * product->array.array_len, 0);

                free(product->array.array_val);
                free(product_strs);
            }
            break;
        default:
            break;
    }
    
    
#ifndef DEBUG
    clnt_destroy (clnt);
#endif  /* DEBUG */
}
