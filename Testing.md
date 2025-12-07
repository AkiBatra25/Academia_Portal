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

---

---

## 7. Automated Test Execution (Scripts and C Unit Test)

Apart from manual execution of test cases, we added small automation to actually run some tests:

- `tests/test_invalid_login.sh`  
  - Black-box test.  
  - Automatically runs the client, chooses Student Login, enters a wrong username/password and exits.  
  - Verifies that the system rejects the login and shows the “Invalid credentials or role” message.

- `tests/test_student_enroll_unenroll.sh`  
  - Black-box module + system test for the Student role.  
  - Logs in as a valid student (`student1 / pass1`), views all courses, enrolls into course ID 101, views enrolled courses, then unenrolls from the same course and logs out.  
  - Demonstrates that the Student module functions (enroll + unenroll) work end-to-end.

- `tests/test_authenticate.c` → compiled as `tests/test_authenticate`  
  - White-box **unit test** for the `authenticate()` function in `Server/common.c`.  
  - Test 1: valid admin credentials (`admin1 / admin1pwd`, role `admin`) → must PASS.  
  - Test 2: wrong password → must be rejected.  
  - Test 3: correct credentials but wrong role → must be rejected.  
  - Test 4: valid student credentials (`student1 / pass1`, role `student`) → must PASS.  
  - This shows how we designed tests to cover different branches (valid/invalid credentials, correct/wrong role) for a single function.


---

## 8. Manual Test Cases for Remaining Features

The project instructions say that the main features must be tested thoroughly, and side features may be tested to a lesser extent.  
We automated the core flows (login, admin add student, student enroll/unenroll, authentication), and we tested the remaining options manually as described below.

### 8.1 Admin Menu – Manual Test Cases

**A1 – Add Faculty**

- **Precondition:** Logged in as `admin1 / admin1pwd`.
- **Steps:**
  1. Choose option `3` (Add Faculty).
  2. Enter a new faculty username (e.g., `faculty1`).
  3. Enter a password (e.g., `facpass1`).
  4. Choose option `4` (View Faculty Details).
- **Expected:**
  - “faculty added with ID X” message is displayed.
  - `faculty1` appears in the faculty list with status “Active”.

**A2 – View Faculty Details**

- **Precondition:** At least one faculty exists.
- **Steps:**  
  1. From Admin Menu, choose option `4` (View Faculty Details).
- **Expected:**  
  - A list of all faculty with their IDs, usernames and status.

**A3 – Activate Student**

- **Precondition:** At least one blocked student exists.
- **Steps:**
  1. Choose option `5` (Activate Student).
  2. Enter the ID of a blocked student.
  3. View Student Details (option `2`) again.
- **Expected:**  
  - Status of that student changes to “Active”.
  - Any error message is shown if the ID is invalid.

**A4 – Block Student**

- **Precondition:** At least one active student exists.
- **Steps:**
  1. Choose option `6` (Block Student).
  2. Enter a valid student ID.
  3. View Student Details (option `2`) again.
- **Expected:**  
  - Status of that student changes to “Blocked”.
  - If you try to log in with that student, login is rejected.

**A5 – Modify Student Details**

- **Precondition:** A student with known ID exists and is Active.
- **Steps:**
  1. Choose option `7` (Modify Student Details).
  2. Enter the student’s ID.
  3. Enter a new username and/or password.
  4. View Student Details (option `2`).
- **Expected:**  
  - The student record shows the updated username.
  - The old credentials no longer work; new credentials work.

**A6 – Modify Faculty Details**

- **Precondition:** A faculty with known ID exists and is Active.
- **Steps:**
  1. Choose option `8` (Modify Faculty Details).
  2. Enter the faculty ID.
  3. Enter a new username and/or password.
  4. View Faculty Details (option `4`).
- **Expected:**  
  - The faculty record shows the updated username.
  - The old credentials no longer work; new credentials work.

---

### 8.2 Faculty Menu – Manual Test Cases

**F1 – View Offering Courses**

- **Precondition:** Faculty is logged in and has at least one course assigned/created.
- **Steps:**
  1. Login as faculty (e.g., `faculty1 / facpass1`).
  2. Choose option `1` (View Offering Courses).
- **Expected:**  
  - A list of courses offered by that faculty is displayed with seat information.

**F2 – Remove Course from Catalog**

- **Precondition:** Faculty has at least one existing course.
- **Steps:**
  1. Choose option `3` (Remove Course from Catalog).
  2. Enter the course ID that belongs to this faculty.
  3. Choose option `1` (View Offering Courses) again.
- **Expected:**  
  - That course no longer appears in the faculty’s course list.
  - Students should not be able to enroll in the removed course.

**F3 – Update Course Details**

- **Precondition:** Faculty has at least one existing course.
- **Steps:**
  1. Choose option `4` (Update Course Details).
  2. Enter a valid course ID.
  3. Enter a new course name and/or max seats.
  4. Choose option `1` (View Offering Courses) again.
- **Expected:**  
  - The course shows the updated name and seat count.

**F4 – Change Password**

- **Precondition:** Logged in as a faculty.
- **Steps:**
  1. Choose option `5` (Change Password).
  2. Enter a new password.
  3. Logout (option `6`) and return to main menu.
  4. Try logging in again with old password (should fail).
  5. Login with new password (should pass).
- **Expected:**  
  - Login with the old password is rejected.
  - Login with the new password is successful.

---

### 8.3 Student Menu – Additional Manual Checks

In addition to the automated script for student enroll/unenroll, we manually verified:

- **Viewing courses when none are enrolled**  
  - Student logs in and chooses “View Enrolled Courses” before enrolling.  
  - Expected: message saying no courses enrolled.

- **Handling invalid course IDs**  
  - Student chooses Enroll, enters a non-existent course ID.  
  - Expected: “Course ID not found” or similar error, no file changes.

These manual tests complement the automated scripts and show that the system behaves correctly on invalid inputs and edge cases.
