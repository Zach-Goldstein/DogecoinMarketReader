#include <stdio.h>
#include <tidy.h>
#include <tidybuffio.h>
#include <curl/curl.h>

#define URL_MARKET_API "https://coinmarketcap.com/currencies/dogecoin/"
#define HTML_PRICE "priceValue" 

size_t write_result_callback(void *in, size_t sz, size_t nmemb, TidyBuffer *out) {
    size_t total_size;
    total_size = sz * nmemb;

    tidyBufAppend(out, in, total_size);

    return total_size;
}

void dump_node(TidyDoc doc, TidyNode tnod, int indent) {
    TidyNode child;
    for (child = tidyGetChild(tnod); child; child = tidyGetNext(child)) {
        ctmbstr name = tidyNodeGetName(child);
        if (name) {
            TidyAttr attr;
            printf("%*.*s%s ", indent, indent, "<", name);
            for (attr = tidyAttrFirst(child); attr; attr = tidyAttrNext(attr)) {
                printf("%s", tidyAttrName(attr));
                tidyAttrValue(attr) ? printf("=\"%s\" ", tidyAttrValue(attr)) : 
                                        printf(" ");
            }
            printf(">\n");
        }
        else {
            TidyBuffer buf;
            tidyBufInit(&buf);
            tidyNodeGetText(doc, child, &buf);
            printf("%*.*s\n", indent, indent, buf.bp?(char *)buf.bp:"");
            tidyBufFree(&buf);
        }
        dump_node(doc, child, indent + 4);
    }
}

static void retrieve_market_data(const char *url) {
    CURL *curl;
    CURLcode ret;

    TidyDoc tdoc;
    TidyBuffer docbuf = { 0 };
    TidyBuffer tidy_errbuf = { 0 };
    int tidyerr;

    curl = curl_easy_init();
    if (!curl)
        return;

    printf("[CURL] Searching for %s\n", URL_MARKET_API);

    curl_easy_setopt(curl, CURLOPT_URL, URL_MARKET_API);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_result_callback);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "tinydoge-agent/1.0");

#ifndef SKIP_PEER_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifndef SKIP_HOSTNAME_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

    tdoc = tidyCreate();
    tidyOptSetBool(tdoc, TidyForceOutput, yes);
    tidyOptSetInt(tdoc, TidyWrapLen, 4096);
    tidySetErrorBuffer(tdoc, &tidy_errbuf);
    tidyBufInit(&docbuf);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &docbuf);
    ret = curl_easy_perform(curl);

    if (ret != CURLE_OK) {
        fprintf(stderr, "[CURL] Failed to retrieve data: %s\n", curl_easy_strerror(ret));
        goto err;
    }

    tidyerr = tidyParseBuffer(tdoc, &docbuf);
    if (tidyerr >= 0) {
        tidyerr = tidyCleanAndRepair(tdoc);
        if (tidyerr >= 0) {
            tidyerr = tidyRunDiagnostics(tdoc);
            if (tidyerr >= 0) {
                dump_node(tdoc, tidyGetRoot(tdoc), 0);
                fprintf(stderr, "%s\n", tidy_errbuf.bp);
            }
        }
    }
    
    curl_easy_cleanup(curl);
    tidyBufFree(&docbuf);
    tidyBufFree(&tidy_errbuf);
    tidyRelease(tdoc);
    return;

err:
    curl_easy_cleanup(curl);
    tidyBufFree(&docbuf);
    tidyBufFree(&tidy_errbuf);
    tidyRelease(tdoc);
    return;
}

void run_market_get(void) {
    retrieve_market_data(NULL);
}