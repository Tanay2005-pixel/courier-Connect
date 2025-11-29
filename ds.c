#include <stdio.h>
#include <stdlib.h>
#include "ds.h"

int str_length(const char *s) {
    int count = 0;
    while(s[count] != '\0') count++;
    return count;
}

int str_compare(const char *str1, const char *str2) {
    int i = 0;
    int len1 = str_length(str1);
    int len2 = str_length(str2);

    while(i < len1 && i < len2) {
        if(str1[i] < str2[i]) return -1;
        if(str1[i] > str2[i]) return 1;
        i++;
    }
    if(len1 < len2) return -1;
    if(len1 > len2) return 1;
    return 0;
}

void str_substring(const char *s, int start, int end, char *result) {
    int len = str_length(s);
    if(end > len) end = len;

    int j = 0;
    for(int i = start; i < end && i < len; i++)
        result[j++] = s[i];
    result[j] = '\0';
}

void str_strip_edges(const char *s, char *result) {
    int start = 0;
    int end = str_length(s) - 1;

    while(start <= end && (s[start]==' '||s[start]=='\t'||s[start]=='\n'||s[start]=='\r'))
        start++;
    while(end >= start && (s[end]==' '||s[end]=='\t'||s[end]=='\n'||s[end]=='\r'))
        end--;

    str_substring(s, start, end+1, result);
}

int str_find(const char *s, const char *pattern) {
    int n = str_length(s);
    int m = str_length(pattern);

    if(m == 0 || m > n) return -1;

    for(int i = 0; i <= n - m; i++) {
        int match = 1;
        for(int j = 0; j < m; j++) {
            if(s[i+j] != pattern[j]) { match = 0; break; }
        }
        if(match) return i;
    }
    return -1;
}


void stack_init(Stack *s) { s->size = 0; }

void stack_push(Stack *s, int item) {
    s->items[s->size++] = item;
}

int stack_pop(Stack *s) {
    if(s->size == 0) return -1;
    return s->items[--s->size];
}

int stack_peek(Stack *s) {
    if(s->size == 0) return -1;
    return s->items[s->size - 1];
}

int stack_size(Stack *s) {
    return s->size;
}

void queue_init(Queue *q) { q->size = 0; q->front = 0; }

void queue_enqueue(Queue *q, int item) {
    q->items[q->size++] = item;
}

int queue_dequeue(Queue *q) {
    if(q->size == 0) return -1;
    int item = q->items[q->front++];
    q->size--;
    return item;
}

int queue_front(Queue *q) {
    if(q->size == 0) return -1;
    return q->items[q->front];
}

int queue_size(Queue *q) {
    return q->size;
}

int heap_left_child(int key, int size) {
    int v = key * 2 + 1;
    return (v < size) ? v : -1;
}

int heap_right_child(int key, int size) {
    int v = key * 2 + 2;
    return (v < size) ? v : -1;
}

void heap_pre_up(int arr[], int size) {
    int key = size - 1;
    int temp = arr[key];
    while(key > 0 && arr[(key - 1) / 2] > temp) {
        arr[key] = arr[(key - 1) / 2];
        key = (key - 1) / 2;
    }
    arr[key] = temp;
}

void heap_pre_down(int arr[], int key, int size) {
    int min_idx = key;
    int lc = heap_left_child(key, size);
    int rc = heap_right_child(key, size);

    if(lc != -1 && arr[lc] < arr[min_idx]) min_idx = lc;
    if(rc != -1 && arr[rc] < arr[min_idx]) min_idx = rc;

    if(min_idx != key) {
        int temp = arr[min_idx];
        arr[min_idx] = arr[key];
        arr[key] = temp;
        heap_pre_down(arr, min_idx, size);
    }
}

void makeset(int parent[], int rank[], int size) {
    for(int i = 0; i < size; i++) {
        parent[i] = i;
        rank[i] = 1;
    }
}

int uf_find(int parent[], int size, int element) {
    if(element < 0 || element >= size) return -1;
    if(parent[element] == element) return element;
    parent[element] = uf_find(parent, size, parent[element]);
    return parent[element];
}

void uf_union_by_rank(int parent[], int rank[], int size, int a, int b) {
    int pa = uf_find(parent, size, a);
    int pb = uf_find(parent, size, b);

    if(pa == pb) return;

    if(rank[pa] > rank[pb]) parent[pb] = pa;
    else if(rank[pa] < rank[pb]) parent[pa] = pb;
    else {
        parent[pa] = pb;
        rank[pb]++;
    }
}


void graph_init(Graph *g, int vertices) {
    g->vertices = vertices;
    for(int i = 0; i < vertices; i++)
        g->adj[i] = NULL;
}

void graph_add_edge(Graph *g, int u, int v, int weight) {
    Edge *e = (Edge*)malloc(sizeof(Edge));
    e->vertex = v;
    e->weight = weight;
    e->next = g->adj[u];
    g->adj[u] = e;
}

void graph_print(Graph *g) {
    for(int i = 0; i < g->vertices; i++) {
        printf("Vertex %d: ", i);
        Edge *temp = g->adj[i];
        while(temp != NULL) {
            printf("(%d, %d) ", temp->vertex, temp->weight);
            temp = temp->next;
        }
        printf("\n");
    }
}
