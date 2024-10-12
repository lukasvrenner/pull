#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT "80"

int tcp_connect(const char *hostname);

int http_request(int sock, const char *hostname);

int main(const int argc, const char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "expected a URL\n");
        exit(EXIT_FAILURE);
    }

    char buf[1024] = { 0 };

    assert(argc >= 2);
    const char *hostname = argv[1];

    int sock = tcp_connect(hostname);

    if (http_request(sock, hostname) == -1) {
        fprintf(stderr, "hostname %s is too long\n", hostname);
    }

    int recieved_len = recv(sock, &buf, sizeof(buf), 0);

    if (recieved_len == -1) {
        perror("could not recieve data");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buf);
    close(sock);
}

int tcp_connect(const char *hostname)
{
    int sock;
    struct addrinfo hints = { 0 }, *res, *ip_addr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, SERVER_PORT, &hints, &res) != 0) {
        fprintf(stderr, "could not find hostname %s\n", hostname);
        exit(EXIT_FAILURE);
    }

    for (ip_addr = res; ip_addr != NULL; ip_addr = ip_addr->ai_next) {
        sock = socket(
            ip_addr->ai_family, ip_addr->ai_socktype, ip_addr->ai_protocol);
        if (sock == -1) {
            perror("could not create a socket");
            continue;
        }

        if (connect(sock, ip_addr->ai_addr, ip_addr->ai_addrlen) == -1) {
            perror("could not create a connection");
            close(sock);
            continue;
        }

        break;
    }
    if (ip_addr == NULL) {
        fprintf(stderr,
                "could not find an IP address with hostname %s\n",
                hostname);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);
    return sock;
}

int http_request(const int sock, const char *hostname)
{
    char buf[512] = { 0 };
    const char *end = buf + sizeof(buf);
    char *str = buf;

    char *start_of_request = "GET / HTTP/1.1\r\nHost: ";
    for (; str < end && *start_of_request; str++, start_of_request++) {
        *str = *start_of_request;
    }

    for (; str < end && *hostname; str++, hostname++) {
        *str = *hostname;
    }

    char *end_of_request = "\r\nConnection: close\r\n\r\n";
    for (; str < end && *end_of_request; str++, end_of_request++) {
        *str = *end_of_request;
    }
    if (str > end)
        return -1;

    if (send(sock, buf, str - buf, 0) == -1) {
        perror("send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    return 0;
}
