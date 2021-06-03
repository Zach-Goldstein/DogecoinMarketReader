#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "address.h"
#include "chain_reader.h"
#include "request.h"
#include "market.h"
// #include "market_reader.h"

double last_price = 0;
double init_price;

void update(void) {
    double cur_price = get_price();
    if (last_price == 0) {
        last_price = cur_price;
        init_price = cur_price;
    }
    else if (last_price == cur_price)
        return;
    
    double percent_change = (cur_price - init_price) / init_price;

    printf("$%f (%f)\n", cur_price, percent_change);
    last_price = cur_price;
}

int main(int argc, char *argv[]) {
    if (argc <= 1)
        exit(-1);
    init_network();

    Address *a = str_to_address(argv[1]);
    if (!a) {
        fprintf(stderr, "Something went wrong!\n");
        exit(-1);
    }
    
    double market_price = get_price();
    double address_value = market_price * get_address_balance(a);
    printf("[Market] $%f\n", market_price);
    printf("Current wallet value: %f\n", address_value);

    address_destory(a);
    
    destroy_network();
}