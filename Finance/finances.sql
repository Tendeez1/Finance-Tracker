CREATE TABLE user (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    first_name TEXT,
    last_name TEXT,
    username TEXT,
    pin_str TEXT,
    finance_id INTEGER UNIQUE,
    FOREIGN KEY (finance_id)
        REFERENCES finance(id)
);

CREATE TABLE finance (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    expenses REAL DEFAULT 0,
    income REAL DEFAULT 0
);

CREATE TABLE old_finance (
    id INTEGER PRIMARY KEY,
    finance_id INTEGER,
    old_income REAL,
    old_expenses REAL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(finance_id)
        REFERENCES finance(id)
);
