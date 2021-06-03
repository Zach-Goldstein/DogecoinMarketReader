#define ADDRESS_LENGTH 34

struct address_t {
    char *address;
};

typedef struct address_t Address;

Address *address_init(void);
void address_destory(Address *a);
Address *str_to_address(const char *str_addr);