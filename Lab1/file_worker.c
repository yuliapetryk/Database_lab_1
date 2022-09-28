#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "file.worker.h"

#include <stdlib.h>

#include "customer.h"
#include "order.h"
#include "keys.h"

#define FILENAME_INITIAL_SIZE 150

const char customerTable[] = "customer";
const char orderTable[] = "order";

FILE* openFile(char fileName[]) {
    FILE* file;
    if ((file = fopen(fileName, "r+b")) == NULL) {
        file = fopen(fileName, "w+b");
    }
    return file;
}

long getFileSize(FILE* file, long itemSize) {
    fseek(file, 0, SEEK_END);
    return ftell(file) / itemSize;
}

void formFileName(const char tableName[], char fileName[], int fileType) {
    if (fileType > 2 || fileType < 0) {
        return;
    }
    strcpy(fileName, "");
    char prefix[] = "/Users/User/Desktop/ПІ-23/ОБД/Лабораторна робота 1/Лабораторна1/";
    strcat(fileName, prefix);
    strcat(fileName, tableName);
    if (fileType == 1) {
        strcat(fileName, "Keys");
    }
    else if (fileType == 2) {
        strcat(fileName, "Trash");
    }
    strcat(fileName, ".dat");
}

int findEmptyPosition(Array* emptyPositions) {
    int position = -1;
    int** arr = (int**)emptyPositions->arr;
    for (int i = 0; i < emptyPositions->actualSize; ++i) {
        if (*arr[i] != -1) {
            position = *arr[i];
            arrayRemove(emptyPositions, i);
            return position;
        }
    }
    return position;
}

int getItemSize(const char tableName[]) {
    if (strcmp(tableName, customerTable) == 0) {
        return sizeof(Customer);
    }
    else if (strcmp(tableName, orderTable) == 0) {
        return sizeof(Order);
    }
    else {
        return 0;
    }
}

void insert(void* item, const char tableName[], int id, Array* tableKeys, Array* emptyPositions) {

    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, 0);
    FILE* file = openFile(fileName);

    long itemSize = getItemSize(tableName);
    int emptyPosition = findEmptyPosition(emptyPositions);
    emptyPosition = (emptyPosition == -1) ? (int)getFileSize(file, itemSize) : emptyPosition;
    fseek(file, emptyPosition * itemSize, SEEK_SET);
    Keys* keys = createKeys(id, emptyPosition);
    arrayInsert(tableKeys, keys);
    fwrite(item, itemSize, 1, file);
    fclose(file);
    sort(tableKeys);
}

bool deleted(void* item, const char tableName[]) {
    if (strcmp(tableName, customerTable) == 0) {
        return ((Customer*)item)->deleted;
    }
    else if (strcmp(tableName, orderTable) == 0) {
        return ((Order*)item)->deleted;
    }
    return true;
}

void setDeleted(void* item, const char tableName[], bool deleted) {
    if (strcmp(tableName, customerTable) == 0) {
        ((Customer*)item)->deleted = deleted;
    }
    else if (strcmp(tableName, orderTable) == 0) {
        ((Order*)item)->deleted = deleted;
    }
}

Array* getItems(const char tableName[]) {
    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, false);
    FILE* file = openFile(fileName);
    long itemSize = getItemSize(tableName);
    int size = (int)getFileSize(file, itemSize);
    Array* itemsArray = createArray(tableName, size, itemSize);
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < size; ++i) {
        void* item = malloc(itemSize);
        fread(item, itemSize, 1, file);
        if (!deleted(item, tableName)) {
            arrayInsert(itemsArray, item);
        }
        else {
            free(item);
        }
    }
    fclose(file);
    return itemsArray;
}

void resetTable(const char tableName[], Array** tableKeys, Array** emptyPositions) {
    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, 0);
    char tempFileName[FILENAME_INITIAL_SIZE] = "";
    formFileName("temp", tempFileName, 0);
    FILE* file = openFile(fileName);
    FILE* tempFile = openFile(tempFileName);
    long itemSize = getItemSize(tableName);
    void* item = malloc(itemSize);
    Array* newKeys = createArray(KEYS_TYPE, (*tableKeys)->actualSize, sizeof(Keys));
    for (int i = 0; i < (*tableKeys)->actualSize; ++i) {
        Keys* keys = (*tableKeys)->arr[i];
        fseek(file, keys->position * itemSize, SEEK_SET);
        fread(item, itemSize, 1, file);
        fwrite(item, itemSize, 1, tempFile);
        arrayInsert(newKeys, createKeys(keys->id, i));
    }
    free(item);
    fclose(file);
    fclose(tempFile);
    remove(fileName);
    rename(tempFileName, fileName);
    deleteArray(*tableKeys);
    deleteArray(*emptyPositions);
    *tableKeys = newKeys;
    *emptyPositions = createArray(INTEGERS_TYPE, 0, sizeof(int));
}

Array* getKeysArray(const char tableName[]) {
    long itemSize = sizeof(Keys);
    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, 1);
    FILE* file = openFile(fileName);
    int fileSize = (int)getFileSize(file, itemSize);
    Array* array = createArray(KEYS_TYPE, fileSize, itemSize);
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < fileSize; ++i) {
        Keys* keys = malloc(itemSize);
        fread(keys, itemSize, 1, file);
        arrayInsert(array, keys);
    }
    return array;
}

Array* getDeletedPositionsArray(const char tableName[]) {
    long itemSize = sizeof(int);
    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, 2);
    FILE* file = openFile(fileName);
    int fileSize = (int)getFileSize(file, itemSize);
    Array* array = createArray(INTEGERS_TYPE, fileSize, itemSize);
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < fileSize; ++i) {
        int* pos = malloc(itemSize);
        fread(pos, itemSize, 1, file);
        arrayInsert(array, pos);
    }
    return array;
}

void saveArray(const char tableName[], int arrayType, Array* array) {
    if (arrayType < 1 || arrayType > 2) { //1 for keys, 2 for trash
        return;
    }
    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, arrayType);
    FILE* file = fopen(fileName, "w+b");
    for (int i = 0; i < array->actualSize; ++i) {
        fwrite(array->arr[i], array->itemSize, 1, file);
    }
    fclose(file);
}

void replaceByPosition(void* newItem, const char tableName[], int position) {
    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, 0);
    FILE* file = openFile(fileName);
    long itemSize = getItemSize(tableName);
    fseek(file, position * itemSize, SEEK_SET);
    fwrite(newItem, itemSize, 1, file);
    fclose(file);
}

void replace(void* newItem, const char tableName[], int id, Array* tableKeys) {
    int index = binarySearch(tableKeys, &id);
    if (index == -1) {
        return;
    }
    Keys* keys = tableKeys->arr[index];
    int position = keys->position;
    replaceByPosition(newItem, tableName, position);
}

void* getByPosition(const char tableName[], int position) {
    char fileName[FILENAME_INITIAL_SIZE] = "";
    formFileName(tableName, fileName, 0);
    FILE* file = openFile(fileName);
    long itemSize = getItemSize(tableName);
    fseek(file, position * itemSize, SEEK_SET);
    void* item = malloc(itemSize);
    fread(item, itemSize, 1, file);
    fclose(file);
    return item;
}

void* getItem(const char tableName[], int id, Array* tableKeys) {
    int index = binarySearch(tableKeys, &id);
    if (index == -1) {
        return NULL;
    }
    Keys* keys = tableKeys->arr[index];
    return getByPosition(tableName, keys->position);
}

void removeRecording(const char tableName[], int id, Array* tableKeys, Array* emptyPositions) {
    int index = binarySearch(tableKeys, &id);
    if (index == -1) {
        return;
    }
    Keys* keys = tableKeys->arr[index];
    int position = keys->position;
    void* item = getByPosition(tableName, position);
    setDeleted(item, tableName, true);
    replaceByPosition(item, tableName, position);
    int* positionPtr = malloc(sizeof(int));
    *positionPtr = position;
    arrayInsert(emptyPositions, positionPtr);
    index = binarySearch(tableKeys, &id);
    arrayRemove(tableKeys, index);
}

void clearTable(const char tableName[]) {
    char fileName[FILENAME_INITIAL_SIZE] = "";
    for (int i = 0; i < 3; ++i) {
        formFileName(tableName, fileName, i);
        remove(fileName);
    }

}