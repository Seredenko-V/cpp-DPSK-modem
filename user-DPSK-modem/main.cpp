#include <math_operations.h>
#include <gray_code.h>
#include <dpsk_modulator.h>
#include <dpsk_demodulator.h>

using namespace std;

int main([[gnu::unused]] int argc, [[gnu::unused]] char *argv[]) {
    math::tests::RunAllTests();
    gray_code::tests::RunAllTests();
    dpsk_mod::tests::RunAllTests();
    dpsk_demod::tests::RunAllTests();
    return 0;
}
