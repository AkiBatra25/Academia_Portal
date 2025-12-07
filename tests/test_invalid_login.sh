#!/bin/bash

echo "=== TEST: Invalid Student Login ==="

# 3 → Student Login
# wronguser → invalid username
# wrongpass → invalid password
# 4 → Exit Application

printf "3\nwronguser\nwrongpass\n4\n" | ./client/client
