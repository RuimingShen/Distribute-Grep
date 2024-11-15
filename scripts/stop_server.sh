#!/bin/bash

USER="gloh2"

for i in $(seq 1 10); do
    # Construct the full host name with zero-padding
    HOST=$(printf "fa24-cs425-19%02d.cs.illinois.edu" "$i")
    
    # Run the command on the remote host
    ssh "${USER}@${HOST}" "pkill -f /cs425/mp1/release/grep_server"

    echo "Attempted to stop the server on ${HOST}."
done