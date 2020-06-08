#include <stdlib.h>
#include <stdbool.h>
#include "test_util.h"

int main(int argc, char *argv[]) {
    puts("aster_blaster_player START");

    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    puts("aster_blaster_player PASS");
}
