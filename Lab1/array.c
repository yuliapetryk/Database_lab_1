#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "keys.h"
#include "customer.h"
#include "order.h"

const char KEYS_TYPE[] = "Keys";
const char INTEGERS_TYPE[] = "Integers";
const char CUSTOMER_TYPE[] = "customer";
const char ORDER_TYPE[] = "order";

Array* createArray(const char typeName[TYPENAME_LEN], int size, long itemSize) {
    Array* array = malloc(sizeof(Array));
    array->itemSize = itemSize;
    array->size = (size < 8) ? 16 : 2 * size;
    array->actualSize = 0;
    array->arr = malloc(array->size * itemSize);
    strcpy(array->typeName, typeName);
    return array;
}

void arrayInsert(Array* array, void* item) {
    if (array->actualSize < array->size) {
        array->arr[array->actualSize++] = item;
        return;
    }
    void** newArr = malloc(2 * array->size * array->itemSize);
    for (int i = 0; i < array->size; ++i) {
        newArr[i] = array->arr[i];
    }
    free(array->arr);
    array->arr = newArr;
    arrayInsert(array, item);
}

void arrayRemove(Array* array, int position) {
    if (position >= array->actualSize || array->actualSize == 0) {
        return;
    }
    free(array->arr[position]);
    for (int i = position + 1; i < array->actualSize; ++i) {
        array->arr[i - 1] = array->arr[i];
    }
    --array->actualSize;
}

void deleteArray(Array* array) {
    for (int i = 0; i < array->actualSize; ++i) {
        free(array->arr[i]);
    }
    free(array->arr);
    free(array);
}

void quickSortKeys(Keys** arr, int start, int end) {
    int medium = arr[(start + end) / 2]->id;
    int ptr1 = 0, ptr2 = end;
    while (ptr1 <= ptr2) {
        while (arr[ptr1]->id < medium) {
            ++ptr1;
        }
        while (arr[ptr2]->id > medium) {
            --ptr2;
        }
        if (ptr1 <= ptr2) {
            Keys* temp = arr[ptr1];
            arr[ptr1++] = arr[ptr2];
            arr[ptr2--] = temp;
        }
    }
    if (ptr2 - start > 0) {
        quickSortKeys(arr, start, ptr2);
    }
    if (end - ptr1 > 0) {
        quickSortKeys(arr, ptr1, end);
    }
}

void sort(Array* array) { //Only for Keys**!
    if (array->actualSize == 0) {
        return;
    }
    if (strcmp(array->typeName, KEYS_TYPE) == 0) {
        quickSortKeys((Keys**)array->arr, 0, array->actualSize - 1);
    }
}

int recursiveBinarySearch(Keys** keys, int* id, int start, int end) {
    int medium = (start + end) / 2;
    if (keys[medium]->id > *id && (end - start) > 0) {
        return recursiveBinarySearch(keys, id, start, medium - 1);
    }
    if (keys[medium]->id < *id && (end - start) > 0) {
        return recursiveBinarySearch(keys, id, medium + 1, end);
    }
    if (keys[medium]->id == *id) {
        return medium;
    }
    return -1;
}

int binarySearch(Array* array, void* key) { //Only for Keys**!
    if (array->actualSize == 0) {
        return -1;
    }
    if (strcmp(array->typeName, KEYS_TYPE) == 0) {
        return recursiveBinarySearch((Keys**)array->arr, (int*)key, 0, array->actualSize - 1);
    }
    return -1;
}

void printArray(Array* array) {
    if (array == NULL) {
        return;
    }
    printf("{\n");
    for (int i = 0; i < array->actualSize; ++i) {
        if (strcmp(array->typeName, INTEGERS_TYPE) == 0) {
            int* num = array->arr[i];
            printf("    ");
            printf("%d\n", *num);
        }
        else if (strcmp(array->typeName, KEYS_TYPE) == 0) {
            Keys* keys = array->arr[i];
            printf("    ");
            printKeys(keys);
        }
        else if (strcmp(array->typeName, CUSTOMER_TYPE) == 0) {
            Customer* customer = array->arr[i];
            printf("    ");
            printCustomer(customer);
        }
        else if (strcmp(array->typeName, ORDER_TYPE) == 0) {
            Order* order = array->arr[i];
            printf("    ");
            printOrder(order);
        }
    }
    printf("};\n");
}