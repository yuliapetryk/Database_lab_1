#define _CRT_SECURE_NO_WARNINGS
#ifndef customer_h
#define customer_h
#include <stdbool.h>

typedef struct {
    char name[100];
    bool deleted;
    int slaveId;
}Customer;

Customer* createCustomer(char name[100]);
void printCustomer(Customer* customer);

#endif