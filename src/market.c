#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "request.h"
#include "settings.h"

// Max length for an exchange name
#define MAX_EXCHANGE_NAME 32

#define URL_MARKET_API "https://chain.so/api/v2/get_price/DOGE/USD"

struct market_data_t {
    double price;
    char price_base[4];
    char exchange[32];
    size_t time;
};

static size_t parse_price(const Curl_Result *to_parse, struct market_data_t *result) {
    char *json = to_parse->data;
    int ret;

    double entry_price = 0;
    char entry_price_base[4] = { 0 };
    char entry_exchange[32] = { 0 };
    size_t entry_time = 0;

    while (strncmp(json, "\"prices\"", 8)) {
        if (*json == '}')
            return 0;
        json++;
    }

    // Scan to price entries
    while (*json != '[') {
        json++;
    }

    for (; *json; json++) {
        if (*json == ']')
            break;
        
        if (*json == '\"') {
            if (!strncmp(json, "\"price\"", 7)) {
                json += 9;
                for (; *(json - 1) != '\"'; json++);
                entry_price = atof(json);
            }
            else if (!strncmp(json, "\"price_base\"", 12)) {
                json += 14;
                for (; *(json - 1) != '\"'; json++);
                for (int i = 0; i < 3 && json[i] != '\"'; i++) 
                    entry_price_base[i] = json[i];
            }
            else if (!strncmp(json, "\"exchange\"", 10)) {
                json += 12;
                for (; *(json - 1) != '\"'; json++);
                for (int i = 0; i < MAX_EXCHANGE_NAME - 1 && json[i] != '\"'; i++) 
                    entry_exchange[i] = json[i];
            }
            else if (!strncmp(json, "\"time\"", 6)) {
                json += 8;
                for (; !(*json >= '0' && *json <= '9'); json++);
                entry_time = (size_t)atoll(json);
            }
            
            // Skip to end
            for (; *(json - 1) != '\"' && *(json) == '}'; json++);
        }

        if (*json == '}') {
            result->price = entry_price;
            strncpy(result->exchange, entry_exchange, MAX_EXCHANGE_NAME);
            memset(entry_exchange, 0, MAX_EXCHANGE_NAME);
            strncpy(result->price_base, entry_price_base, 3);
            memset(entry_price_base, 0, 4);
            result->time = entry_time;
            ret = 1;

#if defined(PREFERRED_BINANCE)
            if (!strcmp(result->exchange, "binance"))
                return ret;
#elif defined(PREFERRED_GEMINI)
            if (!strcmp(result->exchange, "gemini"))
                return ret;
#else
            return ret;
#endif
        }
    }
    return 0;
}

double get_price() {
    Curl_Result *request;
    
    request = retrieve_url(URL_MARKET_API);
    if (!request) {
        fprintf(stderr, "[Market] Could not retrieve market price\n");
        return -1;
    }

    // Check for prices section
    int i;
    for (i = 0; i < request->size; i++) {
        if (!strncmp(request->data + i, "success", 7))
            break;
    }

    if (i == request->size) {
        fprintf(stderr, "[Market] Connected, but no success status code");
        free(request);
        return -1;
    }

    struct market_data_t prices = { 0 };
    if (!parse_price(request, &prices)) {
        prices.price = -1;
        fprintf(stderr, "[Market] Could not parse data\n");
    }
    
    free(request->data);
    free(request);

    return prices.price;
}