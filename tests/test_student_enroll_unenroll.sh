#!/bin/bash

echo "=== TEST: Student Enroll + Unenroll ==="

# This script assumes there is a valid student:
# username: student1
# password: pass1
# and at least one course with ID 101 (or any valid ID).

# Flow:
# 3  -> Student Login
# student1 -> username
# pass1    -> password
# 1  -> View All Courses
# 2  -> Enroll in Course
# 101 -> Course ID to enroll (change if needed)
# 4  -> View Enrolled Courses
# 3  -> Unenroll from Course
# 101 -> Same Course ID
# 6  -> Logout and Exit

printf "3\nstudent1\npass1\n1\n2\n101\n4\n3\n101\n6\n" | ./client/client
