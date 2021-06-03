#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define SETTINGS_FILENAME "settings.h"

#define DISABLE_PEER_VERIFICATION "#define SKIP_PEER_VERIFICATION\n"
#define DISABLE_HOSTNAME_VERIFICATION "#define SKIP_HOSTNAME_VERIFICATION\n"
#define PREFER_BINANCE "#define PREFERRED_BINANCE\n"
#define PREFER_GEMINI "#define PREFERRED_GEMINI\n"

#define PEER_VERIFICATION       1
#define HOSTNAME_VERIFICATION   2
#define BINANCE                 4
#define GEMINI                  8

unsigned long settings = PEER_VERIFICATION |
                            HOSTNAME_VERIFICATION;

void write_settings() {
    int settings_fd;

    settings_fd = open(SETTINGS_FILENAME, O_RDWR | O_CREAT | O_TRUNC);
    if (settings_fd < 0) {
        fprintf(stderr, "[CONFIGURE] Fatal error, could not create settings file\n");
        exit(-1);
    }

    fprintf(stdout, "Peer verification... ");
    if (settings & PEER_VERIFICATION) {
        fprintf(stdout, "yes\n");
    } 
    else {
        fprintf(stdout, "no\n");
        dprintf(settings_fd, DISABLE_PEER_VERIFICATION);
    }

    fprintf(stdout, "Hostname verification... ");
    if (settings & HOSTNAME_VERIFICATION) {
        fprintf(stdout, "yes\n");
    } 
    else {
        fprintf(stdout, "no\n");
        dprintf(settings_fd, DISABLE_HOSTNAME_VERIFICATION);
    }

    fprintf(stdout, "Preferred binance... ");
    if (settings & BINANCE) {
        fprintf(stdout, "yes\n");
        dprintf(settings_fd, PREFER_BINANCE);
    } 
    else {
        fprintf(stdout, "no\n");
    }

    fprintf(stdout, "Preferred gemini... ");
    if (settings & GEMINI) {
        fprintf(stdout, "yes\n");
        dprintf(settings_fd, PREFER_GEMINI);
    } 
    else {
        fprintf(stdout, "no\n");
    }
}

int main(int argc, char *argv[]) {
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-disable-peer-verification")) {
            fprintf(stdout, "[CONFIGURE] Warning: disabling peer verification may be unsafe\n");
            settings ^= (0 ^ settings) & PEER_VERIFICATION;
        }
        else if (!strcmp(argv[i], "-disable-hostname-verification")) {
            fprintf(stdout, "[CONFIGURE] Warning: disabling hostname verification may be unsafe\n");
            settings ^= (0 ^ settings) & HOSTNAME_VERIFICATION;
        }
        else if (!strcmp(argv[i], "-prefer-binance")) {
            settings ^= (-1 ^ settings) & BINANCE;
        }
        else if (!strcmp(argv[i], "-prefer-gemini")) {
            settings ^= (-1 ^ settings) & GEMINI;
        }
        else {
            fprintf(stderr, "[CONFIGURE] Unknown command: %s\n", argv[i]);
        }
    }

    write_settings();
    exit(0);
}