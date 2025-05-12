#!/bin/bash
# Script that shares the public key of this computer to all the listed nodes of the cluster
# Usage: ./script.sh hostfile password

# Check for correct number of arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 hostfile password"
    exit 1
fi

HOSTFILE="$1"
PASSWORD="$2"

# Validate input file
if [ ! -f "$HOSTFILE" ]; then
    echo "The file $HOSTFILE does not exist."
    exit 1
fi

# Check if sshpass is installed
if ! command -v sshpass &> /dev/null; then
    echo "sshpass is not installed. Install it with: sudo apt install sshpass"
    exit 1
fi

# Loop through each line
while IFS= read -r line || [ -n "$line" ]; do
    # Skip empty lines
    if [ -z "$line" ]; then
        continue
    fi

    # Extract the IP address (first field)
    ip=$(echo "$line" | awk '{print $1}')

    echo "Sharing public key to mpi-user@$ip"
    sshpass -p "$PASSWORD" -sshcopy-id -o StrictHostKeyChecking=no "mpi-user@$ip"

    if [ "$?" -ne 0 ]; then
        echo "Error copying to mpi-user@$ip"
    else
        echo "Successfully copied to mpi-user@$ip"
    fi
done < "$HOSTFILE"
