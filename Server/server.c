#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>  
#include "../include/common.h"

#define PORT 8080

void view_all_courses(int client_socket) {
    FILE *fp = fopen("server/courses.dat", "rb");
    if (!fp) {
        write(client_socket, "Error opening courses.dat\n", 27);
        return;
    }

    struct Course c;
    char buffer[256];
    int found = 0;

    write(client_socket, "\n--- Course List ---\n", 22);

    while (fread(&c, sizeof(c), 1, fp)) {
        found = 1;
        snprintf(buffer, sizeof(buffer),
                 "Course ID: %d | Name: %s | Faculty ID: %d | Seats: %d/%d\n",
                 c.course_id, c.name, c.faculty_id, c.enrolled_count, c.max_seats);
        write(client_socket, buffer, strlen(buffer));
    }

    if (!found) {
        write(client_socket, "⚠️ No courses available.\n", 26);
    }

    fclose(fp);
}


void add_user_from_admin(int client_socket, const char *role) {
    char buffer[1024], username[50], password[50];

    // Ask for username
    write(client_socket, "Enter new username: ", 21);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline
    strcpy(username, buffer);

    // Ask for password
    write(client_socket, "Enter new password: ", 21);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline
    strcpy(password, buffer);

    // Generate new ID
    FILE *fp = fopen("server/users.dat", "ab+");
    if (!fp) {
        write(client_socket, "Error opening users.dat\n", 25);
        return;
    }

    struct User u;
    int new_id = 1;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    new_id = size / sizeof(struct User) + 1;

    // Fill user struct
    u.id = new_id;
    strcpy(u.username, username);
    strcpy(u.password, password);
    strcpy(u.role, role);
    u.active = 1;

    fwrite(&u, sizeof(u), 1, fp);
    fclose(fp);

    char msg[100];
    snprintf(msg, sizeof(msg), "✅ %s added with ID %d\n", role, new_id);
    write(client_socket, msg, strlen(msg));
}

void view_users_by_role(int client_socket, const char *role) {
    FILE *fp = fopen("server/users.dat", "rb");
    if (!fp) {
        write(client_socket, "Error opening users.dat\n", 25);
        return;
    }

    struct User u;
    char buffer[256];
    int found = 0;

    snprintf(buffer, sizeof(buffer), "\n--- %s List ---\n", role);
    write(client_socket, buffer, strlen(buffer));

    while (fread(&u, sizeof(u), 1, fp)) {
        if (strcmp(u.role, role) == 0) {
            found = 1;
            snprintf(buffer, sizeof(buffer),
                     "ID: %d | Username: %s | Status: %s\n",
                     u.id, u.username, u.active ? "Active" : "Blocked");
            write(client_socket, buffer, strlen(buffer));
        }
    }

    if (!found) {
        snprintf(buffer, sizeof(buffer), "No %s accounts found.\n", role);
        write(client_socket, buffer, strlen(buffer));
    }

    fclose(fp);
}


void set_student_status(int client_socket, int new_status) {
    char buffer[1024];
    int id;

    while (1) {
        write(client_socket,
              new_status ? "Enter student ID to activate (or 0 to cancel): "
                         : "Enter student ID to block (or 0 to cancel): ",
              strlen(new_status ? "Enter student ID to activate (or 0 to cancel): "
                                : "Enter student ID to block (or 0 to cancel): "));

        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        id = atoi(buffer);

        if (id == 0) {
            write(client_socket, "Operation cancelled.\n", strlen("Operation cancelled.\n"));
            return;
        }

        FILE *fp = fopen("server/users.dat", "r+b");
        if (!fp) {
            write(client_socket, "Error opening users.dat\n", strlen("Error opening users.dat\n"));
            return;
        }

        struct User u;
        int found = 0;

        while (fread(&u, sizeof(u), 1, fp)) {
            if (u.id == id && strcmp(u.role, "student") == 0) {
                found = 1;

                // ✅ Prevent redundant status updates
                if (u.active == new_status) {
                    char msg[100];
                    snprintf(msg, sizeof(msg),
                             "⚠️ Student %d is already %s.\n", id,
                             new_status ? "Active" : "Blocked");
                    write(client_socket, msg, strlen(msg));
                    fclose(fp);
                    return;
                }

                u.active = new_status;
                fseek(fp, -sizeof(u), SEEK_CUR);
                fwrite(&u, sizeof(u), 1, fp);
                break;
            }
        }

        fclose(fp);

        if (found) {
            char msg[100];
            snprintf(msg, sizeof(msg),
                     "✅ Student %d has been %s.\n", id,
                     new_status ? "Activated" : "Blocked");
            write(client_socket, msg, strlen(msg));
            return;
        } else {
            write(client_socket, "❌ Invalid student ID. Try again.\n", strlen("❌ Invalid student ID. Try again.\n"));
        }
    }
}


void update_user_details_by_role(int client_socket, const char *role) {
    char buffer[1024];
    int id;
    char new_username[50], new_password[50];

    write(client_socket, "Enter user ID to modify: ", strlen("Enter user ID to modify: "));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    id = atoi(buffer);

    FILE *fp = fopen("server/users.dat", "r+b");
    if (!fp) {
        write(client_socket, "Error opening users.dat\n", strlen("Error opening users.dat\n"));
        return;
    }

    struct User u;
    int found = 0;

    while (fread(&u, sizeof(u), 1, fp)) {
        if (u.id == id && strcmp(u.role, role) == 0) {
            found = 1;

            // ✅ Prevent update if user is blocked
            if (u.active == 0) {
                write(client_socket, "❌ Cannot modify a blocked user.\n", strlen("❌ Cannot modify a blocked user.\n"));
                fclose(fp);
                return;
            }

            write(client_socket, "Enter new username: ", strlen("Enter new username: "));
            memset(buffer, 0, sizeof(buffer));
            read(client_socket, buffer, sizeof(buffer));
            buffer[strcspn(buffer, "\n")] = '\0';
            strcpy(new_username, buffer);

            write(client_socket, "Enter new password: ", strlen("Enter new password: "));
            memset(buffer, 0, sizeof(buffer));
            read(client_socket, buffer, sizeof(buffer));
            buffer[strcspn(buffer, "\n")] = '\0';
            strcpy(new_password, buffer);

            strcpy(u.username, new_username);
            strcpy(u.password, new_password);

            fseek(fp, -sizeof(u), SEEK_CUR);
            fwrite(&u, sizeof(u), 1, fp);
            write(client_socket, "✅ User updated successfully.\n", strlen("✅ User updated successfully.\n"));
            break;
        }
    }

    if (!found) {
        write(client_socket, "❌ No such user with that role and ID.\n", strlen("❌ No such user with that role and ID.\n"));
    }

    fclose(fp);
}




void unenroll_course(int client_socket, int student_id) {
    char buffer[1024];
    int course_id, found = 0;

    // Show currently enrolled courses
    FILE *efp = fopen("server/enrollments.dat", "rb");
    struct Enrollment e;
    int enrolled[100], count = 0;

    while (fread(&e, sizeof(e), 1, efp)) {
        if (e.student_id == student_id) {
            enrolled[count++] = e.course_id;
        }
    }
    fclose(efp);

    if (count == 0) {
        write(client_socket, "⚠️ You are not enrolled in any course.\n", 40);
        return;
    }

    // Show list
    FILE *cfp = fopen("server/courses.dat", "rb");
    struct Course c;
    write(client_socket, "\nYour Enrolled Courses:\n", 25);
    while (fread(&c, sizeof(c), 1, cfp)) {
        for (int i = 0; i < count; i++) {
            if (c.course_id == enrolled[i]) {
                char line[128];
                snprintf(line, sizeof(line), "ID: %d | %s\n", c.course_id, c.name);
                write(client_socket, line, strlen(line));
            }
        }
    }
    fclose(cfp);

    // Ask for course ID to unenroll
    write(client_socket, "Enter course ID to unenroll: ", 30);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course_id = atoi(buffer);

    // Rewrite enrollments.dat without the removed enrollment
    efp = fopen("server/enrollments.dat", "rb");
    FILE *temp = fopen("server/temp_enrollments.dat", "wb");

    while (fread(&e, sizeof(e), 1, efp)) {
        if (e.student_id == student_id && e.course_id == course_id) {
            found = 1;
            continue; // skip this record
        }
        fwrite(&e, sizeof(e), 1, temp);
    }

    fclose(efp);
    fclose(temp);

    if (!found) {
        write(client_socket, "⚠️ You are not enrolled in this course.\n", 40);
        remove("server/temp_enrollments.dat");
        return;
    }

    remove("server/enrollments.dat");
    rename("server/temp_enrollments.dat", "server/enrollments.dat");

    // Update course file to reduce count
    cfp = fopen("server/courses.dat", "r+b");
    int fd = fileno(cfp);
    lock_file(fd, F_WRLCK);
    while (fread(&c, sizeof(c), 1, cfp)) {
        if (c.course_id == course_id) {
            if (c.enrolled_count > 0) c.enrolled_count--;
            fseek(cfp, -sizeof(c), SEEK_CUR);
            fwrite(&c, sizeof(c), 1, cfp);
            break;
        }
    }
    unlock_file(fd);
    fclose(cfp);

    write(client_socket, "✅ Successfully unenrolled.\n", 28);
}


void enroll_course(int client_socket, int student_id) {
    char buffer[1024];
    int course_id;
    int already_enrolled = 0;

    // Send list of courses
    FILE *cfp = fopen("server/courses.dat", "rb");
    struct Course c;
    if (!cfp) {
        write(client_socket, "Error opening course file.\n", 28);
        return;
    }

    write(client_socket, "\nAvailable Courses:\n", 21);
    while (fread(&c, sizeof(c), 1, cfp)) {
        char line[128];
        snprintf(line, sizeof(line), "ID: %d | %s | Seats: %d/%d\n",
                 c.course_id, c.name, c.enrolled_count, c.max_seats);
        write(client_socket, line, strlen(line));
    }
    fclose(cfp);

    // Ask for course ID
    write(client_socket, "Enter course ID to enroll: ", 28);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course_id = atoi(buffer);

    // Check if already enrolled
    FILE *efp = fopen("server/enrollments.dat", "rb");
    struct Enrollment e;
    while (fread(&e, sizeof(e), 1, efp)) {
        if (e.student_id == student_id && e.course_id == course_id) {
            already_enrolled = 1;
            break;
        }
    }
    fclose(efp);

    if (already_enrolled) {
        write(client_socket, "⚠️ You are already enrolled in this course.\n", 44);
        return;
    }

    // Lock course file for writing
    cfp = fopen("server/courses.dat", "r+b");
    int found = 0;
    int fd = fileno(cfp);
    lock_file(fd, F_WRLCK);
    while (fread(&c, sizeof(c), 1, cfp)) {
        if (c.course_id == course_id) {
            found = 1;
            if (c.enrolled_count >= c.max_seats) {
                unlock_file(fd);
                fclose(cfp);
                write(client_socket, "⚠️ Course is full.\n", 20);
                return;
            }

            // Update enrolled count
            c.enrolled_count++;
            fseek(cfp, -sizeof(c), SEEK_CUR);
            fwrite(&c, sizeof(c), 1, cfp);
            break;
        }
    }
    unlock_file(fd);
    fclose(cfp);

    if (!found) {
        write(client_socket, "⚠️ Course ID not found.\n", 25);
        return;
    }

    // Add to enrollments
    efp = fopen("server/enrollments.dat", "ab");
    fd = fileno(efp);
    lock_file(fd, F_WRLCK);
    struct Enrollment new_e = { student_id, course_id };
    fwrite(&new_e, sizeof(new_e), 1, efp);
    unlock_file(fd);
    fclose(efp);

    write(client_socket, "✅ Enrollment successful.\n", 26);
}


void view_enrolled_courses(int client_socket, int student_id) {
    struct Enrollment e;
    struct Course c;
    int enrolled_ids[100], count = 0;
    char buffer[1024];

    FILE *efp = fopen("server/enrollments.dat", "rb");
    if (!efp) {
        write(client_socket, "Error opening enrollment file.\n", 32);
        return;
    }

    // Find course IDs student is enrolled in
    while (fread(&e, sizeof(e), 1, efp)) {
        if (e.student_id == student_id) {
            enrolled_ids[count++] = e.course_id;
        }
    }
    fclose(efp);

    if (count == 0) {
        write(client_socket, "⚠️ You are not enrolled in any courses.\n", 40);
        return;
    }

    FILE *cfp = fopen("server/courses.dat", "rb");
    if (!cfp) {
        write(client_socket, "Error opening course file.\n", 28);
        return;
    }

    write(client_socket, "\nYour Enrolled Courses:\n", 25);
    while (fread(&c, sizeof(c), 1, cfp)) {
        for (int i = 0; i < count; i++) {
            if (c.course_id == enrolled_ids[i]) {
                snprintf(buffer, sizeof(buffer), "ID: %d | %s\n", c.course_id, c.name);
                write(client_socket, buffer, strlen(buffer));
            }
        }
    }
    fclose(cfp);
}


void change_password(int client_socket, int user_id) {
    char buffer[1024];
    char newpass[50];

    write(client_socket, "Enter new password: ", 21);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(newpass, buffer);

    FILE *fp = fopen("server/users.dat", "r+b");
    if (!fp) {
        write(client_socket, "Error opening user file.\n", 26);
        return;
    }

    struct User u;
    int found = 0;

    while (fread(&u, sizeof(u), 1, fp)) {
        if (u.id == user_id) {
            strcpy(u.password, newpass);
            fseek(fp, -sizeof(u), SEEK_CUR);
            fwrite(&u, sizeof(u), 1, fp);
            found = 1;
            break;
        }
    }

    fclose(fp);

    if (found) {
        write(client_socket, "✅ Password changed successfully.\n", 34);
    } else {
        write(client_socket, "❌ User not found.\n", 19);
    }
}

void view_offering_courses(int client_socket, int faculty_id) {
    FILE *fp = fopen("server/courses.dat", "rb");
    if (!fp) {
        write(client_socket, "Error opening courses.dat\n", 27);
        return;
    }

    struct Course c;
    char buffer[256];
    int found = 0;

    write(client_socket, "\n--- Your Courses ---\n", 23);
    while (fread(&c, sizeof(c), 1, fp)) {
        if (c.faculty_id == faculty_id) {
            found = 1;
            snprintf(buffer, sizeof(buffer),
                     "Course ID: %d | Name: %s | Seats: %d/%d\n",
                     c.course_id, c.name, c.enrolled_count, c.max_seats);
            write(client_socket, buffer, strlen(buffer));
        }
    }

    if (!found) {
        write(client_socket, "You are not offering any courses.\n", 35);
    }

    fclose(fp);
}

void add_course_by_faculty(int client_socket, int faculty_id) {
    char buffer[1024];
    char name[50];
    int max_seats;

    write(client_socket, "Enter course name: ", 20);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(name, buffer);

    write(client_socket, "Enter max seats: ", 18);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    max_seats = atoi(buffer);

    FILE *fp = fopen("server/courses.dat", "ab+");
    struct Course c;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    int new_id = size / sizeof(struct Course) + 101;  // Course IDs start at 101

    c.course_id = new_id;
    strcpy(c.name, name);
    c.max_seats = max_seats;
    c.enrolled_count = 0;
    c.faculty_id = faculty_id;

    fwrite(&c, sizeof(c), 1, fp);
    fclose(fp);

    char msg[100];
    snprintf(msg, sizeof(msg), "✅ Course '%s' added with ID %d\n", name, new_id);
    write(client_socket, msg, strlen(msg));
}

void remove_course_by_faculty(int client_socket, int faculty_id) {
    char buffer[1024];
    int course_id, found = 0;

    write(client_socket, "Enter course ID to remove: ", 28);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course_id = atoi(buffer);

    FILE *fp = fopen("server/courses.dat", "rb");
    FILE *temp = fopen("server/temp_courses.dat", "wb");
    struct Course c;

    while (fread(&c, sizeof(c), 1, fp)) {
        if (c.course_id == course_id && c.faculty_id == faculty_id) {
            found = 1;
            continue; // skip writing to temp
        }
        fwrite(&c, sizeof(c), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove("server/courses.dat");
        rename("server/temp_courses.dat", "server/courses.dat");
        write(client_socket, "✅ Course removed.\n", 19);
    } else {
        remove("server/temp_courses.dat");
        write(client_socket, "❌ Course not found or unauthorized.\n", 36);
    }
}

void update_course_by_faculty(int client_socket, int faculty_id) {
    char buffer[1024];
    int course_id;
    char new_name[50];
    int new_seats;

    write(client_socket, "Enter course ID to update: ", 28);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course_id = atoi(buffer);

    FILE *fp = fopen("server/courses.dat", "r+b");
    struct Course c;
    int found = 0;

    while (fread(&c, sizeof(c), 1, fp)) {
        if (c.course_id == course_id && c.faculty_id == faculty_id) {
            found = 1;

            write(client_socket, "Enter new course name: ", 24);
            memset(buffer, 0, sizeof(buffer));
            read(client_socket, buffer, sizeof(buffer));
            buffer[strcspn(buffer, "\n")] = '\0';
            strcpy(new_name, buffer);

            write(client_socket, "Enter new max seats: ", 22);
            memset(buffer, 0, sizeof(buffer));
            read(client_socket, buffer, sizeof(buffer));
            new_seats = atoi(buffer);

            strcpy(c.name, new_name);
            c.max_seats = new_seats;

            fseek(fp, -sizeof(c), SEEK_CUR);
            fwrite(&c, sizeof(c), 1, fp);
            write(client_socket, "✅ Course updated successfully.\n", 32);
            break;
        }
    }

    if (!found) {
        write(client_socket, "❌ Course not found or unauthorized.\n", 36);
    }

    fclose(fp);
}





// --- Admin Menu ---
void handle_admin(int client_socket) {
    char buffer[1024];
    while (1) {
        char menu[] =
            "\n--- Admin Menu ---\n"
            "1. Add Student\n"
            "2. View Student Details\n"
            "3. Add Faculty\n"
            "4. View Faculty Details\n"
            "5. Activate Student\n"
            "6. Block Student\n"
            "7. Modify Student Details\n"
            "8. Modify Faculty Details\n"
            "9. Logout and Exit\n"
            "Enter your choice: ";
        write(client_socket, menu, strlen(menu));

        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        int choice = atoi(buffer);

        switch (choice) {
            case 1:
                add_user_from_admin(client_socket, "student");
                break;
            case 2:
                view_users_by_role(client_socket,"student");
                break;
            case 3:
                add_user_from_admin(client_socket, "faculty");
                break;
            case 4:
                view_users_by_role(client_socket,"faculty");
                break;
            case 5:
                set_student_status(client_socket,1);
                break;
            case 6:
                set_student_status(client_socket,0);
                break;
            case 7:
                update_user_details_by_role(client_socket,"student");
                break;
            case 8:
                update_user_details_by_role(client_socket,"faculty");
                break;
            case 9:
                write(client_socket, "Exiting admin menu.\n", 21);
                return;

            default:
                write(client_socket, "Invalid choice. Enter from 1 to 9 only. \n", 16);
        }
    }
}


// --- Student Menu ---
void handle_student(int client_socket, int student_id) {
    char buffer[1024];

    while (1) {
        char menu[] =
            "\n--- Student Menu ---\n"
            "1. View All Courses\n"
            "2. Enroll (pick) in Course\n"
            "3. Unenroll from Course\n"
            "4. View Enrolled Courses\n"
            "5. Change Password\n"
            "6. Logout and Exit\n"
            "Enter your choice: ";
        write(client_socket, menu, strlen(menu));

        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        int choice = atoi(buffer);

        switch (choice) {
            case 1:
                view_all_courses(client_socket);
                break;
            case 2:
                enroll_course(client_socket, student_id);
                break;
            case 3:
                unenroll_course(client_socket, student_id);
                break;
            case 4:
                view_enrolled_courses(client_socket, student_id);
                break;
            case 5:
                change_password(client_socket, student_id);
                break;
            case 6:
                write(client_socket, "Exiting student menu.\n", 23);
                return;
            default:
                write(client_socket, "Invalid choice. Enter from 1 to 6 only. \n", 17);
        }
    }
}


// --- Faculty Menu ---
void handle_faculty(int client_socket, int faculty_id) {
    char buffer[1024];

    while (1) {
        char menu[] =
            "\n--- Faculty Menu ---\n"
            "1. View Offering Courses\n"
            "2. Add New Course\n"
            "3. Remove Course from Catalog\n"
            "4. Update Course Details\n"
            "5. Change Password\n"
            "6. Logout and Exit\n"
            "Enter your choice: ";
        write(client_socket, menu, strlen(menu));

        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        int choice = atoi(buffer);

        switch (choice) {
            case 1:
                view_offering_courses(client_socket,faculty_id);
                break;
            case 2:
                add_course_by_faculty(client_socket,faculty_id);
                break;
            case 3:
                remove_course_by_faculty(client_socket,faculty_id);
                break;
            case 4:
                update_course_by_faculty(client_socket,faculty_id);
                break;

            case 5:
                change_password(client_socket, faculty_id);
                break;
            case 6:
                write(client_socket, "Exiting faculty menu.\n", 23);
                return;
            default:
                write(client_socket, "Invalid choice. Enter from 1 to 6 only. \n", 31);
        }
    }
}


// --- Handle each client connection ---
void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[1024];
    struct User user;

    while (1) {
        // Show login menu
        write(client_socket, "\n--- Welcome Back to Academia :: Course Registration ---\n", strlen("\n--- Welcome Back to Academia :: Course Registration ---\n"));
        write(client_socket, "Login type - \n", strlen("Login type - \n"));
        write(client_socket, "1. Admin Login\n", strlen("1. Admin Login\n"));
        write(client_socket, "2. Faculty Login\n", strlen("2. Faculty Login\n"));
        write(client_socket, "3. Student Login\n", strlen("3. Student Login\n"));
        write(client_socket, "4. Exit Application\n", strlen("4. Exit Application\n"));
        write(client_socket, "Enter your choice: ", strlen("Enter your choice: "));

        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        int choice = atoi(buffer);

        if (choice == 4) {
            write(client_socket, "Goodbye!\n", strlen("Goodbye!\n"));
            break; // Exit the loop
        }

        char *expected_role = NULL;
        if (choice == 1) expected_role = "admin";
        else if (choice == 2) expected_role = "faculty";
        else if (choice == 3) expected_role = "student";
        else {
            write(client_socket, "❌ Invalid login type.\n", 23);
            continue;
        }

        // Ask for credentials
        write(client_socket, "Enter username: ", 17);
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        buffer[strcspn(buffer, "\n")] = '\0';
        char username[50];
        strcpy(username, buffer);

        write(client_socket, "Enter password: ", 17);
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        buffer[strcspn(buffer, "\n")] = '\0';
        char password[50];
        strcpy(password, buffer);

        // Authenticate
        if (authenticate(username, password, expected_role, &user)) {
            char msg[128];
            snprintf(msg, sizeof(msg), "\n✅ Login successful. Role: %s\n", user.role);
            write(client_socket, msg, strlen(msg));

            // Call the appropriate menu
            if (strcmp(user.role, "admin") == 0)
                handle_admin(client_socket);
            else if (strcmp(user.role, "faculty") == 0)
                handle_faculty(client_socket, user.id);
            else if (strcmp(user.role, "student") == 0)
                handle_student(client_socket, user.id);

            write(client_socket, "\n✅ You have been logged out.\n", 30);
        } else {
            write(client_socket, "❌ Invalid credentials or role.\n", 33);
        }
    }

    close(client_socket);
    pthread_exit(NULL);
}


// --- Main server function ---
int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    printf("🖥️ Server listening on port %d...\n", PORT);


    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, pclient);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}


