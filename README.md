# Mini Database Engine (C++)

A lightweight file-based database engine built in C++ that stores records in CSV format and supports simple CRUD-style commands through a REPL.

## Features

- Insert records into the database
- Select records by field/value conditions
- Update existing records
- Delete records by matching conditions
- Create indexes on attributes for faster lookups using B-trees
- Persist indexes to disk between runs
- Modular architecture split across parser, storage, and engine layers

## Example Usage

```text
insert id=1 name=Rishi age=20
select id=1
update id=1 age=21
delete id=1
index age
exit
```

## Tech Stack

- C++17
- CMake
- STL containers such as map, vector, unordered_map, and filesystem
- CSV file storage with binary index persistence

## How to Build and Run

### Linux / macOS

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./mini_db
```

### Windows (PowerShell)

```powershell
mkdir build
cd build
cmake ..
cmake --build .
./mini_db.exe
```

## Project Structure

- [src](src) — entry point, parser, engine logic, and storage implementation
- [include](include) — shared data structures and public headers
- [data](data) — CSV database file and persisted index data
- [build](build) — generated CMake build artifacts

## Future Improvements

- SQL-like syntax with WHERE/JOIN support
- Transactions and rollback
- Multi-column or composite indexes
- Query planner and optimization
- In-memory caching
