#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "file.worker.h"
#include "customer.h"
#include "order.h"
#include <stdlib.h>
#include <limits.h>
#include "keys.h"

#define INPUT_MAX_LEN 150

const char customerTableName[] = "customer";
const char orderTableName[] = "order";

int* inputId(void) {
    int* id = malloc(sizeof(int));
    printf("%s: ", "ID");
    scanf("%d", id);
    fflush(stdin);
    return id;
}

Customer* inputCustomer(void) {
    char name[INPUT_MAX_LEN];
    printf("%s: ", "Name");
    gets(name);
    Customer* customer = createCustomer(name);
    return customer;
}

Order* inputOrder(void) {
    char name[INPUT_MAX_LEN];
    char dishes[INPUT_MAX_LEN];
    int uni_id;
    printf("%s: ", "Waiter");
    gets(name);
    printf("%s: ", "Dishes:");
    gets(dishes);
    printf("%s: ", "Customer id");
    scanf("%d", &uni_id);
    Order* order = createOrder(name, dishes, uni_id);
    fflush(stdin);
    return order;
}

void insert_m(Customer* item, int id, Array* masterKeys, Array* masterEmptyPositions) {
    insert(item, customerTableName, id, masterKeys, masterEmptyPositions);
}

void insert_s(Order* item, int id, Array* masterKeys, Array* slaveKeys, Array* slaveEmptyPositions) {
    if (binarySearch(slaveKeys, &id) != -1 || binarySearch(masterKeys, &(item->uni_id)) == -1) {
        return;
    }
    insert(item, orderTableName, id, slaveKeys, slaveEmptyPositions);
    Customer* customer = (Customer*)getItem(customerTableName, item->uni_id, masterKeys);
    int index = binarySearch(slaveKeys, &id);
    Keys* keys = slaveKeys->arr[index];
    int position = keys->position;
    if (customer->slaveId == INT_MAX) {
        customer->slaveId = id;
        replace(customer, customerTableName, item->uni_id, masterKeys);
        return;
    }
    int currentId = customer->slaveId;
    free(customer);
    Order* order = (Order*)getItem(orderTableName, currentId, slaveKeys);
    index = binarySearch(slaveKeys, &(order->nextId));
    Keys* nextKeys = slaveKeys->arr[index];
    while (index != -1 && nextKeys->position < position) {
        currentId = nextKeys->id;
        Order* temp = order;
        order = (Order*)getItem(orderTableName, currentId, slaveKeys);
        free(temp);
        index = binarySearch(slaveKeys, &(order->nextId));
        nextKeys = slaveKeys->arr[index];
    }
    item->nextId = order->nextId;
    order->nextId = id;
    replace(order, orderTableName, currentId, slaveKeys);
    replace(item, orderTableName, id, slaveKeys);
    free(order);
}

int count_m(Array* masterKeys) {
    return masterKeys->actualSize;
}

int count_s(Array* slaveKeys) {
    return slaveKeys->actualSize;
}

int count_m_s(int id, Array* masterKeys, Array* slaveKeys) {
    Customer* customer = getItem(customerTableName, id, masterKeys);
    if (customer == NULL || customer->slaveId == INT_MAX) {
        free(customer);
        return 0;
    }
    Order* order = getItem(orderTableName, customer->slaveId, slaveKeys);
    free(customer);
    int counter = 1;
    while (order->nextId != INT_MAX) {
        Order* temp = order;
        order= getItem(orderTableName, order->nextId, slaveKeys);
        free(temp);
        ++counter;
    }
    return counter;
}

Customer* get_m(int id, Array* masterKeys) {
    Customer* customer = getItem(customerTableName, id, masterKeys);
    return customer;
}

Order* get_s(int id, Array* slaveKeys) {
    Order* order = getItem(orderTableName, id, slaveKeys);
    return order;
}

Array* get_m_s(int id, Array* masterKeys, Array* slaveKeys) {
    Customer* customer = getItem(customerTableName, id, masterKeys);
    if (customer == NULL) {
        return NULL;
    }
    int count = count_m_s(id, masterKeys, slaveKeys);
    if (count == 0) {
        return NULL;
    }
    Array* orders = createArray(orderTableName, count, sizeof(Order));
    Order* order = getItem(orderTableName, customer->slaveId, slaveKeys);
    free(customer);
    arrayInsert(orders, order);
    for (int i = 1; i < count; ++i) {
        order = getItem(orderTableName, order->nextId, slaveKeys);
        arrayInsert(orders, order);
    }
    return orders;
}

void del_m(int id, Array* masterKeys, Array* masterEmptyPositions, Array* slaveKeys, Array* slaveEmptyPositions) {
    Customer* customer = getItem(customerTableName, id, masterKeys);
    if (customer == NULL) {
        return;
    }
    removeRecording(customerTableName, id, masterKeys, masterEmptyPositions);
    if (customer->slaveId == INT_MAX) {
        return;
    }
    int currId = customer->slaveId;
    free(customer);
    Order* order = getItem(orderTableName, currId, slaveKeys);
    do {
        removeRecording(orderTableName, currId, slaveKeys, slaveEmptyPositions);
        Order* temp = order;
        currId = order->nextId;
        order = getItem(orderTableName, currId, slaveKeys);
        free(temp);
    } while (order != NULL);
    if (masterEmptyPositions->actualSize > 20) {
        resetTable(customerTableName, &masterKeys, &masterEmptyPositions);
    }
}

void del_s(int id, Array* masterKeys, Array* slaveKeys, Array* slaveEmptyPositions) {
    Order* order = getItem(orderTableName, id, slaveKeys);
    if (order== NULL) {
        return;
    }
    Customer* customer = getItem(customerTableName, order->uni_id, masterKeys);
    if (customer->slaveId == id) {
        customer->slaveId = order->nextId;
        replace(customer, customerTableName, order->uni_id, masterKeys);
        free(customer);
        removeRecording(orderTableName, id, slaveKeys, slaveEmptyPositions);
        return;
    }
    int itemNextId = order->nextId;
    free(order);
    int currId = customer->slaveId;
    free(customer);
    order = getItem(orderTableName, currId, slaveKeys);
    while (order->nextId != INT_MAX && order->nextId != id) {
        Order* temp = order;
        currId = order->nextId;
        order = getItem(orderTableName, currId, slaveKeys);
        free(temp);
    }
    if (order->nextId == INT_MAX) {
        return;
    }
    order->nextId = itemNextId;
    replace(order, orderTableName, currId, slaveKeys);
    free(order);
    removeRecording(orderTableName, id, slaveKeys, slaveEmptyPositions);
    if (slaveEmptyPositions->actualSize > 20) {
        resetTable(orderTableName, &slaveKeys, &slaveEmptyPositions);
    }
}

void update_m(Customer* newItem, int id, Array* masterKeys) {
    Customer* item = getItem(customerTableName, id, masterKeys);
    if (item == NULL) {
        return;
    }
    newItem->slaveId = item->slaveId;
    free(item);
    replace(newItem, customerTableName, id, masterKeys);
}

void update_s(Order* newItem, int id, Array* masterKeys, Array* slaveKeys, Array* slaveEmptyPositions) {
    Order* item = getItem(orderTableName, id, slaveKeys);
    if (item == NULL) {
        return;
    }
    if (item->uni_id == newItem->uni_id) {
        newItem->nextId = item->nextId;
        free(item);
        replace(newItem, orderTableName, id, slaveKeys);
        return;
    }
    free(item);
    del_s(id, masterKeys, slaveKeys, slaveEmptyPositions);
    insert_s(newItem, id, masterKeys, slaveKeys, slaveEmptyPositions);
}

void clearAll(Array** masterKeys, Array** masterEmptyPositions, Array** slaveKeys, Array** slaveEmptyPositions) {
    clearTable(customerTableName);
    clearTable(orderTableName);
    deleteArray(*masterKeys);
    deleteArray(*masterEmptyPositions);
    deleteArray(*slaveKeys);
    deleteArray(*slaveEmptyPositions);
    *masterKeys = createArray(KEYS_TYPE, 0, sizeof(Keys));
    *masterEmptyPositions = createArray(INTEGERS_TYPE, 0, sizeof(int));
    *slaveKeys = createArray(KEYS_TYPE, 0, sizeof(Keys));
    *slaveEmptyPositions = createArray(INTEGERS_TYPE, 0, sizeof(int));
}


int main(int argc, const char* argv[]) {
    Array* masterKeys = getKeysArray(customerTableName);
    Array* masterEmptyPositions = getDeletedPositionsArray(customerTableName);
    Array* slaveKeys = getKeysArray(orderTableName);
    Array* slaveEmptyPositions = getDeletedPositionsArray(orderTableName);
    char input[INPUT_MAX_LEN];
    while (true) {
       
        printf("%s: ", "Enter command");
        gets(input);
        while (strcmp(input, "") == 0)
        {
            gets(input);
        }
        if (strcmp(input, "insert-m") == 0) {
            Customer* customer = inputCustomer();
            int* id = inputId();
            insert_m(customer, *id, masterKeys, masterEmptyPositions);
            free(customer);
            free(id);
            printf("Inserted successfully\n");

        }
      
        else if (strcmp(input, "insert-s") == 0) {
            Order* order = inputOrder();
            int* id = inputId();
            insert_s(order, *id, masterKeys, slaveKeys, slaveEmptyPositions);
            free(order);
            free(id);
            printf("Inserted successfully\n");
            
        }
        else if (strcmp(input, "count-m") == 0) {
            printf("%d\n", count_m(masterKeys));
           
        }
        else if (strcmp(input, "count-s") == 0) {
            printf("%d\n", count_s(slaveKeys));
            
        }
        else if (strcmp(input, "count-m-s") == 0) {
            int* id = inputId();
            printf("%d\n", count_m_s(*id, masterKeys, slaveKeys));
            free(id);
            
        }
        else if (strcmp(input, "get-m") == 0) {
            int* id = inputId();
            Customer* customer = get_m(*id, masterKeys);
            printCustomer(customer);
            free(customer);
            free(id);
            
        }
        else if (strcmp(input, "get-s") == 0) {
            int* id = inputId();
            Order* order = get_s(*id, slaveKeys);
            printOrder(order);
            free(order);
            free(id);
            
        }
        else if (strcmp(input, "get-m-s") == 0) {
            int* id = inputId();
            Array* slaves = get_m_s(*id, masterKeys, slaveKeys);
            printArray(slaves);
            free(slaves);
            free(id);
            
        }
        else if (strcmp(input, "del-m") == 0) {
            int* id = inputId();
            del_m(*id, masterKeys, masterEmptyPositions, slaveKeys, slaveEmptyPositions);
            free(id);
            printf("Deleted successfully\n");
            
        }
        else if (strcmp(input, "del-s") == 0) {
            int* id = inputId();
            del_s(*id, masterKeys, slaveKeys, slaveEmptyPositions);
            free(id);
            printf("Deleted successfully\n");
            
        }
        else if (strcmp(input, "update-m") == 0) {
            Customer* customer = inputCustomer();
            int* id = inputId();
            update_m(customer, *id, masterKeys);
            free(customer);
            free(id);
            printf("Updated successfully\n");
            
        }
        else if (strcmp(input, "update-s") == 0) {
            Order* order = inputOrder();
            int* id = inputId();
            update_s(order, *id, masterKeys, slaveKeys, slaveEmptyPositions);
            free(order);
            free(id);
            printf("Updated successfully\n");
            
        }
        else if (strcmp(input, "clear") == 0) {
            clearAll(&masterKeys, &masterEmptyPositions, &slaveKeys, &slaveEmptyPositions);
            printf("Cleared successfully\n");
            
        }
        else if (strcmp(input, "utils-m") == 0) {
            Array* customers = getItems(customerTableName);
            printArray(customers);
            deleteArray(customers);

        }
        else if (strcmp(input, "utils-s") == 0) {
            Array* orders = getItems(orderTableName);
            printArray(orders);
            deleteArray(orders);
        }
        else if (strcmp(input, "stop") == 0) {
            break;
        }

        else {
            printf("Invalid input! Please, retry.\n");
            
        }
        
    }

    
    saveArray(customerTableName, 1, masterKeys);
    saveArray(customerTableName, 2, masterEmptyPositions);

    saveArray(orderTableName, 1, slaveKeys);
    saveArray(orderTableName, 2, slaveEmptyPositions);


    deleteArray(masterKeys);
    deleteArray(masterEmptyPositions);
    deleteArray(slaveKeys);
    deleteArray(slaveEmptyPositions);

    return 0;
}
