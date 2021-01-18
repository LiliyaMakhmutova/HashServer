# Hash server &amp; client (for integration tests)

This asynchronous hash server with thread pool can store data by key with certain expiration time. The requests are addtable, remtable, gettable, setval, getval.

## Getting started

Open \*.sln file and compile (build in release x64) the projects (client and server).

### Prerequisites

- x64 machine
- boost 1.72.0 or above
- MS Visual Studio 2019 or above (with C++17 standard compiler)

### Server configuration (and how to run .exe file)


| -d --dir=<path>      | Path to the directory where files will be stored (future)                                                   |
| -i --ip=<IP>         | IP address of server listener                                                                               |
| -p --port=<uint>     | Port of server listener                                                                                     |
| -m --maxtblsz=<uint> | Max size of hash table (records)                                                                            |
| -n --ntables=<uint>  | Max number of available hash tables                                                                         |
| -w --workers=<uint>  | Number of threads                                                                                           |
| -v --verbose         | Flag that indicates that debug messages is printed to stdout (stderr), if not set server prints only errors |
| -h --help            | Print help string                                                                                           |

Example of running the server on Windows:

HashServer.exe -d &quot;path/to/folder&quot; -i &quot;127.0.0.1&quot; -p 1234 -m 100

HashServer.exe -h

or with long options:

HashServer.exe --dir=&quot;path/to/folder&quot; --ip= &quot;127.0.0.1&quot; --port=1234 --maxtblsz=100

HashServer.exe --help

### Server commands

The server accepts the following commands and outputs (each command starts with username to identify user):

| **command** | **description** | **output** |
| --- | --- | --- |
| addtable | user creates new hash table | Number of newly-created hash table |
| remtable \&lt;no\&gt; | user deletes hash table by its number, only table owner is allowed to do it | Nothing (empty string) if succeeds or error string otherwise |
| **gettable**  **\&lt;**** no ****\&gt;** | get full copy of a table by its number, only table owner is allowed to do it | &quot;key:value&quot; string if succeeds or error string otherwise |
| **setval key=\&lt;uint\&gt; val=\&lt;string\&gt; table=\&lt;no\&gt; ttl=\&lt;sec\&gt;** | sets value by key in a table with expiration time, all users allowed | Nothing (empty string) if succeeds or error string otherwise |
| **getval key=\&lt;uint\&gt; table=\&lt;no\&gt;** | gets value by key in table | &quot;ok key=key value=value table=table&quot; string if succeeds or error string otherwise |

Example of command:

Request: JohnDoe addtable

Response: 0

Request: JohnDoe remtable 0

Response: &quot;&quot;

## Running the tests

### Unit tests

Open UnitTestHashMap project inside HashServer solution, build Microsoft Unit Tests and run them (click Run in test explorer).

### Integration tests

Open HashClient project .sln, build (release, x64) and run it.

## Acknowledgements

Boost library open source community.

## License

This project is licensed under the MIT License

## Author

Liliya Makhmutova, liliyamakhmutova17@gmail.com 
