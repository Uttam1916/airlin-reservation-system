#ifndef HASH_H
#define HASH_H

#define TABLE_SIZE 20

typedef struct Passenger {
    int id;
    char name[50];
    char flight[30];
    struct Passenger *next;
} Passenger;

Passenger* hashTable[TABLE_SIZE];

void initHashTable();
int hashFunction(int id);
void insertPassenger(int id, const char *name, const char *flight);
void searchPassenger(int id);
void deletePassenger(int id);
void displayHashTable();

#endif
