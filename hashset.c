#include "hashset.h"
#include <assert.h>


//https://github.com/avsej/hashset.c/blob/master/hashset.c
hashMembers* initHashset(){

    hashMembers* hashSet = (hashMembers*) malloc(1, sizeof(hashMembers));

    hashSet->size = (size_t*) malloc(sizeof(size_t));
    hashSet->size = 3;

    hashSet->cap = (size_t*) malloc(sizeof(size_t));
    hashSet->cap = 8;

    hashSet->members = (size_t**) malloc(hashSet->cap, sizeof(size_t*));

    hashSet->memberCount = (size_t*) malloc(sizeof(size_t));
    hashSet->memberCount = 0;

    hashSet->removedMembers = (size_t*) malloc(sizeof(size_t));
    hashSet->removedMembers = 0;

    hashSet->mask = (size_t*) malloc(sizeof(size_t));
    hashSet->mask = 2;
    
    return hashSet;

}

/**
 * change the void param to the page type eventually, idk what it should be now
 */ 
int add(hashMembers* set, void* newMember){

    size_t newSize = (size_t)newMember;

    size_t index = set->mask & (13 * newSize);

    while(!set->members[index] != 0 || !set->members[index] != 1){

        

    }

}

int remove(){

}

int rehashCheck(){

}

void rehash(){

}

