#include <stdlib.h>

typedef struct {

    size_t cap;
    size_t *members;
    size_t memberCount;
    size_t removedMembers;
    size_t mask;
    size_t size;

}hashMembers;



