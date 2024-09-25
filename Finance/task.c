#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <math.h>
#include "task.h"

//CoPilot was used for debugging the finalize(stmt) issues where database was locked
//https://copilot.microsoft.com/?showconv=1

int main (void){
    int choice;
    int choice2;
    int finance_id = -1;
    int login_status = SQLITE_ERROR;

    displayMenu();
    scanf("%d", &choice);
    switch (choice){
        case 1:
            make_user(&finance_id);
            break;
        case 2:
            login_status = login(&finance_id);
            break;
        case 3:
            return 0;
        default:
            printf("ERROR: Not an available option\n");
    }

    while (login_status == SQLITE_OK && finance_id !=-1) {
        displayFinance();
        scanf("%d", &choice2);
        switch(choice2){
            case 1:
                update_expenses(&expenses,old_expenses,finance_id);
                break;
            case 2:
                update_income(&income,old_income, finance_id);
                break;
            case 3:
                statistics(finance_id);
                return 0;
            case 4:
                printf("Logging out....\n");
                return 0;
            default:
                return 0;
        }
    }

    if (login_status == SQLITE_NOTFOUND){
        printf("Please try logging in again\n");
        return 0;
    }


}
