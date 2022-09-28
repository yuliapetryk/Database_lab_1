#define _CRT_SECURE_NO_WARNINGS
#ifndef keys_h
#define keys_h

typedef struct {
    int id;
    int position;
} Keys;

Keys* createKeys(int id, int position);
void printKeys(Keys* keys);

#endif