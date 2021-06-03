#include <stdlib.h>

struct curl_result_t {
    char *data;
    size_t size;
};

typedef struct curl_result_t Curl_Result;

void    init_network(void);
void    destroy_network(void);
Curl_Result *retrieve_url(const char *url);