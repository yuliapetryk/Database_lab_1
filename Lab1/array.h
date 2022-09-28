#define _CRT_SECURE_NO_WARNINGS
#ifndef array_h
#define array_h

#define TYPENAME_LEN 100

typedef struct {
    void** arr;
    char typeName[TYPENAME_LEN];
    int size;
    int actualSize;
    long itemSize;
} Array;

Array* createArray(const char typeName[TYPENAME_LEN], int size, long itemSize);
void arrayInsert(Array* array, void* item);
void arrayRemove(Array* array, int position);
void deleteArray(Array* array);
void sort(Array* array);
int binarySearch(Array* array, void* id);
void printArray(Array* array);

#endif
