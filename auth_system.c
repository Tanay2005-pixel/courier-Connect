#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ds.h"

#define BUCKETDIR "auth_buckets/"
#define BUCKETPREFIX "bucket_"
#define LOGDIR "user_logs/"
#define LOGPREFIX "log_"

#define NUMBUCKETS 10
#define TABLESIZE 997

#define INDEXFILE "index.txt"
#define INDEXSIZE 9997
#define INDEXPRIME 9991
#define BUCKETPRIME 991


void init_index() {
    FILE *f = fopen(INDEXFILE, "r");
    if (!f) {
        f = fopen(INDEXFILE, "w");
        if (!f) return;
        for (int i = 0; i < INDEXSIZE; i++)
            fprintf(f, "\n");
    }
    if (f) fclose(f);
}



void ensure_bucket(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) {
        f = fopen(file, "w");
        if (!f) return;
        for (int i = 0; i < TABLESIZE; i++)
            fprintf(f, "\n");
    }
    if (f) fclose(f);
}



void read_slot(const char *file, int index, char *out) {
    ensure_bucket(file);
    FILE *f = fopen(file, "r");
    if (!f) {
        out[0] = '\0';
        return;
    }

    char buf[500];
    int line = 0;
    int target = index - 1;   
    out[0] = '\0';

    if (target < 0) {
        fclose(f);
        return;
    }

    while (fgets(buf, 499, f)) {
        if (line == target) {
            str_strip_edges(buf, out);
            fclose(f);
            return;
        }
        line++;
    }
    fclose(f);
   
    out[0] = '\0';
}



void write_slot(const char *file, int index, const char *data) {
    ensure_bucket(file);
    FILE *f = fopen(file, "r");
    if (!f) return;

    char **lines = (char**)malloc(TABLESIZE * sizeof(char*));
    if (!lines) {
        fclose(f);
        return;
    }

    for (int i = 0; i < TABLESIZE; i++) {
        lines[i] = (char*)malloc(500);
        if (!lines[i]) {
           
            for (int k = 0; k < i; k++) free(lines[k]);
            free(lines);
            fclose(f);
            return;
        }

        if (!fgets(lines[i], 499, f))
            strcpy(lines[i], "\n");
    }
    fclose(f);

    int target = index - 1;  
    if (target < 0 || target >= TABLESIZE) {
       
        for (int i = 0; i < TABLESIZE; i++) free(lines[i]);
        free(lines);
        return;
    }

    snprintf(lines[target], 499, "%s\n", data);

    f = fopen(file, "w");
    if (!f) {
        for (int i = 0; i < TABLESIZE; i++) free(lines[i]);
        free(lines);
        return;
    }
    for (int i = 0; i < TABLESIZE; i++)
        fputs(lines[i], f);
    fclose(f);

    for (int i = 0; i < TABLESIZE; i++) free(lines[i]);
    free(lines);
}



int index_h1(const char *u) {
    int len = str_length(u);
    int h = len % INDEXSIZE;     
    if (h == 0) h = INDEXSIZE;   
    return h;
}

int index_h2(const char *u) {
    int len = str_length(u);
    int step = INDEXPRIME - (len % INDEXPRIME);
    if (step == 0) step = 1;
    return step;
}



int parse_index_line(const char *line, char *u, int *b, int *slot, int *logslot) {
    char t[500];
    str_strip_edges(line, t);
    if (str_length(t) == 0) return 0;

    char part[4][100] = {{0}};
    int sec = 0, si = 0;

    for (int i = 0; t[i] != '\0'; i++) {
        if (t[i] == '|') {
            sec++;
            si = 0;
        } else {
            part[sec][si++] = t[i];
        }
    }

    if (sec != 3) return 0;

    strcpy(u, part[0]);
    *b = atoi(part[1]);
    *slot = atoi(part[2]);      
    *logslot = atoi(part[3]);   
    return 1;
}


int index_insert(const char *u, int b, int slot) {
    FILE *f = fopen(INDEXFILE, "r");
    if (!f) return 0;

    char **lines = (char**)malloc(INDEXSIZE * sizeof(char*));
    if (!lines) {
        fclose(f);
        return 0;
    }

    for (int i = 0; i < INDEXSIZE; i++) {
        lines[i] = (char*)malloc(500);
        if (!lines[i]) {
            for (int k = 0; k < i; k++) free(lines[k]);
            free(lines);
            fclose(f);
            return 0;
        }
        if (!fgets(lines[i], 499, f))
            strcpy(lines[i], "\n");
    }
    fclose(f);

    int h1 = index_h1(u);  
    int h2 = index_h2(u);

    for (int i = 0; i < INDEXSIZE; i++) {
        int idx = (h1 + i * h2) % INDEXSIZE; 
        if (idx == 0) idx = INDEXSIZE;       

        int arr_idx = idx - 1;              

        char ru[100];
        int rb, rs, rl;

        if (!parse_index_line(lines[arr_idx], ru, &rb, &rs, &rl)) {
           
            snprintf(lines[arr_idx], 499, "%s|%d|%d|%d\n", u, b, slot, slot);

            f = fopen(INDEXFILE, "w");
            if (!f) {
                for (int k = 0; k < INDEXSIZE; k++) free(lines[k]);
                free(lines);
                return 0;
            }
            for (int j = 0; j < INDEXSIZE; j++)
                fputs(lines[j], f);
            fclose(f);
            break;
        }
    }

    for (int i = 0; i < INDEXSIZE; i++) free(lines[i]);
    free(lines);
    return 1;
}



int index_search(const char *u, int *bucket, int *slot, int *logslot) {
    init_index();
    FILE *f = fopen(INDEXFILE, "r");
    if (!f) return 0;

    char buf[500];
    int h1 = index_h1(u);  
    int h2 = index_h2(u);

    for (int i = 0; i < INDEXSIZE; i++) {
        int idx = (h1 + i * h2) % INDEXSIZE;   
        if (idx == 0) idx = INDEXSIZE;        

        
        fseek(f, 0, SEEK_SET);
        for (int j = 0; j < idx; j++) {
            if (!fgets(buf, 499, f)) {
                buf[0] = '\0';
                break;
            }
        }

        char ru[100];
        int rb, rs, rl;
        if (!parse_index_line(buf, ru, &rb, &rs, &rl)) continue;

        if (str_compare(ru, u) == 0) {
            *bucket = rb;
            *slot = rs;      
            *logslot = rl;   
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}



int hash1(const char *u) {
    return str_length(u) % TABLESIZE;   
}

int hash2(const char *u) {
    int step = BUCKETPRIME - (str_length(u) % BUCKETPRIME);
    return (step == 0) ? 1 : step;
}


int get_bucket_index(const char *u) {
    return str_length(u) % NUMBUCKETS;
}


void extract_username(const char *rec, char *out) {
    int idx = str_find(rec, " - ");
    if (idx == -1) { out[0] = '\0'; return; }
    str_substring(rec, 0, idx, out);
}

void extract_password(const char *rec, char *out) {
    int idx = str_find(rec, " - ");
    if (idx == -1) { out[0] = '\0'; return; }
    str_substring(rec, idx + 3, str_length(rec), out);
}


int insert_user(const char *u, const char *p) {
    int b, s, l;
    if (index_search(u, &b, &s, &l)) return 0; 

    b = get_bucket_index(u);
    char file[200];
    sprintf(file, "%s%s%d.txt", BUCKETDIR, BUCKETPREFIX, b);
    ensure_bucket(file);

    int h1 = hash1(u);
    int h2 = hash2(u);

    for (int i = 0; i < TABLESIZE; i++) {
        int pos0 = (h1 + i * h2) % TABLESIZE; 
        int slot = pos0;                      

      
        if (slot == 0) slot = TABLESIZE;

        char rec[300];
        read_slot(file, slot, rec);  

        if (str_length(rec) == 0) {
            char buf[300];
            sprintf(buf, "%s - %s", u, p);

            write_slot(file, slot, buf);

            index_insert(u, b, slot);
            return 1;
        }
    }
    return 0;
}


int search_user(const char *u, const char *p) {
    int b, s, l;
    if (!index_search(u, &b, &s, &l)) return 0;

    char file[200];
    sprintf(file, "%s%s%d.txt", BUCKETDIR, BUCKETPREFIX, b);

    char rec[300];
    read_slot(file, s, rec);   

    char su[100], sp[100];
    extract_username(rec, su);
    extract_password(rec, sp);

    return (str_compare(u, su) == 0 && str_compare(p, sp) == 0);
}



void insert_record(const char *u, const char *entry) {
    int b, s, l;
    if (!index_search(u, &b, &s, &l)) return;

    char file[200];
    sprintf(file, "%s%s%d.txt", LOGDIR, LOGPREFIX, b);
    ensure_bucket(file);

    char rec[500];
    read_slot(file, l, rec);  

    char buf[500];
    if (str_length(rec) == 0)
        sprintf(buf, "%s", entry);
    else
        sprintf(buf, "%s | %s", rec, entry);

    write_slot(file, l, buf);
}


void sign_up(){
    char u[100], p[100], t[200];

    printf("Enter username: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, u);

    printf("Enter password: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, p);

    if (str_length(p) < 4) {
        printf("Password too short!\n");
        return;
    }

    if (insert_user(u, p))
        printf("Registration success!\n");
    else
        printf("User already exists!\n");
}

int sign_in(){
    char u[100], p[100], t[200];

    printf("Enter username: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, u);

    printf("Enter password: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, p);

    FILE* curr = fopen("curent.txt", "w");
    if (!curr) {
        printf("Error opening curent.txt\n");
        return 0;
    }

    if (search_user(u, p)) {
        insert_record(u, "SIGN_IN");
        printf("Welcome %s\n", u);
        fprintf(curr, "%s\n", u);
        fclose(curr);
        return 1;
    } else {
        printf("Invalid credentials\n");
        fclose(curr);
        return 0;
    }
}



int auth_menu() {
    init_index();

    while (1) {
        int c;
        printf("\n1. Sign Up\n2. Sign In\n3. Exit\nChoice: ");
        scanf("%d", &c);

        if (c == 1) sign_up();
        else if (c == 2) sign_in();
        else break;
    }
    return 0;
}