#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <turtls.h>

static int tcp_connect(const char *hostname, const char *port);
static ssize_t tcp_send(const void *data, size_t n, const void *ctx);
static ssize_t tcp_read(void *buf, size_t n, const void *ctx);
static void tcp_close(const void *ctx);

int main(const int argc, const char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "expected a URL\n");
        exit(EXIT_FAILURE);
    }
    const char *hostname = argv[1];
    const char *port;

    if (argc >= 3) {
        port = argv[2];
    } else {
        port = "https";
    }

    int sock = tcp_connect(hostname, port);

    int flags = fcntl(sock, F_GETFL, 0);

    if (flags < 0) {
        perror("could not get flags");
        exit(EXIT_FAILURE);
    }

    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("could not disable blocking");
        exit(EXIT_FAILURE);
    }

    struct turtls_Io io = {
        .write_fn = tcp_send,
        .read_fn = tcp_read,
        .close_fn = tcp_close,
        .ctx = &sock,
    };

    struct turtls_Config config = turtls_generate_config();
    config.extensions.server_name = hostname;
    const char *app_protos[] = { "h2" };
    config.extensions.app_protos = app_protos;
    config.extensions.app_proto_count = 1;

    struct turtls_Connection *connection = turtls_alloc();

    struct turtls_ShakeResult result = turtls_connect(io, connection, &config);

    switch (result.tag) {
    case TURTLS_SHAKE_RESULT_OK:
        puts("handshake succeeded");
        break;
    case TURTLS_SHAKE_RESULT_TIMEOUT:
        fputs("record timeout\n", stderr);
        exit(EXIT_FAILURE);
        break;
    case TURTLS_SHAKE_RESULT_IO_ERROR:
        perror("io error");
        exit(EXIT_FAILURE);
        break;
    case TURTLS_SHAKE_RESULT_RNG_ERROR:
        fputs("could not generate a secure random number\n", stderr);
        exit(EXIT_FAILURE);
        break;
    case TURTLS_SHAKE_RESULT_CONFIG_ERROR:
        /* TODO: display the specific error once provided */
        fputs("there was a config error\n", stderr);
        exit(EXIT_FAILURE);
        break;
    case TURTLS_SHAKE_RESULT_PRIV_KEY_IS_ZERO:
        fputs("the generated private key was zero\n", stderr);
        exit(EXIT_FAILURE);
        break;

    case TURTLS_SHAKE_RESULT_RECEIVED_ALERT:
        /* TODO: stringify the alert */
        fprintf(stderr, "received alert: %s\n", turtls_stringify_alert(result.received_alert));
        exit(EXIT_FAILURE);
        break;
    case TURTLS_SHAKE_RESULT_SENT_ALERT:
        /* TODO: stringify the alert */
        fprintf(stderr, "peer error: %s\n", turtls_stringify_alert(result.received_alert));
        exit(EXIT_FAILURE);
        break;
    }

    puts(turtls_app_proto(connection));

    turtls_close(connection);
    turtls_free(connection);
}

static ssize_t tcp_send(const void *data, size_t n, const void *ctx)
{
    ssize_t bytes_read = send(*(int *) ctx, data, n, 0);
    if (bytes_read < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
        bytes_read = 0;
    }
    return bytes_read;
}

static ssize_t tcp_read(void *buf, size_t n, const void *ctx)
{
    ssize_t bytes_read = recv(*(int *) ctx, buf, n, 0);
    if (bytes_read < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
        bytes_read = 0;
    }
    return bytes_read;
}

static void tcp_close(const void *ctx) { close(*(int *) ctx); }

static int tcp_connect(const char *hostname, const char *port)
{
    int sock;
    struct addrinfo hints = { 0 }, *result, *p;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, port, &hints, &result) != 0) {
        fprintf(stderr, "could not find hostname %s\n", hostname);
        exit(EXIT_FAILURE);
    }

    for (p = result; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
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
        fprintf(stderr, "could not find an IP address with hostname %s\n", hostname);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);
    return sock;
}
