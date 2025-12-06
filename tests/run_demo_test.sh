#!/bin/bash

echo "=== Building project ==="
make

if [ -x "./init/init_data" ]; then
    echo "=== Initializing data ==="
    ./init/init_data
fi

echo "=== Starting server ==="
./server/server &
SERVER_PID=$!

sleep 1

echo "=== Running demo client (auto exit) ==="
printf "4\n" | ./client/client

echo "=== Stopping server ==="
kill "$SERVER_PID" 2>/dev/null || true

echo "=== Demo test completed ==="
