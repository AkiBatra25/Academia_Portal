#include <stdio.h>
#include <string.h>
#include "../include/common.h"

void add_user(int id, const char *username, const char *password, const char *role, int active) {
    FILE *fp = fopen("Server/users.dat", "ab");
    struct User u = { id, "", "", "", active };
    strcpy(u.username, username);
    strcpy(u.password, password);
    strcpy(u.role, role);
    fwrite(&u, sizeof(u), 1, fp);
    fclose(fp);
}

void add_course(int course_id, const char *name, int faculty_id, int max_seats) {
    FILE *fp = fopen("Server/courses.dat", "ab");
    struct Course c = { course_id, "", faculty_id, max_seats, 0 };
    strcpy(c.name, name);
    fwrite(&c, sizeof(c), 1, fp);
    fclose(fp);
}

int main() {
    // Clear old data
    FILE *fp = fopen("Server/users.dat", "w"); fclose(fp);
    fp = fopen("Server/courses.dat", "w"); fclose(fp);
    fp = fopen("Server/enrollments.dat", "w"); fclose(fp);

    // Sample Users
    add_user(1, "admin1", "admin1pwd", "admin", 1);
    add_user(2, "student1", "stud1pwd", "student", 1);
    add_user(3, "faculty1", "fact1pwd", "faculty", 1);

    add_user(4,"admin2","admin2pwd", "admin", 1);    
    add_user(5, "student2", "stud2pwd", "student", 1);
    add_user(6, "faculty2", "fact2pwd", "faculty", 1);
   
    add_user(7, "student3", "stud3pwd", "student", 1);
    add_user(8, "faculty3", "fact3pwd", "faculty", 1);
    
    add_user(9, "student4", "stud4pwd", "student", 1);
    

    // Sample Courses
    add_course(101, "OperatingSystems", 3, 3);
    add_course(102, "DBMS", 3, 2);
    add_course(103,"SPIT",8,3);

    
    printf("Initialization complete. Sample users and courses added.\n");
    return 0;
}
