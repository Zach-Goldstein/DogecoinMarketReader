#include <curl/curl.h>
#include <malloc.h>
#include <string.h>

#include "address.h"
#include "chain_reader.h"
#include "request.h"

#define URL_CHAIN_API   "https://dogechain.info/chain/Dogecoin/q/"
#define URL_BALANCE     "addressbalance/"
#define URL_CHECK       "checkaddress/"
#define URL_BLOCKCOUNT  "getblockcount/"
#define URL_DIF         "getdifficulty"
#define URL_RECV        "getreceivedbyaddress/"
#define URL_SENT        "getsentbyaddress/"
#define URL_TOTALMINED  "totalbc"

#define OPT_BALANCE     1
#define OPT_CHECK       2
#define OPT_BLOCKCOUNT  3
#define OPT_DIF         4
#define OPT_RECV        5
#define OPT_SENT        6
#define OPT_TOTALMINED  7

#define REQ_TRUE        "1E"

static const char *build_url(int opt, Address *addr) {
    int needs_addr;
    const char *url_opt;
    switch (opt) {
        case OPT_BALANCE:
            url_opt = URL_BALANCE;
            needs_addr = 1;
            break;
        case OPT_CHECK:
            url_opt = URL_CHECK;
            needs_addr = 1;
            break;
        case OPT_BLOCKCOUNT:
            url_opt = URL_BLOCKCOUNT;
            needs_addr = 0;
            break;
        case OPT_DIF:
            url_opt = URL_DIF;
            needs_addr = 0;
            break;
        case OPT_RECV:
            url_opt = URL_RECV;
            needs_addr = 1;
            break;
        case OPT_SENT:
            url_opt = URL_SENT;
            needs_addr = 1;
            break;
        case OPT_TOTALMINED:
            url_opt = URL_TOTALMINED;
            needs_addr = 0;
            break;
        default:
            return NULL;
    }

    int len;
    if (needs_addr && !addr) {
        fprintf(stderr, "[URL] Operation requires wallet address\n");
        return NULL;
    }

    if (needs_addr)
        len = strlen(URL_CHAIN_API) + strlen(url_opt) + strlen(addr->address);
    else
        len = strlen(URL_CHAIN_API) + strlen(url_opt);
    
    char *url = (char *)malloc((len + 1) * sizeof(char));
    if (!url)
        return NULL;
    
    memset(url, 0, len + 1);

    strcpy(url, URL_CHAIN_API);
    strcat(url, url_opt);
    if (needs_addr)
        strcat(url, addr->address);

    return (const char *)url;
}

double get_address_balance(Address *addr) {
    const Curl_Result *request;
    const char *url;
    double bal;

    url = build_url(OPT_BALANCE, addr);
    request = retrieve_url(url);
    if (!request) {
        fprintf(stderr, "[Balance] Could not retrieve address balance\n");
        bal = -1;
    }
    else if (!(request->data[0] >= '0' && request->data[0] <= '9')) {
        fprintf(stderr, "[Balance] Invalid request data: %s\n", request->data);
        bal = -1;
    }
    else {
        bal = atof(request->data);
        fprintf(stdout, "[Balance] %f\n", bal);
    }

    free((void *)url);
    free((void *)request->data);
    free((void *)request);
    return bal;
}

int get_address_check(Address *addr) {
    const Curl_Result *request;
    const char *url;
    int ret;

    url = build_url(OPT_CHECK, addr);
    request = retrieve_url(url);
    if (!request) {
        fprintf(stderr, "[Check] Could not retrive address balance\n");
        ret = 0;
    }
    else if ((strncmp(request->data, REQ_TRUE, 2))) {
        fprintf(stderr, "[Check] No address found, %s\n", request->data);
        ret = 0;
    }
    else {
        fprintf(stdout, "[Check] Address OK\n");
        ret = 1;
    }

    free((void *)url);
    free((void *)request->data);
    free((void *)request);
    return ret;
}

long get_blockcount(void) {
    const Curl_Result *request;
    const char *url;
    long ret;

    url = build_url(OPT_BLOCKCOUNT, NULL);
    request = retrieve_url(url);
    if (!request) {
        fprintf(stderr, "[Blockcount] Could not retrive blockcount\n");
        ret = -1;
    }
    else if (!(request->data[0] >= '0' && request->data[0] <= '9')) {
        fprintf(stderr, "[Blockcount] Invalid request data%s\n", request->data);
        ret = -1;
    }
    else {
        ret = atoi(request->data);
        fprintf(stdout, "[Blockcount] %ld\n", ret);
    }

    free((void *)url);
    if (ret >= 0)
        free((void *)request->data);
    free((void *)request);
    return ret;
}

double get_difficulty(void) {
    const Curl_Result *request;
    const char *url;
    double ret;

    url = build_url(OPT_DIF, NULL);
    request = retrieve_url(url);
    if (!request) {
        fprintf(stderr, "[Difficulty] Could not retrive difficulty\n");
        ret = -1;
    }
    else if (!(request->data[0] >= '0' && request->data[0] <= '9')) {
        fprintf(stderr, "[Difficulty] Invalid request data\n");
        ret = -1;
    }
    else {
        ret = atof(request->data);
        fprintf(stdout, "[Difficulty] %f\n", ret);
    }

    free((void *)url);
    free((void *)request->data);
    free((void *)request);
    return ret;
}

double get_address_received(Address *addr) {
    const Curl_Result *request;
    const char *url;
    double bal;

    url = build_url(OPT_RECV, addr);
    request = retrieve_url(url);
    if (!request) {
        fprintf(stderr, "[Received] Could not retrive address balance\n");
        bal = -1;
    }
    else if (!(request->data[0] >= '0' && request->data[0] <= '9')) {
        fprintf(stderr, "[Received] Invalid request data\n");
        bal = -1;
    }
    else {
        bal = atof(request->data);
        fprintf(stdout, "[Received] %f\n", bal);
    }

    free((void *)url);
    free((void *)request->data);
    free((void *)request);
    return bal;
}

double get_address_sent(Address *addr) {
    const Curl_Result *request;
    const char *url;
    double bal;

    url = build_url(OPT_SENT, addr);
    request = retrieve_url(url);
    if (!request) {
        fprintf(stderr, "[Sent] Could not retrive address balance\n");
        bal = -1;
    }
    else if (!(request->data[0] >= '0' && request->data[0] <= '9')) {
        fprintf(stderr, "[Sent] Invalid request data\n");
        bal = -1;
    }
    else {
        bal = atof(request->data);
        fprintf(stdout, "[Sent] %f\n", bal);
    }

    free((void *)url);
    free((void *)request->data);
    free((void *)request);
    return bal;
}

long get_totalmined(void) {
    const Curl_Result *request;
    const char *url;
    long ret;

    url = build_url(OPT_TOTALMINED, NULL);
    request = retrieve_url(url);
    if (!request) {
        fprintf(stderr, "[Total Mined] Could not retrive total mined\n");
        ret = -1;
    }
    else if (!(request->data[0] >= '0' && request->data[0] <= '9')) {
        fprintf(stderr, "[Total Mined] Invalid request data\n");
        ret = -1;
    }
    else {
        ret = atoi(request->data);
        fprintf(stdout, "[Total Mined] %ld\n", ret);
    }

    free((void *)url);
    free((void *)request->data);
    free((void *)request);
    return ret;
}