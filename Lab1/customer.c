#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "customer.h"

Customer* createCustomer(char name[100]) {
    Customer* customer = malloc(sizeof(Customer));
    strcpy(customer->name, name);
    customer->deleted = false;
    customer->slaveId = INT_MAX;
    return customer;
}

void printCustomer(Customer* customer) {
    if (customer == NULL) {
        printf("No data:(\n");
        return;
    }
    printf("Name: %s\n", customer->name);
}