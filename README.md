# Mini Database Engine (C++)

A lightweight file-based database engine built in C++ that supports basic query operations using CSV storage.

## Features

* INSERT records
* SELECT with conditions (index-accelerated)
* DELETE with filtering
* UPDATE records
* CREATE INDEX on any attribute
* Binary index persistence (automatically saves/loads indexes)
* CSV-based persistent storage
* Modular architecture (parser, storage, execution)

## Example Usage

```
INSERT id=1 name=Rishi age=20
SELECT *
SELECT id=1
UPDATE id=1 age=21
DELETE id=1
INDEX id
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

* SQL-like syntax (SELECT with WHERE/JOIN clauses)
* Transactions
* Multiple indexes per column
* Query optimization
* Memory-based caching
