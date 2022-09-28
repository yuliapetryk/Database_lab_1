#define _CRT_SECURE_NO_WARNINGS
#ifndef order_h
#define order_h
#include <stdbool.h>

typedef struct {
    char name[100];
    char dishes[150];
    int uni_id;
    bool deleted;
    int nextId;
} Order;

Order* createOrder(char name[100], char dishes[150], int uni_id);
void printOrder(Order* order);

#endif