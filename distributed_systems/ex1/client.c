#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

// #define h_addr h_addr_list[0]
#define SERVER_PORT 1312

#define FLOAT_TO_STRING_LEN 20

#define AVG     1
#define MINMAX  2
#define PROD    3
#define EXIT    4

int connectToServer(const char *host);
short menu();

int main(int argc, char const *argv[]) {
    int sd;

    short choice, net_choice;
    int lenY, net_lenY;
    int *y;
    float a;
    char a_str[FLOAT_TO_STRING_LEN];
    int i;
    int tmp;
    
    float avg;
    char avg_str[FLOAT_TO_STRING_LEN];
    int min_max_array[2];
    float *product;
    char *product_strs;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s hostname\n", argv[0]);
        exit(1);
    }

    sd = connectToServer(argv[1]);
    
    choice = menu();
    while (choice != EXIT) {
        printf("Length of Y: ");
        scanf("%d", &lenY);

        if (lenY <= 0) {
            continue;
        }

        y = malloc(lenY * sizeof(int));

        for (i = 0; i < lenY; i++) {
            printf("Y[%d] = ", i);
            scanf("%d", &tmp);
            y[i] = htonl(tmp);
        }

        if (choice == PROD) {
            printf("a = ");
            scanf("%f", &a);
        }
        net_choice = htons(choice);
        send(sd, &net_choice, sizeof(net_choice), 0);
        net_lenY = htonl(lenY);
        send(sd, &net_lenY, sizeof(net_lenY), 0);
        send(sd, y, lenY * sizeof(y[0]), 0);


        if (choice == AVG) {
            recv(sd, avg_str, sizeof(avg_str), 0);
            avg = atof(avg_str);

            printf("-----------\n");
            printf("Average = %.02f\n", avg);
            printf("-----------\n");
        }
        else if (choice == MINMAX) {
            recv(sd, min_max_array, sizeof(min_max_array), 0);
            min_max_array[0] = ntohl(min_max_array[0]);
            min_max_array[1] = ntohl(min_max_array[1]);

            printf("-----------\n");
            printf("Min = %d\n", min_max_array[0]);
            printf("Max = %d\n", min_max_array[1]);
            printf("-----------\n");
        }
        else if (choice == PROD) {
            // convert float to string
            // sprintf adds the null terminating character
            sprintf(a_str, "%f", a);

            send(sd, &a_str, sizeof(a_str), 0);

            product = malloc(lenY * sizeof(float));
            product_strs = malloc(lenY * FLOAT_TO_STRING_LEN);
            recv(sd, product_strs, lenY * FLOAT_TO_STRING_LEN, 0);

            // convert the string with all numbers to individual floats
            for (i = 0; i < lenY; i++) {
                product[i] = atof(product_strs + i * FLOAT_TO_STRING_LEN);
            }

            printf("-----------\n");
            printf("a*Y = ");
            for (i = 0; i < lenY; i++) {
                printf("%.02f ", product[i]);
            }
            printf("\n-----------\n");

            free(product);
            free(product_strs);
        }

        free(y);
        choice = menu();
    }

    choice = htons(choice);
    send(sd, &choice, sizeof(choice), 0);   // send exit
    close(sd);

    return 0;
}

int connectToServer(const char *host) {
    int sd;
    struct sockaddr_in server;
    struct hostent *server_host;

    // create socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket: ");
        exit(2);
    }

    // Get info on host
    if ((server_host = gethostbyname(host)) == NULL) {
        perror("Unknown host: ");
        exit(3);
    }

    // Set up struct sockaddr_in
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    bcopy((char *)server_host->h_addr, (char *)&server.sin_addr, server_host->h_length);

    // Connect
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Cannot connect to server: ");
        exit(4);
    }

    printf("Connected to the server\n");

    return sd;
}

short menu() {
    int ans;
    printf("1. Average\n");
    printf("2. Min, Max\n");
    printf("3. a*Y\n");
    printf("4. Exit\n");
    do {
        printf("> ");
        scanf("%d", &ans);
    } while (ans <= 0 || ans >= 5);
    return ans;
}
