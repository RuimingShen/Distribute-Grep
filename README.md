## Distributed Grep System
### Build Instructions

#### Install **boost**
You will need to install **boost** library first before building the code. To install **boost**, run the following on the linux terminal:

```
sudo apt-get update
sudo apt install libboost-all-dev
```

#### Build
There is a `build.sh` shell script available to simplify the building process. Simply run the following to build the executables:

```
./build.sh
```

#### Executables
The executables will be located in `./build/bin`, including:
- `grep_server`: The program that receives grep command from `query_client` and execute them on the local log files.
- `query_client`: The program that receives grep command from the user and relay it to multiple `grep_server`.
