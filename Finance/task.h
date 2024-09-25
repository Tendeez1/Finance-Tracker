#include <sqlite3.h>

extern double income;
extern double expenses;
extern double old_income;
extern double old_expenses;

int insert_user(const char *first_name, const char *last_name, const char *username, const char *pin_str, int finance_id);
int insert_finance(int *finance_id);
int statistics(int finance_id);
int update_income(double *income, double old_income, int finance_id);
int update_expenses(double *expenses, double old_expenses, int finance_id);
int login(int *finance_id);
int make_user(int *finance_id);
void displayMenu();
void displayFinance();
