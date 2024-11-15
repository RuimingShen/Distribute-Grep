#!/bin/bash

# set -x  # Enable debugging

# Copy the binaries to every vm in /cs425/mp1/release/ 
# (make sure that the directories already exist in all vms before running this script)
for i in $(seq -w 1 10); do
    BIN_PATH="./build/bin"
    CLIENT_BIN="query_client"
    SERVER_BIN="grep_server"
    HOST="fa24-cs425-19${i}.cs.illinois.edu"
    
    # Copy to a writable path
    scp "$BIN_PATH/$CLIENT_BIN" "$BIN_PATH/$SERVER_BIN" "gloh2@$HOST:/cs425/mp1/release/" 
done
