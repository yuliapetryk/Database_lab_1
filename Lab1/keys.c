#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "keys.h"

Keys* createKeys(int id, int position) {
    Keys* keys = malloc(sizeof(Keys));
    keys->id = id;
    keys->position = position;
    return keys;
}

void printKeys(Keys* keys) {
    printf("%d %d\n", keys->id, keys->position);
}