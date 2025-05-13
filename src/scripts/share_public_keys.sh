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

# Ensure ~/.ssh exists and permissions are correct
mkdir -p ~/.ssh
chmod 700 ~/.ssh

# Generate SSH key if it doesn't exist
if [ ! -f ~/.ssh/id_rsa ]; then
    echo "Generating SSH key..."
    ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa
fi

# Loop through each line
while IFS= read -r line || [ -n "$line" ]; do
    # Skip empty lines
    if [ -z "$line" ]; then
        continue
    fi

    # Extract the IP address (first field)
    ip=$(echo "$line" | awk '{print $1}')
    echo "==> Processing $ip"

    # Add to known_hosts to avoid host verification prompt
    ssh-keyscan -H "$ip" >> ~/.ssh/known_hosts 2>/dev/null

    # Copy SSH key
    echo "==> Sharing public key to mpi-user@$ip"
    sshpass -p "$PASSWORD" ssh-copy-id -o StrictHostKeyChecking=no "mpi-user@$ip"

    if [ "$?" -ne 0 ]; then
        echo "❌ Error copying to mpi-user@$ip"
    else
        echo "✅ Successfully copied to mpi-user@$ip"
    fi
done < "$HOSTFILE"
