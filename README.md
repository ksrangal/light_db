# light_db

`light_db` is a lightweight, transactional in-memory key-value store implemented in C++. It provides persistence through a Write-Ahead Log (WAL), ensuring data durability. The database supports basic CRUD operations (`set`, `get`, `remove`) and includes support for nested transactions with `commit` and `rollback` capabilities.

## Features

*   **In-Memory Caching:** All key-value pairs are held in memory for fast read/write access.
*   **Write-Ahead Log (WAL) Persistence:** All data modifications (add, update, delete) are appended to a log file on disk before being applied, ensuring durability across sessions.
*   **Transactional Integrity:** Supports atomic transactions with `begin`, `commit`, and `rollback` operations.
*   **Nested Transactions:** Allows for starting new transactions within an existing one, providing fine-grained control over data changes.
*   **Cache Warm-up:** On startup, the database automatically rebuilds its in-memory state by replaying the WAL file.

## Architecture

The system is composed of three main components:

*   **`db` (db.hxx, db.cpp):** This is the core component that provides the user-facing API. It manages the primary in-memory cache (`std::unordered_map`) and uses a stack of transactions to handle nested operations. It coordinates between the in-memory state and the persistent store.
*   **`Store` (store.hxx, store.cpp):** This class handles all interactions with the filesystem. It is responsible for appending serialized log entries to the WAL file and for reading the log during the warm-up phase to populate the in-memory cache.
*   **`WalEntry` (wal.hxx, wal.cpp):** This class defines the structure of a single record in the Write-Ahead Log. Each entry contains the operation type (`ADD`, `UPDATE`, `DELETE`), a timestamp, the key, and the value. It includes methods for serializing an entry for disk storage and deserializing it back into an object.

## API and Usage

The main `db` class provides a simple interface for interacting with the database.

### API Methods

*   `db(std::string store_path)`: Initializes the database, opening the WAL file at `store_path` and warming up the cache from its contents.
*   `void set(const std::string& key, const std::string& value)`: Creates or updates a key-value pair. If inside a transaction, the change is staged until commit.
*   `std::optional<std::string> get(const std::string& key)`: Retrieves the value associated with a key from the committed, in-memory store. Note: This method does not see uncommitted changes inside a transaction.
*   `void remove(std::string key)`: Marks a key for deletion.
*   `void begin()`: Pushes a new transaction context onto the stack.
*   `void commit()`: Merges the changes from the current transaction into its parent transaction or into the main store if it's the outermost transaction. Changes to the main store are written to the WAL.
*   `void rollback()`: Discards all changes made in the current transaction.

### Example

```cpp
#include "db.hxx"
#include <iostream>

void demonstrate_transactions() {
    // Initialize DB, loading from or creating a file at /tmp/my_database.wal
    DB::db db("/tmp/my_database.wal");

    db.set("user:1", "Alice");
    db.print("user:1"); // Output: user:1=Alice

    // Start a transaction
    db.begin();
    db.set("user:1", "Bob");

    // 'get' reads from the main cache, so it won't see the uncommitted change
    std::cout << "Value inside transaction (before commit): ";
    db.print("user:1"); // Output: Value inside transaction (before commit): user:1=Alice

    // Commit the transaction to make the change permanent
    db.commit();
    std::cout << "Value after commit: ";
    db.print("user:1"); // Output: Value after commit: user:1=Bob

    // Start a transaction and then roll it back
    db.begin();
    db.set("user:1", "Charlie");
    db.rollback();

    std::cout << "Value after rollback: ";
    db.print("user:1"); // Output: Value after rollback: user:1=Bob (change was discarded)
}
```

## How to Build and Run

### Prerequisites
- A C++ compiler that supports C++17 (e.g., g++, clang++).

### Compilation
Navigate to the root directory of the repository and compile the source files.

```bash
g++ -std=c++17 -o main src/*.cpp
```

### Running the Example
After successful compilation, an executable named `main` will be created. Run it to execute the test cases defined in `src/app.cpp`:

```bash
./main
```

The program will perform a series of set, update, and transactional operations, printing the results to the console. It will also create a `db.wal` file in the `/tmp` directory to demonstrate persistence.
