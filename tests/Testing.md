# Testing Documentation – Academia Portal

This document describes the testing strategy and test cases for the Academia Portal project.

---

## 1. Project Overview

- Language: C  
- Architecture: TCP client–server  
- Roles: Admin, Faculty, Student  
- Data stored in: `users.dat`, `courses.dat`, `enrollments.dat`  

Testing verifies:
- Login correctness  
- Menu operations  
- File updates  
- Edge cases  
- Concurrency behaviour (multiple clients)

---

## 2. Types of Testing Used

### 2.1 White-box Testing (Structural)

White-box testing uses internal knowledge of the code.  
Applied to functions such as:

- `enroll_course()`
- `unenroll_course()`
- `set_student_status()`
- `change_password()`

We target:
- Statement coverage  
- Branch coverage  
- Basic path coverage  

### 2.2 Black-box Testing (Specification-based)

Black-box testing does NOT look at the code.  
It only checks behaviour through menus, inputs, outputs.

Examples:
- Admin adds a user  
- Student enrolls in a course  
- Faculty adds a course  
- Wrong password → error  
- Invalid option → “Invalid choice”  

---

## 3. Levels of Testing

### 3.1 Unit Testing
Tests individual functions.

Examples:
- `enroll_course()` → success / already enrolled / course full / invalid ID  
- `set_student_status()` → activate / block / invalid ID / cancel  
- `unenroll_course()` → valid / invalid / none enrolled  

### 3.2 Module Testing
Tests groups of related functions.

**Admin Module:**  
add, view, block/activate, modify users  

**Student Module:**  
view courses, enroll, unenroll, view enrolled, change password  

**Faculty Module:**  
add, view, update, remove courses, change password  

### 3.3 Integration Testing
Tests interaction between modules.

Examples:
- Admin adds student → student logs in  
- Faculty adds course → student sees and enrolls  
- Two students enroll same course → seat count stays consistent  

### 3.4 System Testing
Tests the whole system as a black box.

Examples:
- Login → menu → action → logout  
- Invalid login  
- Exit application  

---

## 4. Example Unit Test Cases (White-box)

### 4.1 Function: `enroll_course()`

**EC-1: Successful Enrollment**  
- Course exists, seats available, student not enrolled  
- Expect: “Enrollment successful”, seat count updated  

**EC-2: Already Enrolled**  
- Student already enrolled  
- Expect: “Already enrolled”, no change  

**EC-3: Course Full**  
- max_seats reached  
- Expect: “Course is full”  

**EC-4: Invalid Course ID**  
- ID does not exist  
- Expect: “Course ID not found”  

**EC-5: File Error**  
- courses.dat missing  
- Expect: “Error opening course file.”  

---

## 5. Example System Test Cases (Black-box)

| TC   | Role    | Scenario               | Input Sequence                               | Expected Output                 |
|------|---------|------------------------|-----------------------------------------------|---------------------------------|
| ST-1 | Admin   | Valid login/logout     | Choose Admin → correct creds → Logout         | Admin menu works; logout shown |
| ST-2 | Student | Invalid login          | Choose Student → wrong password               | "Invalid credentials"          |
| ST-3 | Student | Enroll in a course     | Login → View → Enroll → enter valid ID        | "Enrollment successful"        |
| ST-4 | Student | Unenroll from course   | Login → Unenroll → enter valid ID             | "Successfully unenrolled"      |
| ST-5 | Faculty | Add new course         | Login → Add Course → enter name & seats       | Course appears in list         |
| ST-6 | Any     | Invalid menu choice    | Enter 99                                      | "Invalid choice"               |
| ST-7 | All     | Exit application       | Choose option 4                                | "Goodbye"                      |
                 
---

## 6. How to Run the Demo Test Script

From the project root:

./tests/run_demo_test.sh


This script:
- builds the project  
- initializes data  
- starts the server  
- runs the client once  
- stops the server  

More tests can be performed manually during the demo.