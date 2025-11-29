

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "ds.h"

#define REQUEST_FILE        "requests.txt"
#define CURRENT_AGENT_FILE  "curent.txt"

#define MAX_LINE_LEN    1000
#define NUM_SUGGESTIONS 3

#define NUM_VERTICES    10   
#define INF_DISTANCE    1000000000

#define BID_BUCKET_DIR  "bid_buckets/"
#define BID_FILE_PREFIX "bid_req_"
#define MAX_BIDS        1000

typedef struct {
    char agent[100];
    long amount;
    long timestamp;
} Bid;

typedef struct {
    Bid data[MAX_BIDS];
    int size;
} BidHeap;


static void heap_init(BidHeap *h) {
    h->size = 0;
}

static int bid_less(const Bid *a, const Bid *b) {
    if (a->amount < b->amount) return 1;
    if (a->amount > b->amount) return 0;
    return (a->timestamp < b->timestamp);
}

static void bid_swap(Bid *a, Bid *b) {
    Bid tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapify_up(BidHeap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (!bid_less(&h->data[i], &h->data[p])) break;
        bid_swap(&h->data[i], &h->data[p]);
        i = p;
    }
}

static void heap_push(BidHeap *h, const Bid *b) {
    if (h->size >= MAX_BIDS) return;
    h->data[h->size] = *b;
    heapify_up(h, h->size);
    h->size++;
}


static long now_epoch() {
    return (long)time(NULL);
}

static int parse_request_time(const char *line, long *out_t) {
    const char *p = strstr(line, "Time");
    if (!p) return 0;

    while (*p && *p != ':') p++;
    if (*p != ':') return 0;
    p++;

    while (*p == ' ' || *p == '\t') p++;

    char *endptr;
    long val = strtol(p, &endptr, 10);
    if (p == endptr) return 0;

    *out_t = val;
    return 1;
}


static int request_within_one_hour(const char *line) {
    long t;
    if (!parse_request_time(line, &t)) return 0;
    long diff = now_epoch() - t;
    return (diff >= 0 && diff <= 3600);
}

static void ensure_bid_dir() {
#ifdef _WIN32
    _mkdir(BID_BUCKET_DIR);
#else
    struct stat st;
    if (stat(BID_BUCKET_DIR, &st) == -1) {
        mkdir(BID_BUCKET_DIR, 0700);
    }
#endif
}

static void make_bidfile(int request_id, char *out, int out_sz) {
    snprintf(out, out_sz, "%s%s%d.txt", BID_BUCKET_DIR, BID_FILE_PREFIX, request_id);
}

static void load_bids(const char *fname, BidHeap *heap) {
    heap_init(heap);
    FILE *f = fopen(fname, "r");
    if (!f) return;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        Bid b;
        if (sscanf(line, "%ld|%99[^|]|%ld", &b.amount, b.agent, &b.timestamp) == 3) {
            heap_push(heap, &b);
        }
    }
    fclose(f);
}

static void save_bids(const char *fname, const BidHeap *heap) {
    FILE *f = fopen(fname, "w");
    if (!f) return;

    for (int i = 0; i < heap->size; i++) {
        const Bid *b = &heap->data[i];
        fprintf(f, "%ld|%s|%ld\n", b->amount, b->agent, b->timestamp);
    }
    fclose(f);
}


static void get_current_agent(char *out, int out_sz) {
    FILE *f = fopen(CURRENT_AGENT_FILE, "r");
    if (!f) {
        strcpy(out, "Unknown");
        return;
    }
    if (!fgets(out, out_sz - 1, f)) {
        strcpy(out, "Unknown");
        fclose(f);
        return;
    }
    out[strcspn(out, "\r\n")] = 0;
    fclose(f);
}

static void bid_on_request(int id) {
    ensure_bid_dir();

    char agent[100];
    get_current_agent(agent, sizeof(agent));

    printf("Enter bid amount for Request %d: ", id);
    long amt;
    if (scanf("%ld", &amt) != 1) {
        printf("Invalid bid amount.\n");
        return;
    }

    char fname[256];
    make_bidfile(id, fname, sizeof(fname));

    BidHeap heap;
    load_bids(fname, &heap);

    Bid b;
    strncpy(b.agent, agent, sizeof(b.agent) - 1);
    b.agent[sizeof(b.agent) - 1] = '\0';
    b.amount = amt;
    b.timestamp = now_epoch();

    heap_push(&heap, &b);
    save_bids(fname, &heap);

    printf("Bid placed successfully for Request %d by %s\n", id, agent);
}


static int vertex_index_from_char(char c) {
    c = toupper((unsigned char)c);
    if (c < 'A' || c > 'J') return -1;
    return c - 'A';
}

static char idx_to_char(int idx) {
    if (idx < 0 || idx >= NUM_VERTICES) return '?';
    return (char)('A' + idx);
}

static void build_graph(Graph *g) {
    graph_init(g, NUM_VERTICES);

    graph_add_edge(g, 0, 1, 5); graph_add_edge(g, 1, 0, 5);
    graph_add_edge(g, 1, 2, 6); graph_add_edge(g, 2, 1, 6);
    graph_add_edge(g, 2, 3, 4); graph_add_edge(g, 3, 2, 4);
    graph_add_edge(g, 3, 4, 8); graph_add_edge(g, 4, 3, 8);

    
    graph_add_edge(g, 5, 6, 5); graph_add_edge(g, 6, 5, 5);
    graph_add_edge(g, 6, 7, 4); graph_add_edge(g, 7, 6, 4);
    graph_add_edge(g, 7, 8, 6); graph_add_edge(g, 8, 7, 6);
    graph_add_edge(g, 8, 9, 5); graph_add_edge(g, 9, 8, 5);

  
    graph_add_edge(g, 0, 5, 4); graph_add_edge(g, 5, 0, 4);  
    graph_add_edge(g, 1, 6, 7); graph_add_edge(g, 6, 1, 7);  
    graph_add_edge(g, 2, 7, 5); graph_add_edge(g, 7, 2, 5);  
    graph_add_edge(g, 3, 8, 6); graph_add_edge(g, 8, 3, 6);  
    graph_add_edge(g, 4, 9, 7); graph_add_edge(g, 9, 4, 7);  
}


static void dijkstra(Graph *g, int start, int dist[], int parent[]) {
    int visited[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++) {
        dist[i] = INF_DISTANCE;
        visited[i] = 0;
        parent[i] = -1;
    }
    dist[start] = 0;

    for (int step = 0; step < NUM_VERTICES; step++) {
        int u = -1, best = INF_DISTANCE;
        for (int i = 0; i < NUM_VERTICES; i++) {
            if (!visited[i] && dist[i] < best) {
                best = dist[i];
                u = i;
            }
        }
        if (u == -1) break;
        visited[u] = 1;

        for (Edge *e = g->adj[u]; e; e = e->next) {
            int v = e->vertex;
            if (!visited[v] && dist[u] + e->weight < dist[v]) {
                dist[v] = dist[u] + e->weight;
                parent[v] = u;
            }
        }
    }
}


static int build_path(int src, int dst, int parent[], int out_path[], int *out_len) {
    int temp[NUM_VERTICES];
    int len = 0;
    int cur = dst;

    while (cur != -1) {
        temp[len++] = cur;
        if (cur == src) break;
        cur = parent[cur];
    }

    if (len == 0 || temp[len - 1] != src) {
        *out_len = 0;
        return 0; 
    }

    for (int i = 0; i < len; i++) {
        out_path[i] = temp[len - 1 - i]; 
    }
    *out_len = len;
    return 1;
}


static void print_route_agent_sender_receiver(Graph *g, int agent_v,
                                              int sender_v, int receiver_v) {
    int dist[NUM_VERTICES], parent[NUM_VERTICES];
    int path1[NUM_VERTICES], len1;
    int path2[NUM_VERTICES], len2;

  
    dijkstra(g, agent_v, dist, parent);
    if (dist[sender_v] >= INF_DISTANCE) {
        printf("No path from %c to %c\n",
               idx_to_char(agent_v), idx_to_char(sender_v));
        return;
    }
    if (!build_path(agent_v, sender_v, parent, path1, &len1)) {
        printf("Failed to build path Agent->Sender\n");
        return;
    }
    int d1 = dist[sender_v];

 
    dijkstra(g, sender_v, dist, parent);
    if (dist[receiver_v] >= INF_DISTANCE) {
        printf("No path from %c to %c\n",
               idx_to_char(sender_v), idx_to_char(receiver_v));
        return;
    }
    if (!build_path(sender_v, receiver_v, parent, path2, &len2)) {
        printf("Failed to build path Sender->Receiver\n");
        return;
    }
    int d2 = dist[receiver_v];


    int full[NUM_VERTICES * 2];
    int flen = 0;
    for (int i = 0; i < len1; i++) full[flen++] = path1[i];
    for (int i = 1; i < len2; i++) full[flen++] = path2[i];

    printf("\nRoute (Agent -> Sender -> Receiver):\n");
    for (int i = 0; i < flen; i++) {
        printf("%c", idx_to_char(full[i]));
        if (i + 1 < flen) printf(" -> ");
    }
    printf("\nTotal Distance: %d\n", d1 + d2);
}


static int extract_sender_receiver_vertices(const char *line,
                                            int *sv, int *rv) {
    char s_name[80], s_phone[40], s_loc[20];
    char r_name[80], r_phone[40], r_loc[20];

  
    if (sscanf(line,
               "ID=%*d | Agent: %*[^|]| Sender: %79[^,],%39[^,],%19[^|]| Receiver: %79[^,],%39[^,],%19[^|]|",
               s_name, s_phone, s_loc,
               r_name, r_phone, r_loc) != 6) {
        return 0;
    }

    *sv = vertex_index_from_char(s_loc[0]);
    *rv = vertex_index_from_char(r_loc[0]);

    return (*sv >= 0 && *rv >= 0);
}


typedef struct {
    int id;
    int dist;
    char line[MAX_LINE_LEN];
    int used;
} Suggestion;


static void consider_suggestion(Suggestion best[], int id, int dist, const char *line) {
    if (dist >= INF_DISTANCE || id <= 0) return;

    int pos = -1;
    for (int i = 0; i < NUM_SUGGESTIONS; i++) {
        if (!best[i].used) { pos = i; break; }
    }

    if (pos == -1) {
        int worst = 0;
        for (int i = 1; i < NUM_SUGGESTIONS; i++)
            if (best[i].dist > best[worst].dist) worst = i;
        if (dist >= best[worst].dist) return;
        pos = worst;
    }

    best[pos].used = 1;
    best[pos].id   = id;
    best[pos].dist = dist;
    strncpy(best[pos].line, line, MAX_LINE_LEN - 1);
    best[pos].line[MAX_LINE_LEN - 1] = '\0';
}


void suggest_nearest() {
    Graph g;
    build_graph(&g);

    char agent_name[50], agent_loc[10];

    FILE *fa = fopen(CURRENT_AGENT_FILE, "r");
    if (!fa) {
        printf("Error: %s not found\n", CURRENT_AGENT_FILE);
        return;
    }
    if (!fgets(agent_name, sizeof(agent_name), fa)) {
        fclose(fa);
        printf("Error reading agent name\n");
        return;
    }
    agent_name[strcspn(agent_name, "\r\n")] = 0;

    if (!fgets(agent_loc, sizeof(agent_loc), fa)) {
        fclose(fa);
        printf("Error reading agent location\n");
        return;
    }
    agent_loc[strcspn(agent_loc, "\r\n")] = 0;
    fclose(fa);

    printf("\nLogged Agent: %s\nLocation: %s\n", agent_name, agent_loc);

    int start_v = vertex_index_from_char(agent_loc[0]);
    if (start_v < 0) {
        printf("Invalid agent location.\n");
        return;
    }

    int dist[NUM_VERTICES], parent[NUM_VERTICES];
    dijkstra(&g, start_v, dist, parent);

    Suggestion best[NUM_SUGGESTIONS] = {0};

    FILE *rf = fopen(REQUEST_FILE, "r");
    if (!rf) {
        printf("No requests found.\n");
        return;
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), rf)) {
        int id;
        if (sscanf(line, "ID=%d", &id) != 1) continue;

        int sv, rv;
        if (!extract_sender_receiver_vertices(line, &sv, &rv)) continue;

        consider_suggestion(best, id, dist[sv], line);
    }
    fclose(rf);

    printf("\n=== Top %d Nearest Requests ===\n", NUM_SUGGESTIONS);
    int shown = 0;
    for (int i = 0; i < NUM_SUGGESTIONS; i++) {
        if (!best[i].used) continue;
        shown++;
        int active = request_within_one_hour(best[i].line);
        printf("%d) ID=%d  Dist=%d  [%s]\n%s\n",
               i + 1,
               best[i].id,
               best[i].dist,
               active ? "ACTIVE" : "EXPIRED",
               best[i].line);
    }

    if (!shown) {
        printf("No nearby requests.\n");
        return;
    }

    printf("\n1) Bid on requests from above list (multiple)\n");
    printf("2) Enter Request IDs manually (multiple)\n");
    printf("0) Cancel\nChoice: ");
    int opt;
    if (scanf("%d", &opt) != 1) {
        printf("Invalid choice.\n");
        return;
    }
    if (opt == 0) return;

    if (opt == 1) {
        int count;
        printf("How many requests do you want to bid on? (1-%d): ", NUM_SUGGESTIONS);
        if (scanf("%d", &count) != 1 || count < 1 || count > NUM_SUGGESTIONS) {
            printf("Invalid count.\n");
            return;
        }

        for (int k = 0; k < count; k++) {
            int choice;
            printf("Select request number (1-%d): ", NUM_SUGGESTIONS);
            if (scanf("%d", &choice) != 1) {
                printf("Invalid selection.\n");
                return;
            }
            if (choice < 1 || choice > NUM_SUGGESTIONS) {
                printf("Out of range.\n");
                continue;
            }
            Suggestion *sel = &best[choice - 1];
            if (!sel->used) continue;

            if (!request_within_one_hour(sel->line)) {
                printf(" Request ID %d expired, skipping.\n", sel->id);
                continue;
            }

            int sv, rv;
            if (!extract_sender_receiver_vertices(sel->line, &sv, &rv)) {
                printf("Could not parse locations for ID %d\n", sel->id);
                continue;
            }

            bid_on_request(sel->id);
            print_route_agent_sender_receiver(&g, start_v, sv, rv);
        }
    }
    else if (opt == 2) {
        int count;
        printf("How many Request IDs do you want to bid on? (1-%d): ", NUM_SUGGESTIONS);
        if (scanf("%d", &count) != 1 || count < 1 || count > NUM_SUGGESTIONS) {
            printf("Invalid count.\n");
            return;
        }

        for (int k = 0; k < count; k++) {
            int reqid;
            printf("Enter Request ID: ");
            if (scanf("%d", &reqid) != 1) {
                printf("Invalid ID input.\n");
                return;
            }

            FILE *fr = fopen(REQUEST_FILE, "r");
            if (!fr) {
                printf("Cannot open %s\n", REQUEST_FILE);
                return;
            }

            int found = 0;
            char reqline[MAX_LINE_LEN];
            while (fgets(reqline, sizeof(reqline), fr)) {
                int id;
                if (sscanf(reqline, "ID=%d", &id) == 1 && id == reqid) {
                    found = 1;
                    if (!request_within_one_hour(reqline)) {
                        printf(" Request ID %d expired, skipping.\n", reqid);
                        break;
                    }

                    int sv, rv;
                    if (!extract_sender_receiver_vertices(reqline, &sv, &rv)) {
                        printf("Could not parse locations for ID %d\n", reqid);
                        break;
                    }

                    bid_on_request(reqid);
                    print_route_agent_sender_receiver(&g, start_v, sv, rv);
                    break;
                }
            }
            fclose(fr);
            if (!found) {
                printf("Request ID %d not found.\n", reqid);
            }
        }
    }
    else {
        printf("Invalid option.\n");
    }
}

int suggestion_menu() {
    suggest_nearest();
    return 0;
}
