#define _CRT_SECURE_NO_WARNINGS
#ifndef file_worker_h
#define file_worker_h
#define KEYS_TYPE "Keys"
#define INTEGERS_TYPE "Integers"

#include <string.h>
#include <stdbool.h>

#include "array.h"

FILE* openFile(char fileName[]);
long getFileSize(FILE* file, long itemSize);
int findEmptyPosition(Array* emptyPositions);
void formFileName(const char tableName[], char fileName[], int fileType);
void insert(void* item, const char tableName[], int id, Array* tableKeys, Array* emptyPositions);
void replaceByPosition(void* newItem, const char tableName[], int position);
void replace(void* newItem, const char tableName[], int id, Array* tableKeys);
void removeRecordingByPosition(const char tableName[], int position, Array* tableKeys, Array* emptyPositions);
void removeRecording(const char tableName[], int id, Array* tableKeys, Array* emptyPositions);
void* getByPosition(const char tableName[], int position);
void* getItem(const char tableName[], int id, Array* tableKeys);
Array* getItems(const char tableName[]);
void resetTable(const char tableName[], Array** tableKeys, Array** emptyPositions);
void clearTable(const char tableName[]);

Array* getKeysArray(const char tableName[]);
Array* getDeletedPositionsArray(const char tableName[]);
void saveArray(const char tableName[], int arrayType, Array* array);

#endif