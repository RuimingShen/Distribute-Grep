#!/bin/bash

USER="gloh2"

# Start the servers on all vm
for i in $(seq 1 10); do
    # Construct the full host name with zero-padding
    HOST=$(printf "fa24-cs425-19%02d.cs.illinois.edu" "$i")
    
    # Run the command on the remote host
    ssh "${USER}@${HOST}" "nohup /cs425/mp1/release/grep_server /cs425/mp1/log/vm${i}.log > /cs425/mp1/log/grep_server.log 2>&1 &"

    echo "Started server on ${HOST}."
done