#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ds.h"

#define AGENT_BUCKETDIR    "agent_buckets/"
#define AGENT_BUCKETPREFIX "agent_bucket_"
#define AGENT_LOGDIR       "agent_logs/"
#define AGENT_LOGPREFIX    "agent_log_"

#define NUMBUCKETS 10
#define TABLESIZE  997

#define AGENT_INDEXFILE "agent_index.txt"
#define INDEXSIZE       9997
#define INDEXPRIME      9991
#define BUCKETPRIME     991

void agent_init_index() {
    FILE *f = fopen(AGENT_INDEXFILE, "r");
    if (!f) {
        f = fopen(AGENT_INDEXFILE, "w");
        if (!f) return;
        for (int i = 0; i < INDEXSIZE; i++)
            fprintf(f, "\n");
    }
    if (f) fclose(f);
}


void agent_ensure_bucket(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) {
        f = fopen(file, "w");
        if (!f) return;
        for (int i = 0; i < TABLESIZE; i++)
            fprintf(f, "\n");
    }
    if (f) fclose(f);
}


void agent_read_slot(const char *file, int index, char *out) {
    agent_ensure_bucket(file);
    FILE *f = fopen(file, "r");
    if (!f) { out[0]='\0'; return; }

    int target = index - 1;
    char buf[500];
    out[0] = '\0';

    for (int i = 0; fgets(buf, 499, f); i++) {
        if (i == target) {
            str_strip_edges(buf, out);
            fclose(f);
            return;
        }
    }
    fclose(f);
}

void agent_write_slot(const char *file, int index, const char *data) {
    agent_ensure_bucket(file);
    FILE *f = fopen(file, "r");
    if (!f) return;

    char **lines = malloc(TABLESIZE * sizeof(char*));
    for (int i = 0; i < TABLESIZE; i++) {
        lines[i] = malloc(500);
        if (!fgets(lines[i], 499, f))
            strcpy(lines[i], "\n");
    }
    fclose(f);

    snprintf(lines[index - 1], 499, "%s\n", data);

    f = fopen(file, "w");
    for (int i = 0; i < TABLESIZE; i++) fputs(lines[i], f);
    fclose(f);

    for (int i = 0; i < TABLESIZE; i++) free(lines[i]);
    free(lines);
}


int agent_index_h1(const char *u) {
    int len = str_length(u);
    int h = len % INDEXSIZE;
    return (h == 0) ? INDEXSIZE : h;
}

int agent_index_h2(const char *u) {
    int len = str_length(u);
    int step = INDEXPRIME - (len % INDEXPRIME);
    return (step == 0) ? 1 : step;
}



int agent_parse_index_line(const char *line, char *u, int *b, int *slot, int *logslot) {
    char t[500];
    str_strip_edges(line, t);
    if (str_length(t) == 0) return 0;

    char part[4][100] = {{0}};
    int section = 0, si = 0;

    for (int i = 0; t[i]; i++) {
        if (t[i] == '|') { section++; si = 0; }
        else part[section][si++] = t[i];
    }
    if (section != 3) return 0;

    strcpy(u, part[0]);
    *b = atoi(part[1]);
    *slot = atoi(part[2]);
    *logslot = atoi(part[3]);
    return 1;
}



int agent_index_insert(const char *u, int b, int slot) {
    FILE *f = fopen(AGENT_INDEXFILE, "r");
    if (!f) return 0;

    char **lines = malloc(INDEXSIZE * sizeof(char*));
    for (int i = 0; i < INDEXSIZE; i++) {
        lines[i] = malloc(500);
        if (!fgets(lines[i], 499, f))
            strcpy(lines[i], "\n");
    }
    fclose(f);

    int h1 = agent_index_h1(u);
    int h2 = agent_index_h2(u);

    for (int i = 0; i < INDEXSIZE; i++) {
        int idx = (h1 + i*h2) % INDEXSIZE;
        if (idx == 0) idx = INDEXSIZE;

        int arr = idx - 1;
        char ru[100]; int rb, rs, rl;

        if (!agent_parse_index_line(lines[arr], ru, &rb, &rs, &rl)) {
            sprintf(lines[arr], "%s|%d|%d|%d\n", u, b, slot, slot);

            f = fopen(AGENT_INDEXFILE, "w");
            for (int j = 0; j < INDEXSIZE; j++) fputs(lines[j], f);
            fclose(f);
            break;
        }
    }

    for (int i = 0; i < INDEXSIZE; i++) free(lines[i]);
    free(lines);
    return 1;
}


int agent_index_search(const char *u, int *b, int *slot, int *logslot) {
    agent_init_index();
    FILE *f = fopen(AGENT_INDEXFILE, "r");
    if (!f) return 0;

    char buf[500];
    int h1 = agent_index_h1(u);
    int h2 = agent_index_h2(u);

    for (int i = 0; i < INDEXSIZE; i++) {
        int idx = (h1 + i*h2) % INDEXSIZE;
        if (idx == 0) idx = INDEXSIZE;

        fseek(f, 0, SEEK_SET);
        for (int j = 0; j < idx; j++) fgets(buf, 499, f);

        char ru[100]; int rb, rs, rl;
        if (!agent_parse_index_line(buf, ru, &rb, &rs, &rl)) continue;

        if (str_compare(u, ru) == 0) {
            *b = rb; *slot = rs; *logslot = rl;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}


int agent_hash1(const char *u) { return str_length(u) % TABLESIZE; }
int agent_hash2(const char *u) {
    int step = BUCKETPRIME - (str_length(u) % BUCKETPRIME);
    return (step == 0) ? 1 : step;
}

int agent_get_bucket_index(const char *u) { return str_length(u) % NUMBUCKETS; }


void agent_extract_username(const char *rec, char *out) {
    int idx = str_find(rec, " - ");
    if (idx == -1) { out[0]='\0'; return; }
    str_substring(rec, 0, idx, out);
}

void agent_extract_password(const char *rec, char *out) {
    int idx1 = str_find(rec, " - ");
    int idx2 = -1;
    int len = str_length(rec);

    for (int i = idx1 + 3; i < len - 2; i++) {
        if (rec[i]==' ' && rec[i+1]=='-' && rec[i+2]==' ') {
            idx2 = i; break;
        }
    }

    if (idx2 == -1)
        str_substring(rec, idx1+3, len, out);
    else
        str_substring(rec, idx1+3, idx2, out);
}

void agent_extract_agency(const char *rec, char *out) {
    int len = str_length(rec);
    int count = 0;
    int i2 = -1;

    for (int i = 0; i < len - 2; i++) {
        if (rec[i]==' ' && rec[i+1]=='-' && rec[i+2]==' ') {
            count++;
            if (count == 2) { i2 = i; break; }
        }
    }
    if (i2 == -1) { out[0]='\0'; return; }
    str_substring(rec, i2+3, len, out);
}



int agency_exists(const char *agency) {
    FILE *f = fopen("agency.txt", "r");
    if (!f) return 0;

    char buf[200], t[200];
    while (fgets(buf, 199, f)) {
        str_strip_edges(buf, t);
        if (str_compare(t, agency)==0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}


int agent_insert(const char *u, const char *p, const char *agency) {
    int b, s, l;
    if (!agency_exists(agency)) {
        printf("Invalid agency name!\n");
        return 0;
    }

    if (agent_index_search(u, &b, &s, &l)) return 0;

    b = agent_get_bucket_index(u);
    char file[200];
    sprintf(file, "%s%s%d.txt", AGENT_BUCKETDIR, AGENT_BUCKETPREFIX, b);
    agent_ensure_bucket(file);

    int h1 = agent_hash1(u);
    int h2 = agent_hash2(u);

    for (int i = 0; i < TABLESIZE; i++) {
        int pos = (h1 + i*h2) % TABLESIZE;
        if (pos == 0) pos = TABLESIZE;

        char rec[300];
        agent_read_slot(file, pos, rec);

        if (str_length(rec)==0) {
            sprintf(rec, "%s - %s - %s", u, p, agency);

            agent_write_slot(file, pos, rec);
            agent_index_insert(u, b, pos);
            return 1;
        }
    }
    return 0;
}



int agent_search(const char *u, const char *p) {
    int b, s, l;
    if (!agent_index_search(u, &b, &s, &l)) return 0;

    char file[200];
    sprintf(file, "%s%s%d.txt", AGENT_BUCKETDIR, AGENT_BUCKETPREFIX, b);

    char rec[300];
    agent_read_slot(file, s, rec);

    char su[100], sp[100], sa[100];
    agent_extract_username(rec, su);
    agent_extract_password(rec, sp);
    agent_extract_agency(rec, sa);

    return (str_compare(u, su)==0 && str_compare(p, sp)==0);
}


void agent_insert_record(const char *u, const char *entry) {
    int b, s, l;
    if (!agent_index_search(u, &b, &s, &l)) return;

    char file[200];
    sprintf(file, "%s%s%d.txt", AGENT_LOGDIR, AGENT_LOGPREFIX, b);
    agent_ensure_bucket(file);

    char rec[500];
    agent_read_slot(file, l, rec);

    if (str_length(rec)==0) sprintf(rec, "%s", entry);
    else {
        strcat(rec, " | ");
        strcat(rec, entry);
    }
    agent_write_slot(file, l, rec);
}


void agent_sign_up() {
    char u[100], p[100], a[100], t[200];

    printf("Enter agent username: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, u);

    printf("Enter password: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, p);

    printf("Enter agency name: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, a);

    if (str_length(p) < 4) {
        printf("Password too short!\n");
        return;
    }

    if (agent_insert(u, p, a))
        printf("Agent registered successfully!\n");
    else
        printf("Registration failed! Duplicate agent or invalid agency.\n");
}

int agent_sign_in(){
    char u[100], p[100], loc[100], t[200];

    printf("Enter agent username: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, u);

    printf("Enter password: ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, p);

    printf("Enter current location (A-J): ");
    scanf(" %[^\n]", t);
    str_strip_edges(t, loc);

    if (agent_search(u, p)) {
        FILE* curr = fopen("curent.txt", "w");
        if (!curr) {
            printf("Error opening curent.txt\n");
            return 0;
        }

    
        char log_entry[200];
        sprintf(log_entry, "AGENT_SIGN_IN from %s", loc);
        agent_insert_record(u, log_entry);

        printf("Welcome Agent %s! (Location logged)\n", u);

        fprintf(curr, "%s\n", u);
        fprintf(curr, "%s\n", loc);

        fclose(curr);
        return 1;
    } else {
        printf("Invalid credentials.\n");
        return 0;
    }
}




int agent_auth_menu() {
    agent_init_index();

    while (1) {
        int c;
        printf("\n==== AGENT AUTH SYSTEM ====\n");
        printf("1. Agent Sign Up\n");
        printf("2. Agent Sign In\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &c);

        if (c == 1) agent_sign_up();
        else if (c == 2) agent_sign_in();
        else break;
    }
    return 0;
}
