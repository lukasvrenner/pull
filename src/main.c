#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <turtls.h>

int tcp_connect(const char *hostname);
ssize_t tcp_send(const void *data, size_t n, const void *ctx);
ssize_t tcp_read(void *buf, size_t n, const void *ctx);
void tcp_close(const void *ctx);

int http_request(int sock, const char *hostname);

int main(const int argc, const char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "expected a URL\n");
        exit(EXIT_FAILURE);
    }
    const char *hostname = argv[1];

    int sock = tcp_connect(hostname);

    fcntl(sock, F_SETFL, O_NONBLOCK);

    struct turtls_Io io = {
        .write_fn = tcp_send,
        .read_fn = tcp_read,
        .close_fn = tcp_close,
        .ctx = &sock,
    };

    shake_hands_client(io);
    if (http_request(sock, hostname) == -1) {
        fprintf(stderr, "hostname %s is too long\n", hostname);
    }

    char buf[1024] = { 0 };
    // save room for a null byte at the end
    int recieved_len = recv(sock, &buf, sizeof(buf) - 1, 0);
    close(sock);

    if (recieved_len == -1) {
        perror("could not recieve data");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buf);
}

ssize_t tcp_send(const void *data, size_t n, const void *ctx)
{
    return send(*(int *)ctx, data, n, 0);
}

ssize_t tcp_read(void *buf, size_t n, const void *ctx)
{
    ssize_t bytes_read = recv(*(int *)ctx, buf, n, 0);
    if (bytes_read < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
        return 0;
    }
    return bytes_read;
}

void tcp_close(const void *ctx) {
    close(*(int *)ctx);
}


int tcp_connect(const char *hostname)
{
    int sock;
    struct addrinfo hints = { 0 }, *result, *p;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, "https", &hints, &result) != 0) {
        fprintf(stderr, "could not find hostname %s\n", hostname);
        exit(EXIT_FAILURE);
    }

    for (p = result; p != NULL; p = p->ai_next) {
        sock = socket(
            p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) {
            perror("could not create a socket");
            continue;
        }

        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            perror("could not create a connection");
            close(sock);
            continue;
        }

        break;
    }
    if (p == NULL) {
        fprintf(stderr,
                "could not find an IP address with hostname %s\n",
                hostname);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);
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
