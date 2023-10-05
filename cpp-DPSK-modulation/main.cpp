#include "math_operations.h"
#include "gray_code.h"
#include "dpsk_modulation.h"

#include <iostream>

using namespace std;

int main() {
    math::tests::RunAllTests();
    gray_code::tests::RunAllTests();
    dpsk_mod::tests::RunAllTests();
    return 0;
}
