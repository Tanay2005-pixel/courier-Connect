#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ds.h"


void   sign_up();
int    sign_in();
void   init_index();
int    auth_menu();        
void   agent_sign_up();
int    agent_sign_in();
void   agent_init_index();
int    agent_auth_menu();    

int    request_menu();
void   create_request();
void   view_requests();
void   delete_request();
int    suggestion_menu();
void   suggest_nearest();
void clear_screen() {
    printf("\x1b[2J\x1b[H");
}

void wait_for_enter() {
    int c;
    printf("\nPress ENTER to continue...");
    while ((c = getchar()) != '\n' && c != EOF) { }
}
void print_banner() {
    printf("============================================================\n");
    printf("            SMART BID & REQUEST MANAGEMENT SYSTEM           \n");
    printf("============================================================\n\n");
}
void print_subbanner(const char *title) {
    printf("------------------------------------------------------------\n");
    printf("  %s\n", title);
    printf("------------------------------------------------------------\n");
}
void customer_flow() {
    int done = 0;
    int choice;

    init_index(); 

    while (!done) {
        clear_screen();
        print_banner();
        print_subbanner("CUSTOMER PORTAL");

        printf("1) Sign Up\n");
        printf("2) Sign In\n");
        printf("3) Back to Main Menu\n");
        printf("\nEnter choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_screen();
            printf("Invalid input. Returning to main menu.\n");
            wait_for_enter();
            return;
        }

        clear_screen();

        switch (choice) {
            case 1:
                print_subbanner("CUSTOMER SIGN UP");
                sign_up();
                wait_for_enter();
                break;

            case 2: {
                print_subbanner("CUSTOMER SIGN IN");
                int ok = sign_in();
                if (ok) {
                    printf("\nAuthentication successful! Redirecting to REQUEST MODULE...\n");
                    wait_for_enter();
                    clear_screen();
                    request_menu();
                    done = 1; 
                } else {
                    printf("\nAuthentication failed. Try again.\n");
                    wait_for_enter();
                }
                break;
            }

            case 3:
                done = 1;
                break;

            default:
                printf("Invalid choice.\n");
                wait_for_enter();
                break;
        }
    }
}
void agent_flow() {
    int done = 0;
    int choice;

    agent_init_index();

    while (!done) {
        clear_screen();
        print_banner();
        print_subbanner("AGENT PORTAL");

        printf("1) Agent Sign Up\n");
        printf("2) Agent Sign In\n");
        printf("3) Back to Main Menu\n");
        printf("\nEnter choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_screen();
            printf("Invalid input. Returning to main menu.\n");
            wait_for_enter();
            return;
        }

        clear_screen();

        switch (choice) {
            case 1:
                print_subbanner("AGENT SIGN UP");
                agent_sign_up();
                wait_for_enter();
                break;

            case 2: {
                print_subbanner("AGENT SIGN IN");

                remove("curent.txt");
                agent_sign_in();
                FILE *fa = fopen("curent.txt", "r");
                if (!fa) {
                    printf("\nAuthentication failed or no agent logged in.\n");
                    wait_for_enter();
                    break; // back to AGENT PORTAL menu
                }

                char agent_name[100], agent_loc[100];
                if (!fgets(agent_name, sizeof(agent_name), fa) ||
                    !fgets(agent_loc, sizeof(agent_loc), fa)) {
                    fclose(fa);
                    printf("\nAgent login data is invalid. Returning to agent menu.\n");
                    wait_for_enter();
                    break;
                }
                fclose(fa);
                agent_name[strcspn(agent_name, "\r\n")] = 0;
                agent_loc[strcspn(agent_loc, "\r\n")] = 0;
                int running = 1;
                while (running) {
                    clear_screen();
                    print_banner();
                    print_subbanner("AGENT SUGGESTION MODULE");

                    printf("Logged in as: %s (Location: %s)\n\n", agent_name, agent_loc);
                    suggest_nearest();

                    printf("\n\n1) Bid / View Requests\n");
                    printf("0) Logout to Main Menu\n");
                    printf("\nEnter choice: ");

                    int ac;
                    if (scanf("%d", &ac) != 1) {
                        clear_screen();
                        printf("Invalid input. Returning to main menu.\n");
                        wait_for_enter();
                        running = 0;
                        done = 1;
                        break;
                    }

                    clear_screen();

                    switch (ac) {
                        case 1:
                            print_banner();
                            print_subbanner("AGENT BIDDING / REQUESTS");
                            suggestion_menu();
                            
                            break;

                        case 0:
                            running = 0;
                            done = 1; 
                            break;

                        default:
                            printf("Invalid choice.\n");
                            wait_for_enter();
                            break;
                    }
                }

                break;
            }

            case 3:
                done = 1;
                break;

            default:
                printf("Invalid choice.\n");
                wait_for_enter();
                break;
        }
    }
}



int main() {
    int running = 1;

    while (running) {
        int choice;

        clear_screen();
        print_banner();

        printf("Who are you?\n\n");
        printf("1) Customer\n");
        printf("2) Agent\n");
        printf("0) Exit\n");
        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_screen();
            printf("Invalid input. Exiting.\n");
            break;
        }

       
        clear_screen();

        switch (choice) {
            case 1:
                customer_flow();
                break;

            case 2:
                agent_flow();
                break;

            case 0:
                running = 0;
                break;

            default:
                printf("Invalid choice.\n");
                wait_for_enter();
                break;
        }
    }

    clear_screen();
    printf("Thank you for using the system. Goodbye!\n\n");
    return 0;
}
