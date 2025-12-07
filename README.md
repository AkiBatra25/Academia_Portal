# Academia Portal – Client–Server Course Registration System (OS Project)

This project implements a multi-threaded TCP client–server system for an academic course registration portal.  
It was developed as part of **Operating Systems Lab** and **Software Engineering** coursework.

---

# 1. Project Overview

The Academia Portal supports three types of users:

- **Admin** – manages students and faculty accounts  
- **Faculty** – manages courses  
- **Student** – registers and deregisters for courses  

The system is implemented in **C** and uses:

- **TCP sockets** for client–server communication  
- **pthreads** for concurrency (one thread per client)  
- **Binary files** for persistent storage  
- **File locking (fcntl)** for safe concurrent access  

---

# 2. Features

## Admin Features
- Add / View Students  
- Add / View Faculty  
- Activate / Block Student  
- Modify Student details  
- Modify Faculty details  
- Logout  

## Faculty Features
- View Offering Courses  
- Add New Course  
- Remove Course  
- Update Course Details  
- Change Password  
- Logout  

## Student Features
- View All Courses  
- Enroll in Course  
- Unenroll from Course  
- View Enrolled Courses  
- Change Password  
- Logout  

---

# 3. System Architecture

## Client
- Terminal-based UI  
- Reads user inputs  
- Sends commands to server  
- Displays server responses  

## Server
- TCP server on port **8080**  
- Spawns **one pthread per client**  
- Performs authentication, course operations, user operations  
- Uses **file locking** for safe concurrent access  

## Data Storage  
Binary files stored in `/server/`:

- `users.dat`  
- `courses.dat`  
- `enrollments.dat`  

---

# 4. How to Compile and Run

From the project root directory:

### 4.1 Build the Server
```bash
gcc -include -o Server/server Server/server.c Server/common.c
```

### 4.2 Build the Client
```bash
gcc -Iinclude -o Client/client Client/client.c
```

### 4.3 (Optional) Initialize Sample Data
This will create sample admin, faculty, student, and course entries.
```bash
gcc -Iinclude -o init/init_data init/init_data.c
./init/init_data
```

### 4.4 Run the Server
```bash
./server/server
```
You should see:
```
Server listening on port 8080...
```

### 4.5 Run the Client (in another terminal)
```bash
./client/client
```
You will see:
```
1. Admin Login
2. Faculty Login
3. Student Login
4. Exit Application
```
### 4.6 Run Automated & Manual Tests

Testing for the Academia Portal includes automated shell scripts, a C unit test, and manual feature tests.

#### 4.6.1 Run Demo Test Script (End-to-End System Test)

This script builds the system, initializes sample data, launches the server, and performs a demo client interaction.
```bash
chmod +x tests/run_demo_test.sh
./tests/run_demo_test.sh
```
#### 4.6.2 Running Black-Box Test Scripts

Before running any script, start the server in Terminal 1:
```bash
./Server/server

```
You should see:

Server listening on port 8080...


Now open Terminal 2 and run any of the following tests.

(a) Invalid Login Test
```bash
chmod +x tests/test_invalid_login.sh
./tests/test_invalid_login.sh
```

Expected:

Invalid credentials or role

(b) Student Enrollment + Unenrollment Test

```bash
chmod +x tests/test_student_enroll_unenroll.sh
./tests/test_student_enroll_unenroll.sh
```

This test automatically performs:

login → view courses → enroll → view enrolled → unenroll → logout

(c) Admin Add Student Test

```bash
chmod +x tests/test_admin_add_student.sh
./tests/test_admin_add_student.sh

```
Expected:

Student added successfully

Updated student list displayed

4.6.3 Run the Unit Test (White-Box Test on authenticate())

Compile the unit test:
```bash

gcc -Iinclude -o tests/test_authenticate tests/test_authenticate.c Server/common.c
```

Run it:
```bash
./tests/test_authenticate
```

The test prints PASS/FAIL for:

Valid Admin login

Wrong password

Wrong role

Valid Student login

#### 4.6.4 Manual Testing (Remaining Features)

Some menus require human input.

Start the server:
```bash
./Server/server

```
Start the client in another terminal:
```bash
./Client/client

```
Manually test features such as:

Modify Student / Faculty Details

Activate / Block Student

Add / Update / Remove Course

Verify that:

Success/error messages appear correctly

users.dat, courses.dat, and enrollments.dat are updated

# 5. Folder Structure

```
Academia_Portal/
│
├── client/               # Client-side C program (menus, socket communication)
├── server/               # Server-side code (authentication, menus, logic)
├── include/              # Header files (structures, prototypes)
├── init/                 # Initialization tool for creating sample data
├── tests/                # Automated scripts + unit tests (SE Deliverable 3)
│
├── Testing.md            # Full Software Engineering testing report
└── README.md             # Main project documentation
```

# 6. Operating Systems Concepts Used

This project demonstrates multiple OS principles learned in the Operating Systems lab:

### 6.1 Multi-Threading (pthreads)
- The server creates **one thread per client** using `pthread_create()`.
- Each client connection is handled independently.
- Demonstrates concurrency and thread-based parallelism.

### 6.2 Inter-Process Communication (TCP Sockets)
- Client and server communicate using:
  - `socket()`
  - `bind()`
  - `listen()`
  - `accept()`
  - `connect()`
  - `read()` / `write()`
- Shows real-world network programming and IPC mechanisms.

### 6.3 File Management & Persistence
- All users, courses, and enrollments are stored as **binary files**.
- Demonstrates OS-level file handling:
  - `fopen()`, `fread()`, `fwrite()`, `fseek()`, `ftell()`

### 6.4 File Locking (fcntl)
- Used to prevent race conditions when multiple clients update the same file.
- Achieved using:
  - `fcntl(fd, F_SETLKW, &lock)`
- Ensures consistency of course seat counts and user records.

### 6.5 Synchronization & Race Condition Prevention
- Critical file operations (enroll, unenroll, update course, modify user) use write locks.
- Prevents corrupted data from concurrent access.

### 6.6 Server–Client Architecture
- Continuous server loop handles multiple simultaneous connections.
- Demonstrates concepts of:
  - Blocking/non-blocking I/O  
  - Process lifecycle  
  - Resource sharing  


# 7. Software Engineering – Testing Summary

Testing was performed at multiple levels according to Software Engineering principles.  
Both **white-box** and **black-box** testing methods were used, along with **branch coverage** and **path coverage** for key logic.

---

### 7.1 Unit Testing (White-box)

A unit test (`tests/test_authenticate.c`) was created for the `authenticate()` function.

#### Tests include:
- Valid Admin login  
- Invalid password  
- Wrong role (admin as student)  
- Valid Student login  

#### Coverage Achieved:
- **Branch Coverage:**  
  All true/false outcomes of important `if` conditions were executed.

- **Path Coverage:**  
  Major execution paths were covered:  
  - Successful login  
  - Wrong password  
  - Wrong role  
  - User not found  

---

### 7.2 Module Testing

Each functional module was tested independently:

#### Admin Module
- Add Student  
- Add Faculty  
- View Students / View Faculty  
- Modify Student / Faculty  
- Block / Activate Student  

#### Student Module
- View All Courses  
- Enroll in Course  
- Unenroll from Course  
- View Enrolled Courses  

#### Faculty Module
- Add Course  
- Remove Course  
- Update Course Details  
- View Offering Courses  

Some tests are automated, others manually validated.

---

### 7.3 Integration Testing

Scenarios tested:

- Admin adds a student → student can immediately log in.  
- Faculty adds a course → student can view and enroll in it.  
- Student enrollment updates:
  - `enrollments.dat`  
  - `courses.dat` seat count  

- Unenrollment removes the record and reduces seat count properly.

---

### 7.4 System Testing (Black-box)

End-to-end tests covering complete workflows:

- Admin login → perform operations → logout  
- Student login → enroll/unenroll/view courses → logout  
- Faculty login → add/update/remove courses → logout  
- Invalid input handling  
- Graceful exit from application  

These tests verify the entire client–server system works correctly.

---

### 7.5 Automated Testing Scripts

Located in `/tests/`:

- **test_invalid_login.sh** – Incorrect credentials test  
- **test_student_enroll_unenroll.sh** – Student full workflow  
- **test_admin_add_student.sh** – Admin adds and views a student  
- **test_authenticate** – C unit test (white-box)

Automated tests act as black-box simulations of real user input.

---

### 7.6 Summary of Test Coverage

| Testing Type | Status |
|--------------|---------|
| Unit Testing | ✔ Done |
| Module Testing | ✔ Done |
| Integration Testing | ✔ Done |
| System Testing | ✔ Done |
| Black-box Testing | ✔ Done |
| White-box Testing | ✔ Done |
| Branch Coverage | ✔ Achieved |
| Path Coverage | ✔ Achieved |

Detailed test cases and outputs are included in **Testing.md**.



# 8. Full Testing Report
All detailed test cases, tables, diagrams, and coverage explanations are available in:

👉 Testing.md

This file is the complete SE Deliverable 3 testing report.






