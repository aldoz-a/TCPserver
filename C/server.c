#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MYBUFSIZE 1024
#define BACKLOG   10      // per listen()

int main(int argc, char *argv[])
{
    // test argomenti
    if (argc != 2) {
        // errore di chiamata
        printf("%s: numero argomenti errato\n", argv[0]);
        printf("uso: %s port [e.g.: %s 9999]\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    // creo il socket in modo internet/TCP
    int srv_sock;
    if ((srv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        // errore socket()
        printf("%s: non posso creare il socket (%s)\n", argv[0], strerror(errno));
        return EXIT_FAILURE;
    }

    // preparo la struttura sockaddr_in per questo server
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;            // set famiglia di indirizzi
    server.sin_addr.s_addr = INADDR_ANY;    // set indirizzo del server
    server.sin_port = htons(atoi(argv[1])); // set port del server

    // associo l'indirizzo del server al socket
    if (bind(srv_sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        // errore bind()
        printf("%s: errore bind (%s)", argv[0], strerror(errno));
        close(srv_sock);
        return EXIT_FAILURE;
    }

    // start ascolto con una coda di max BACKLOG connessioni
    if (listen(srv_sock, BACKLOG) == -1) {
        // errore listen()
        printf("%s: errore listen (%s)\n", argv[0], strerror(errno));
        close(srv_sock);
        return EXIT_FAILURE;
    }

    // accetto connessioni da un client entrante
    printf("%s: attesa connessioni entranti...\n", argv[0]);
    socklen_t socksize = sizeof(struct sockaddr_in);
    struct sockaddr_in client;      // struttura sockaddr_in per il client remoto
    int cli_sock;
    if ((cli_sock = accept(srv_sock, (struct sockaddr *)&client, &socksize)) == -1) {
        // errore accept()
        printf("%s: errore accept (%s)\n", argv[0], strerror(errno));
        close(srv_sock);
        return EXIT_FAILURE;
    }

    // chiudo il socket non più in uso
    close(srv_sock);

    // loop di ricezione messaggi dal client
    char cli_msg[MYBUFSIZE];
    int recv_size;
    while ((recv_size = recv(cli_sock, cli_msg, MYBUFSIZE, 0)) > 0 ) {
        // send messaggio di ritorno al client
        printf("%s: ricevuto messaggio dal sock %d: %s\n", argv[0], cli_sock, cli_msg);
        char srv_msg[MYBUFSIZE];
        sprintf(srv_msg, "mi hai scritto: %s", cli_msg);
        if (send(cli_sock, srv_msg, strlen(srv_msg), 0) == -1) {
            // errore send()
            printf("%s: errore send (%s)\n", argv[0], strerror(errno));
            close(cli_sock);
            return EXIT_FAILURE;
        }

        // clear del buffer
        memset(cli_msg, 0, MYBUFSIZE);
    }

    // loop terminato: test motivo
    if (recv_size == -1) {
        // errore recv()
        printf("%s: errore recv (%s)\n", argv[0], strerror(errno));
        close(cli_sock);
        return EXIT_FAILURE;
    }
    else if (recv_size == 0) {
        // Ok: il client si è disconnesso
        printf("%s: client disconnesso\n", argv[0]);
    }

    // esco con Ok
    close(cli_sock);
    return EXIT_SUCCESS;
}
