#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "order.h"

Order* createOrder(char name[100], char dishes[150], int uni_id) {
    Order* order = malloc(sizeof(Order));
    strcpy(order->name, name);
    strcpy(order->dishes,dishes);
    order->uni_id = uni_id;
    order->deleted = false;
    order->nextId = INT_MAX;
    return order;
}

void printOrder(Order* order) {
    if (order == NULL) {
        printf("No data :(\n");
        return;
    }
    printf("Name: %s; Dishes: %s; Customer ID: %d\n", order->name, order->dishes, order->uni_id);
}