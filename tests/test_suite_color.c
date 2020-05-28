#include "vector.h"
#include "test_util.h"
#include "color.h"
#include <assert.h>

void test_rgb_color_construction() {
    rgb_color_t zero = rgb(0, 0, 0);
    assert(zero.r == COLOR_BLACK.r);
    assert(zero.g == COLOR_BLACK.g);
    assert(zero.b == COLOR_BLACK.b);

    rgb_color_t x = rgb(0, 1, 1);
    assert(x.r == 0);
    assert(x.g == 1);
    assert(x.b == 1);

    rgb_color_t y = rgb(1, 1, 0);
    assert(y.r == 1);
    assert(y.g == 1);
    assert(y.b == 0);

    rgb_color_t one = rgb(1, 1, 1);
    assert(one.r == 1);
    assert(one.g == 1);
    assert(one.b == 1);
}

void test_hsv() {
    rgb_color_t color = hsv(0, 1, 1);
    assert(color.r == 1);
    assert(color.g == 0);
    assert(color.b == 0);

    color = hsv(60, 1, 1);
    assert(color.r == 1);
    assert(color.g == 1);
    assert(color.b == 0);

    color = hsv(120, 1, 1);
    assert(color.r == 0);
    assert(color.g == 1);
    assert(color.b == 0);

    color = hsv(180, 1, 1);
    assert(color.r == 0);
    assert(color.g == 1);
    assert(color.b == 1);

    color = hsv(240, 1, 1);
    assert(color.r == 0);
    assert(color.g == 0);
    assert(color.b == 1);

    color = hsv(300, 1, 1);
    assert(color.r == 1);
    assert(color.g == 0);
    assert(color.b == 1);

    color = hsv(360, 1, 1);
    assert(color.r == 1);
    assert(color.g == 0);
    assert(color.b == 0);
}

int main(int argc, char *argv[]) {
    puts("rgb_color_test START");

    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_rgb_color_construction);
    DO_TEST(test_hsv);

    puts("rgb_color_test PASS");
}
