## Distributed Grep System
### Build Instructions

#### Install **boost**
You will need to install **boost** and **cmake** library first before building the code. To install all the required packages, run the following on the linux terminal:

```
./scripts/install_package.sh
```

#### Build
There is a `./scripts/build.sh` shell script available to simplify the building process. Simply run the following to build the executables:

```
./scripts/build.sh
```

#### Executables
The executables will be located in `./build/bin`, including:
- `grep_server`: The program that receives grep command from `query_client` and execute them on the local log files.
- `query_client`: The program that receives grep command from the user and relay it to multiple `grep_server`.

### Run Instructions
- Run server using: `grep_server <log_file>`, where **log_file** is the file that the server will grep on.
- Run client using: `query_client`, it will then prompt for a grep command.

### Scripts
There are a few scripts located in `scripts` directory:
- `build.sh`: Build all the executables.
- `deploy.sh`: Copy the executables to all vms.
- `install_package.sh`: Install the necessary package to compile the code (Run once)
- `start_server.sh`: Run `grep_server vm{i}.log` in all VMs.
- `stop_server.sh`: Stop `grep_server` running in all VMs.