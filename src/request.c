/* 
 * Handles curl requests across the Internet
 * 
 */
#include <curl/curl.h>
#include <malloc.h>
#include <string.h>

#include "request.h"
#include "settings.h"

/*
 * Call before any network requests.
 * Should only be called at program start-up.
 */
void init_network(void) {
    curl_global_init(CURL_GLOBAL_ALL);
}

/*
 * Call when no more network requests are expected. 
 * Should only be called at program termination.
 */
void destroy_network(void) {
    curl_global_cleanup();
}

/*
 * Stores curl result in a buffer.
 */
static size_t write_result_callback(void *data, size_t sz, size_t nmemb, void *userp) {
    size_t total_size = sz * nmemb;
    Curl_Result *cres = (Curl_Result *)userp;

    char *ptr = realloc(cres->data, cres->size + total_size + 1);

    if (!ptr) {
        fprintf(stderr, "[CURL] Not enough memory.\n");
        return 0;
    }

    cres->data = ptr;
    memcpy(&(cres->data[cres->size]), data, total_size);
    cres->size += total_size;
    cres->data[cres->size] = 0;

    return total_size;
}

/*
 * Resolves url and retrieves data at the link.
 * Returns: Pointer to data struct on success or NULL on failure
 */
Curl_Result *retrieve_url(const char *url) {
    Curl_Result *cres;
    CURL *curl;
    CURLcode ret;

    cres = (Curl_Result *)malloc(sizeof(Curl_Result));
    if (!cres)
        return NULL;
    
    memset(cres, 0, sizeof(Curl_Result));

    curl = curl_easy_init();
    if (!curl)
        goto err;

    //printf("[CURL] Searching for %s\n", url);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_result_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) cres);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "tinydoge-agent/1.0");

#ifndef SKIP_PEER_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifndef SKIP_HOSTNAME_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

    ret = curl_easy_perform(curl);

    if (ret != CURLE_OK) {
        fprintf(stderr, "[CURL] Failed to retrieve data: %s\n", curl_easy_strerror(ret));
        goto err;
    } 
    
    curl_easy_cleanup(curl);
    return cres;

err:
    free(cres->data);
    free(cres);
    return NULL;
}