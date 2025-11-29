#ifndef DS_H
#define DS_H



int str_length(const char *s);
int str_compare(const char *s1, const char *s2);
void str_strip_edges(const char *src, char *dest);
int str_find(const char *str, const char *pattern);
void str_substring(const char *src, int start, int end, char *dest);



typedef struct {
    int items[1000];
    int size;
} Stack;

void stack_init(Stack *s);
void stack_push(Stack *s, int item);
int stack_pop(Stack *s);
int stack_peek(Stack *s);
int stack_size(Stack *s);




typedef struct {
    int items[1000];
    int size;
    int front;
} Queue;

void queue_init(Queue *q);
void queue_enqueue(Queue *q, int item);
int queue_dequeue(Queue *q);
int queue_front(Queue *q);
int queue_size(Queue *q);



typedef struct Edge {
    int vertex;
    int weight;
    struct Edge *next;
} Edge;

typedef struct {
    int vertices;
    Edge *adj[100];
} Graph;

void graph_init(Graph *g, int vertices);
void graph_add_edge(Graph *g, int u, int v, int weight);
void graph_print(Graph *g);

#endif
