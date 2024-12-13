#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
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

    struct TurtlsIo io = {
        .write_fn = tcp_send,
        .read_fn = tcp_read,
        .close_fn = tcp_close,
        .ctx = &sock,
    };
    struct TurtlsConn *tls_conn = turtls_new(io);
    struct TurtlsConfig *config = turtls_get_config(tls_conn);

    const char app_protos[] = { 2, 'h', '2' };
    config->extensions.app_protos = app_protos;
    config->extensions.app_protos_len = sizeof(app_protos);

    config->extensions.server_name = hostname;

    enum TurtlsError result = turtls_connect(tls_conn);

    switch (result) {
    case TURTLS_ERROR_NONE:
        puts("handshake succeeded");
        break;
    case TURTLS_ERROR_WANT_READ:
        fputs("read error\n", stderr);
        exit(EXIT_FAILURE);
        break;
    case TURTLS_ERROR_WANT_WRITE:
        fputs("write error\n", stderr);
        exit(EXIT_FAILURE);
        break;
    case TURTLS_ERROR_TLS:
        fprintf(stderr, "tls error: %s\n", turtls_stringify_alert(turtls_get_tls_error(tls_conn)));
        exit(EXIT_FAILURE);
        break;
    case TURTLS_ERROR_TLS_PEER:
        fprintf(
            stderr, "peer tls error: %s\n", turtls_stringify_alert(turtls_get_tls_error(tls_conn)));
        exit(EXIT_FAILURE);
        break;

    case TURTLS_ERROR_RNG:
        fputs("could not generate a secure random number\n", stderr);
        exit(EXIT_FAILURE);
        break;
    case TURTLS_ERROR_PRIV_KEY_IS_ZERO:
        fputs("the generated private key was zero\n", stderr);
        exit(EXIT_FAILURE);
        break;
    case TURTLS_ERROR_MISSING_EXTENSIONS:
        fputs("missing extensions\n", stderr);
        exit(EXIT_FAILURE);
        break;
    }

    puts(turtls_app_proto(tls_conn));

    turtls_close(tls_conn);
    turtls_free(tls_conn);
}

static ssize_t tcp_send(const void *data, size_t n, const void *ctx)
{
    return send(*(int *) ctx, data, n, 0);
}

static ssize_t tcp_read(void *buf, size_t n, const void *ctx)
{
    return recv(*(int *) ctx, buf, n, 0);
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
