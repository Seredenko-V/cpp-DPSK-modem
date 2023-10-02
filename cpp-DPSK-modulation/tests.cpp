#include "gray_code.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace gray_code {
    namespace tests {
        void TestIsPowerOfTwo() {
            assert(IsPowerOfTwo(1));
            assert(IsPowerOfTwo(2));
            assert(IsPowerOfTwo(16));
            assert(IsPowerOfTwo(4096));
            assert(!IsPowerOfTwo(-2));
            assert(!IsPowerOfTwo(5));
            assert(!IsPowerOfTwo(24));
            assert(!IsPowerOfTwo(4097));
            cerr << "gray_code::TestIsPowerOfTwo has passed"s << endl;
        }

        void TestThrowsFromExtractNumBitsFormValue(int value) {
            try {
                ExtractNumBitsFormValue(value);
                assert(false);
            } catch (const invalid_argument& /*except*/) {
                // число должно быть положительным
            }
        }

        void TestExtractNumBitsFormValue() {
            TestThrowsFromExtractNumBitsFormValue(0);
            TestThrowsFromExtractNumBitsFormValue(-7);
            assert(ExtractNumBitsFormValue(1) == 1);
            assert(ExtractNumBitsFormValue(15) == 4);
            assert(ExtractNumBitsFormValue(4096) == 13);
            cerr << "gray_code::TestExtractNumBitsFormValue has passed"s << endl;
        }

        void RunAllTests() {
            TestIsPowerOfTwo();
            TestExtractNumBitsFormValue();
            cerr << "gray_code::AllTests has passed"s << endl;
        }
    } // namespace tests
} // namespace gray_code
