// include/common.h
#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>
struct User {
    int id;
    char username[50];
    char password[50];
    char role[10]; // admin, student, faculty
    int active;    // 1 = active, 0 = inactive
};

struct Course {
    int course_id;
    char name[50];
    int faculty_id;
    int max_seats;
    int enrolled_count;
};

struct Enrollment {
    int student_id;
    int course_id;
};

// Function declarations
bool authenticate(const char *, const char *, const char *, struct User *);
void lock_file(int fd, short type);
void unlock_file(int fd);

#endif
