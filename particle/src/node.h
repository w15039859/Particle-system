
typedef struct particle {
    float x,y;
    float v_x, v_y;
    float ttl;
    struct particle *next;
} node_t;

void initialise_heap(void);
node_t *allocnode(void);
void freenode(node_t *n);
