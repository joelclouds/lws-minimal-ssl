#include <libwebsockets.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

static int interrupted = 0;

static void sigint_handler(int sig) {
    (void)sig;
    interrupted = 1;
}

static int callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len) {
    (void)user; // Suppress warning
    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        fprintf(stderr, "[INFO] Connected\n");
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            const char *msg = "Hello from C";
            unsigned char buf[LWS_PRE + 512];
            unsigned char *p = &buf[LWS_PRE];
            size_t n = strlen(msg);

            memcpy(p, msg, n);
            if (lws_write(wsi, p, n, LWS_WRITE_TEXT) < 0) {
                fprintf(stderr, "[ERROR] lws_write failed\n");
                interrupted = 1;
            } else {
                fprintf(stderr, "[SENT] %s\n", msg);
            }
        }
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        fprintf(stderr, "[RECV] %.*s\n", (int)len, (char *)in);
        interrupted = 1;
        break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        fprintf(stderr, "[ERROR] Connection failed\n");
        interrupted = 1;
        break;

    default:
        break;
    }
    return 0;
}

static const struct lws_protocols protocols[] = {
    { "minimal", callback_minimal, 0, 0, 0, NULL, 0 },
    LWS_PROTOCOL_LIST_TERM
};

int main(void) {
    struct lws_context_creation_info info = {0};
    struct lws_client_connect_info i = {0};
    struct lws_context *context;
    struct lws *wsi;

    signal(SIGINT, sigint_handler);

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "[FATAL] lws_create_context failed\n");
        return 1;
    }

    i.context = context;
    i.address = "echo.websocket.org"; //"demo.piesocket.com";
    i.port = 443;
    i.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
    i.path = "/"; ///v3/channel_1?api_key=VCXCEuvhGcBDP7XhiJJUDvR1e1D3eiVjgZ9VRiaV&channel_id=1";
    i.host = i.address;
    i.origin = i.address;
    i.protocol = protocols[0].name;

    wsi = lws_client_connect_via_info(&i);
    if (!wsi) {
        fprintf(stderr, "[FATAL] lws_client_connect_via_info failed\n");
        lws_context_destroy(context);
        return 1;
    }

    while (!interrupted && lws_service(context, 1000) >= 0) {}

    lws_context_destroy(context);
    return 0;
}
