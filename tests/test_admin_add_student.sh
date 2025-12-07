#!/bin/bash

echo "=== TEST: Admin adds a new student and views list ==="

# Assumes:
# - Admin username: admin1
# - Admin password: admin1pwd
# Flow:
# 1  -> Admin Login
# admin1    -> username
# admin1pwd -> password
# 1  -> Add Student
# newstud1  -> new student username
# newpass1  -> new student password
# 2  -> View Student Details
# 9  -> Logout and Exit (admin menu)
# 4  -> Exit Application (main menu)

printf "1\nadmin1\nadmin1pwd\n1\nnewstud1\nnewpass1\n2\n9\n4\n" | ./client/client
