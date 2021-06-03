#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "address.h"

Address *address_init(void) {
    Address *a;
    char *s;

    a = (Address *)malloc(sizeof(Address));
    if (!a)
        return NULL;
    
    s = (char *)malloc(ADDRESS_LENGTH + 1);
    if (!s) {
        free(a);
        return NULL;
    }

    memset(a, 0, sizeof(Address));
    memset(s, 0, ADDRESS_LENGTH + 1);

    a->address = s;

    return a;
}

void address_destory(Address *a) {
    if (!a)
        return;
    
    free(a->address);
    free(a);
}

Address *str_to_address(const char *str_addr) {
    Address *a = NULL;

    // Verify string length
    if (strlen(str_addr) != ADDRESS_LENGTH) {
        fprintf(stderr, "[ADDR] Invalid string length\n");
        goto err;
    }

    // Verify string contents
    int i;
    for (i = 0; i < ADDRESS_LENGTH; i += 1) {
        int check_char = ((str_addr[i] >= 'a' && str_addr[i] <= 'z') || 
            (str_addr[i] >= 'A' && str_addr[i] <= 'Z') ||
            (str_addr[i] >= '0' && str_addr[i] <= '9'));
        
        if (!check_char) {
            fprintf(stderr, "[ADDR] Invalid string characters\n");
        }
    }

    // Create new string buffer and copy in
    a = address_init();
    if (!a) {
        fprintf(stderr, "[ADDR] Could not create address\n");
        goto err;
    }

    memcpy(a->address, str_addr, ADDRESS_LENGTH);
    
    return a;

err:
    return NULL;
}