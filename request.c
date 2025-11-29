#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ds.h"


int generate_request_id() {
    FILE *f = fopen("requests.txt", "a+"); 
    if (!f) {
        printf(" Failed to create/read %s\n", "requests.txt");
        return 1;
    }

    rewind(f);
    int id, max_id = 0;
    char line[1000];

    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%*[^0-9]%d", &id) == 1)
            if (id > max_id) max_id = id;
    }
    fclose(f);
    return max_id + 1;
}

void write_request(const char *record) {
    FILE *f = fopen("requests.txt", "a");
    if (!f) {
        printf(" Error writing to %s\n", "requests.txt");
        return;
    }
    fprintf(f, "%s\n", record);
    fclose(f);
}

void get_current_agent(char *out) {
    FILE *f = fopen("curent.txt", "r");
    if (!f) {
        strcpy(out, "Unknown");
        return;
    }
    if (!fgets(out, 99, f)) strcpy(out, "Unknown");
    out[strcspn(out, "\n")] = 0;
    fclose(f);
}


void create_request() {
    char s_name[100], s_phone[50], s_loc[10];
    char r_name[100], r_phone[50], r_loc[10];
    char box[50], dim[50], wt[50];
    char agent[100];

    get_current_agent(agent);

    int id = generate_request_id();

    printf("\n--- Create New Request ---\n");

    printf("Sender Name: "); scanf(" %[^\n]", s_name);
    printf("Sender Phone: "); scanf(" %[^\n]", s_phone);
    printf("Sender Location(A-J): "); scanf(" %[^\n]", s_loc);

    printf("Receiver Name: "); scanf(" %[^\n]", r_name);
    printf("Receiver Phone: "); scanf(" %[^\n]", r_phone);
    printf("Receiver Location(A-J): "); scanf(" %[^\n]", r_loc);

    printf("Box Type: "); scanf(" %[^\n]", box);
    printf("Dimensions: "); scanf(" %[^\n]", dim);
    printf("Weight: "); scanf(" %[^\n]", wt);

    long epoch_time = (long)time(NULL);

    char record[1000];
    snprintf(record, sizeof(record),
             "ID=%d | Agent: %s | Sender: %s,%s,%s | Receiver: %s,%s,%s | Box: %s | Dim: %s | Wt: %s | Time: %ld",
             id, agent,
             s_name, s_phone, s_loc,
             r_name, r_phone, r_loc,
             box, dim, wt,
             epoch_time);

    write_request(record);

    printf("\n Request Created Successfully!\n");
    printf(" Request ID = %d\n\n", id);
}


void view_requests() {
    FILE *f = fopen("requests.txt", "r");
    if (!f) {
        printf(" No requests found!\n");
        return;
    }
    printf("\n--- Existing Requests ---\n");
    char line[1000];
    while (fgets(line, sizeof(line), f))
        printf("%s", line);
    fclose(f);
    printf("\n");
}


void delete_request() {
    int id;
    printf("Enter Request ID to delete: ");
    scanf("%d", &id);

    FILE *f = fopen("requests.txt", "r");
    if (!f) {
        printf(" No file found.\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    char line[1000];
    int removed = 0;

    while (fgets(line, sizeof(line), f)) {
        int rid;
        if (sscanf(line, "%*[^0-9]%d", &rid) == 1 && rid == id) {
            removed = 1;
            continue;
        }
        fputs(line, temp);
    }
    fclose(f);
    fclose(temp);

    remove("requests.txt");
    rename("temp.txt", "requests.txt");

    if (removed)
        printf(" Request %d deleted.\n\n", id);
    else
        printf(" Request ID not found.\n\n");
}


int request_menu() {
    int choice;

    while (1) {
        printf("\n=== REQUEST MENU ===\n");
        printf("1 Create Request\n");
        printf("2 View Requests\n");
        printf("3 Delete Request\n");
        printf("0️  Exit\n");
        printf("Select: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: create_request(); break;
            case 2: view_requests(); break;
            case 3: delete_request(); break;
            case 0: printf("Exiting...\n"); return 0;
            default: printf("Invalid choice!\n");
        }
    }
}
