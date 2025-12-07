#include <stdio.h>
#include "../include/common.h"

/*
 * Simple UNIT TEST DRIVER for authenticate()
 *
 * This file directly calls the authenticate() function from common.c.
 * It does not use sockets or the client UI.
 *
 * We assume that init/init_data has already created:
 *   - an admin user:   username = "admin1",   password = "admin1pwd"
 *   - a student user:  username = "student1", password = "pass1"
 * (Adjust these values below if your actual usernames/passwords differ.)
 */

int main() {
    struct User u;
    int ok;

    printf("===== UNIT TEST: authenticate() =====\n\n");

    // Test 1: valid admin credentials
    printf("Test 1: valid admin credentials (admin1 / admin1pwd, role=admin)\n");
    ok = authenticate("admin1", "admin1pwd", "admin", &u);
    if (ok)
        printf("  -> PASS (login allowed)\n\n");
    else
        printf("  -> FAIL (login rejected unexpectedly)\n\n");

    // Test 2: wrong password
    printf("Test 2: wrong password for admin (admin1 / wrongpass, role=admin)\n");
    ok = authenticate("admin1", "wrongpass", "admin", &u);
    if (!ok)
        printf("  -> PASS (login correctly rejected)\n\n");
    else
        printf("  -> FAIL (login incorrectly allowed)\n\n");

    // Test 3: wrong role (use admin creds but ask for student role)
    printf("Test 3: correct credentials but wrong role (admin1 as student)\n");
    ok = authenticate("admin1", "admin1pwd", "student", &u);
    if (!ok)
        printf("  -> PASS (login correctly rejected)\n\n");
    else
        printf("  -> FAIL (login incorrectly allowed)\n\n");

    // Test 4: valid student credentials
    printf("Test 4: valid student credentials (student1 / pass1, role=student)\n");
    ok = authenticate("student1", "pass1", "student", &u);
    if (ok)
        printf("  -> PASS (login allowed)\n\n");
    else
        printf("  -> FAIL (login rejected unexpectedly)\n\n");

    printf("===== END OF authenticate() UNIT TESTS =====\n");
    return 0;
}
