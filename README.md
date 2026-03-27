# Mini Database Engine (C++)

A lightweight file-based database engine built in C++ that supports basic query operations using CSV storage.

## Features

* INSERT records
* SELECT with conditions
* DELETE with filtering
* CSV-based persistent storage
* Modular architecture (parser, storage, execution)

## Example Usage

```
INSERT id=1 name=Rishi age=20
SELECT *
DELETE id=1
```

## Tech Stack

* C++
* STL (map, vector, filesystem)
* File I/O

## How to Run

```
g++ src/*.cpp -Iinclude -o db
./db
```

## Future Improvements

* UPDATE queries
* Indexing
* SQL-like syntax
* Transactions
