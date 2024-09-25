#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "task.h"

double income = 0.0;
double expenses = 0.0;
double old_income =0.0;
double old_expenses = 0.0;

int insert_user(const char *first_name, const char *last_name , const char *username, const char *pin_str, int finance_id){
    //CS50 chat bot helped with implementing sql into C
    sqlite3 *db;
    int result = sqlite3_open("database.db", &db);  //Opens the database

    if (result != SQLITE_OK){ //Check if open database was successful
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return result;
    }
    const char *sql = "INSERT INTO user (first_name, last_name, username, pin_str, finance_id) VALUES (?, ?, ?, ?, ?)"; //SQL commands
    sqlite3_stmt *stmt;

    result = sqlite3_prepare_v2(db,sql,-1,&stmt, NULL); //Prepare statement for execution
    if (result != SQLITE_OK){//Check if preparation is successful
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return result;
    }
    //Binds the value listed above
    sqlite3_bind_text(stmt, 1,first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, pin_str, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, finance_id);

    result = sqlite3_step(stmt);
    if(result != SQLITE_DONE) {//Checks if execution was finished properly
        printf("Execution Fail: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return result;
    }
    sqlite3_finalize(stmt);//Finalize statement
    sqlite3_close(db);//Close database

    return SQLITE_OK;//Same as returning 0

}


int insert_finance(int *finance_id){
    //CS50 chat bot helped with implementing SQL into C
    sqlite3 *db;
    int result = sqlite3_open("database.db", &db);//Opens the database

    if (result != SQLITE_OK){ //Check if opening database was successful
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return result;
    }
    const char *sql = "INSERT INTO finance (expenses, income) VALUES (0,0)"; //Insert the expenses and income into the account as 0
    sqlite3_stmt *stmt;
    result = sqlite3_prepare_v2(db,sql,-1,&stmt, NULL); //Prepares the statement for execution
    if (result != SQLITE_OK){ //Checks if the preparation is successfull
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return result;
    }

    result = sqlite3_step(stmt);
    if(result != SQLITE_DONE) { //Checks if the execution is finished
        printf("Execution Fail: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return result;
    }

    *finance_id = (int) sqlite3_last_insert_rowid(db);

    sqlite3_finalize(stmt); //Finalize the statement
    sqlite3_close(db); //Close the database

    return SQLITE_OK;

}


int statistics(int finance_id) {
    sqlite3*db;
    sqlite3_stmt *stmt;
    int result;

    result = sqlite3_open("database.db", &db);
    if (result != SQLITE_OK){
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return result;
    }

    const char *select_sql = "SELECT expenses, income FROM finance WHERE id = ?";
    result = sqlite3_prepare_v2(db,select_sql,-1,&stmt, NULL);
    if (result != SQLITE_OK){//Checks if preparation was successful
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return result;
    }

    sqlite3_bind_int(stmt, 1, finance_id);

    if (sqlite3_step(stmt) == SQLITE_ROW){
        expenses = sqlite3_column_double(stmt,0);
        income = sqlite3_column_double(stmt,1);
    }


    const char *archived_sql = "SELECT old_expenses, old_income FROM old_finance WHERE id = ?";
    result = sqlite3_prepare_v2(db,archived_sql,-1,&stmt, NULL);
    if (result != SQLITE_OK){//Checks if preparation was successful
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return result;
    }

    sqlite3_bind_int(stmt, 1, finance_id);

    if (sqlite3_step(stmt) == SQLITE_ROW){
        old_expenses = sqlite3_column_double(stmt,0);
        old_income = sqlite3_column_double(stmt,1);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    printf("\n========================Finance Statistics=========================\n");
    printf("Last Month Income: %.2f This Month Income: %.2f\n", old_income, income);
    printf("Last Month Expenses: %.2f This Month Expenses: %.2f\n", old_expenses, expenses);
    printf("===================================================================\n");

    return SQLITE_OK;

}

int update_income(double *income, double old_income, int finance_id){
    sqlite3*db;
    sqlite3_stmt *stmt;
    int result;


    printf("Please input your income for this month: \n");
    scanf("%lf", income);

    result = sqlite3_open("database.db", &db); //Opens the database file
    if (result != SQLITE_OK){ //Check if opening was successful
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return result;
    }

    const char *select_sql = "SELECT income FROM finance WHERE id = ?"; //Insert the expenses and income into the account as 0
    result = sqlite3_prepare_v2(db,select_sql,-1,&stmt, NULL); //Prepares the statement for execution
    if (result == SQLITE_OK){ //Checks if the preparation is successfull
        sqlite3_bind_int(stmt, 1, finance_id);
        if (sqlite3_step(stmt) == SQLITE_ROW){
            old_income = sqlite3_column_double(stmt,0);
        }
    }

    sqlite3_finalize(stmt);

    const char *select_sql2 = "SELECT COUNT (*) FROM old_finance WHERE finance_id = ?"; //Checks the count of query inside the table that will match the exact finance ID
    result = sqlite3_prepare_v2(db,select_sql2,-1,&stmt, NULL);
    if (result == SQLITE_OK){
        sqlite3_bind_int(stmt, 1, finance_id); //Binds the finance ID if the preparation
        if (sqlite3_step(stmt) ==SQLITE_ROW && sqlite3_column_int(stmt,0) > 0){ //This will return the data on the row as well as check if the finance ID exists or not
            sqlite3_finalize(stmt);
            const char *update_sql = "UPDATE old_finance SET old_income = ? WHERE finance_id = ?";
            result = sqlite3_prepare_v2(db, update_sql,-1,&stmt, NULL);
            if (result == SQLITE_OK){ // This save the current income into the SQL table
                sqlite3_bind_double(stmt, 1, old_income);
                sqlite3_bind_int(stmt, 2, finance_id);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
        } else {
            sqlite3_finalize(stmt);
            const char *archive_sql = "INSERT INTO old_finance (finance_id, old_income) VALUES (?, ?)"; //If the finance_id doesn't exist then it will insert into the table
            result = sqlite3_prepare_v2(db,archive_sql,-1,&stmt, NULL);
            if (result == SQLITE_OK){
                sqlite3_bind_int(stmt, 1, finance_id);
                sqlite3_bind_double(stmt, 2, old_income);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
        }
    }

    const char *update_sql2 = "UPDATE finance SET income = ? WHERE id = ?"; //Checks the pin and finance ID with the inputted username in user table
    result = sqlite3_prepare_v2(db,update_sql2,-1,&stmt, NULL);
    if (result == SQLITE_OK){//Checks if preparation was successful
        sqlite3_bind_double(stmt, 1, *income);
        sqlite3_bind_int(stmt, 2, finance_id);
        result = sqlite3_step(stmt);
        if (result != SQLITE_DONE){ //Catches specific errors of the execution does not work
            printf("Execution Fail: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Income been updated successfully\n");
        }
    } else {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    }


    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return SQLITE_OK;

}

//Updating expenses
int update_expenses(double *expenses, double old_expenses, int finance_id){
    //Declare variables
    sqlite3*db;
    sqlite3_stmt *stmt;
    int result;

    printf("Please input your expenses for this month: \n");
    scanf("%lf", expenses);

    result = sqlite3_open("database.db", &db); //Opens the database file
    if (result != SQLITE_OK){ //Check if opening was successful
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return result;
    }

    const char *select_sql = "SELECT expenses FROM finance WHERE id = ?"; //Insert the expenses and income into the account as 0
    result = sqlite3_prepare_v2(db,select_sql,-1,&stmt, NULL); //Prepares the statement for execution
    if (result == SQLITE_OK){ //Checks if the preparation is successfull
        sqlite3_bind_int(stmt, 1, finance_id);
        if (sqlite3_step(stmt) == SQLITE_ROW){
            old_expenses = sqlite3_column_double(stmt,0);
        }
    }

    sqlite3_finalize(stmt);


    const char *select_sql2 = "SELECT COUNT (*) FROM old_finance WHERE finance_id = ?"; //Checks for counts of finance ID
    result = sqlite3_prepare_v2(db,select_sql2,-1,&stmt, NULL);
    if (result == SQLITE_OK){
        sqlite3_bind_int(stmt, 1, finance_id);
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt,0) > 0){
            sqlite3_finalize(stmt);
            const char *update_sql = "UPDATE old_finance SET old_expenses = ? WHERE finance_id = ?";
            result = sqlite3_prepare_v2(db, update_sql,-1,&stmt, NULL);
            if (result == SQLITE_OK){ //Update the current expenses into the SQL table
                sqlite3_bind_double(stmt, 1, old_expenses);
                sqlite3_bind_int(stmt, 2, finance_id);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }

        } else {
            sqlite3_finalize(stmt);
            const char *archive_sql = "INSERT INTO old_finance (finance_id, old_expenses) VALUES (?, ?)";
            result = sqlite3_prepare_v2(db,archive_sql,-1,&stmt, NULL);
            if (result == SQLITE_OK){ // Insert finance ID and current expenses if the value is not found
                sqlite3_bind_int(stmt, 1, finance_id);
                sqlite3_bind_double(stmt, 2, old_expenses);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
        }
    }


    const char *update_sql2 = "UPDATE finance SET expenses = ? WHERE id = ?"; //Checks the pin and finance ID with the inputted username in user table
    result = sqlite3_prepare_v2(db,update_sql2,-1,&stmt, NULL);
    if (result == SQLITE_OK){//Checks if preparation was successful
        sqlite3_bind_double(stmt, 1, *expenses);
        sqlite3_bind_int(stmt, 2, finance_id);
        result = sqlite3_step(stmt);
        if (result != SQLITE_DONE){
            printf("Execution Fail: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Expenses been updated successfully\n");
        }
    } else {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return SQLITE_OK;
    }

//Logs the user in
int login(int *finance_id) {
    //Declare variables and allocating buffer(memory) sizes
    char username[50];
    char pin_str[50];
    char temp_pin_str[50];
    int pin;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int result;

    printf("What is your username:\n");
    scanf("%s", username);

    while(1){ //Loops until a proper 4 digit is entered
        printf("Please input your 4 digit pin:\n");
        scanf("%d", &pin);
        if (pin >= 0 && pin <= 9999){ //Checks if the pin is a  4 digit number
            sprintf(pin_str, "%d", pin);
            break;
        } else {
            printf ("Please provide a 4 digit password only\n");
        }
    }

    result = sqlite3_open("database.db", &db); //Opens the database file
    if (result != SQLITE_OK){ //Check if opening was successful
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return result;
    }

    const char *sql = "SELECT finance_id, pin_str FROM user WHERE username = ?"; //Checks the pin and finance ID with the inputted username in user table
    result = sqlite3_prepare_v2(db,sql,-1,&stmt, NULL);
    if (result != SQLITE_OK){//Checks if preparation was successful
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return result;
    }

     sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ROW){
        *finance_id = sqlite3_column_int(stmt,0);
        strcpy(temp_pin_str,(const char*)sqlite3_column_text(stmt,1));

        if(strcmp(pin_str,temp_pin_str)==0){//Compares the inputted pin with the database pin
            printf("Login is successful\n");
        } else{
            printf("Login was not sucesssful, password is incorrect\n");
            *finance_id = -1;
        }
    } else {
        printf("Username not in database\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return SQLITE_NOTFOUND;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (*finance_id != -1)? SQLITE_OK: SQLITE_ERROR; //Based on finance ID it will return either good or error

}

//This function was reference from a stack exchange forum
int make_user(int *finance_id){
    //Declares all the variables and assign memory.
    char first_name[50];
    char last_name[50];
    char username[50];
    char pin_str[50];
    char temp_username[50];
    int pin;
    sqlite3 *db;
    sqlite3_stmt *stmt;

    printf("Input your first name:\n");
    scanf("%s", first_name);
    printf("Input your last name:\n");
    scanf("%s", last_name);
    printf("Create a username:\n");
    scanf("%s",username);

    while(1){ //Loop through until 4 digit pin code is entered
        printf("Create a 4 digit pin:\n");
        scanf("%d", &pin);
        if (pin >= 0 && pin <= 9999){ //Checks if the digit is a 4 digit or not
            sprintf(pin_str, "%d", pin);
            break;
        } else {
            printf ("Please provide a 4 digit password only\n");
        }
    }

    int result = insert_finance(finance_id);
    if (result != SQLITE_OK) {//Checks if finance ID can be made
        printf("ERROR: Cannot create finance ID: %d\n", result);
        return result;
    }

    result = sqlite3_open("database.db", &db);
    if (result != SQLITE_OK){//Check if database can be open
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return result;
    }

    const char *sql = "SELECT COUNT(*) FROM user WHERE username = ?";
    result = sqlite3_prepare_v2(db,sql,-1,&stmt, NULL);
    if (result != SQLITE_OK){ //Check if the statement can be prepared for execution
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return result;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ROW){ //Checks the rows
        int count = sqlite3_column_int(stmt,0); //Returns the value of that column
        if(count > 0){
            printf("That username is taken\n");
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return SQLITE_CONSTRAINT; //Return in case of error;
    }

    sqlite3_finalize(stmt);

    result = insert_user(first_name, last_name, username, pin_str, *finance_id);
    if (result == SQLITE_OK){ //Check if the user is created
        printf("User created successfully\n");
    } else {
        printf("Error creating user: %d\n", result);
    }

    sqlite3_close(db);

    return result;
}
}
void displayMenu(){
    printf("\n=======================Finance Tracker=======================\n");
    printf("|                       1.Register                          |\n");
    printf("|                       2.Login                             |\n");
    printf("|                       3.Exit                              |\n");
    printf ("=============================================================\n");
    printf("Please choose an option:\n");
}

void displayFinance(){
    printf("\n==============================Options==============================\n");
    printf("|                       1.Update Expenses                         |\n");
    printf("|                       2.Update Income                           |\n");
    printf("|                       3.Statistics                              |\n");
    printf("|                       4.Log Out                                 |\n");
    printf ("===================================================================\n");
    printf("Please choose an option:\n");
}

