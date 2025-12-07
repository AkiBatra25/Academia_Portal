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
gcc -Iinclude -o server/server server/server.c server/common.c
```

### 4.2 Build the Client
```bash
gcc -Iinclude -o client/client client/client.c
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


7.1 Unit Testing (White-box)
A C unit test (tests/test_authenticate.c) was created for authenticate().

Tested:
Valid Admin credentials

Invalid Admin password

Wrong role (admin as student)

Valid Student login

Coverage Achieved:
Branch Coverage

All true/false branches of key conditions executed.

Path Coverage

Successful login path

Wrong password path

Wrong role path

7.2 Module Testing
Tested modules include:

Admin module:
Add student

View students

Add faculty

View faculty

Student module:
View courses

Enroll

Unenroll

View enrolled courses

Faculty module:
Add course

View courses

Update/remove course (manually tested)

7.3 Integration Testing
Examples:

Admin adds a student → student can log in

Faculty adds a course → student can view and enroll

Enrollment correctly updates both courses.dat & enrollments.dat

7.4 System Testing (Black-box)
End-to-end workflows tested:

Admin login → operations → logout

Student login → enroll/unenroll → logout

Faculty login → course operations

Invalid input handling

Exit application gracefully

7.5 Automated Testing
The /tests folder includes:

Shell scripts that simulate client input

Unit tests for core functions

Semi-automated flows for Admin and Student modules

8. Full Testing Report
All detailed test cases, tables, diagrams, and coverage explanations are available in:

👉 Testing.md

This file is the complete SE Deliverable 3 testing report.


