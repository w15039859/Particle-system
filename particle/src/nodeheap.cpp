/* simple node allocator for linked list */
#include <stdlib.h>
#include <node.h>

static const int heapsize = 1000;
static node_t heap[heapsize];
static node_t *freenodes;
void initialise_heap(void)
{
    int n;
    for( n=0 ; n<(heapsize-1) ; n++) {
        heap[n].next = &heap[n+1];
    }
    heap[n].next = NULL;
    freenodes = &heap[0];
}

node_t *allocnode(void)
{
    node_t *node = NULL;
    if( freenodes ) {
        node = freenodes;
        freenodes = freenodes->next;
    }
    return node;
}

void freenode(node_t *n)
{
    n->next = freenodes;
    freenodes = n;
}
