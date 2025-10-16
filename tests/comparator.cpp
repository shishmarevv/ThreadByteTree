/*
* @author: Viktor Shishmarev
 * @date: 14.10.2025
 * @description:
 */
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "comparator.h"

using tbt::ByteVector;
using tbt::ByteVectorLess;

static ByteVector bv(std::initializer_list<int> il) {
    ByteVector v;
    v.reserve(il.size());
    for (int x : il) v.push_back(static_cast<uint8_t>(x));
    return v;
}

static std::string vec_to_str(const ByteVector& v) {
    std::ostringstream oss;
    oss << "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i) oss << ", ";
        oss << "0x" << std::hex << std::uppercase << std::setw(2)
            << std::setfill('0') << static_cast<int>(v[i]);
    }
    oss << "]";
    return oss.str();
}

struct TestCase {
    const char* name;
    ByteVector left;
    ByteVector right;
    bool expectLess; // true if left < right
};

int main() {
    std::vector<TestCase> tests;

    // Edge Cases
    tests.push_back({"edge: empty vs empty", bv({}), bv({}), false});
    tests.push_back({"edge: empty vs non-empty", bv({}), bv({0}), true});
    tests.push_back({"edge: non-empty vs empty", bv({0}), bv({}), false});
    tests.push_back({"edge: equal vectors (short)", bv({10, 20}), bv({10, 20}), false});
    tests.push_back({"edge: prefix shorter is less", bv({1, 2}), bv({1, 2, 3}), true});
    tests.push_back({"edge: prefix longer is greater", bv({1, 2, 3}), bv({1, 2}), false});
    tests.push_back({"edge: unsigned semantics 0xFF > 0x7F", bv({0xFF}), bv({0x7F}), false});
    tests.push_back({"edge: unsigned semantics 0x7F < 0xFF", bv({0x7F}), bv({0xFF}), true});
    tests.push_back({"edge: differ at inner byte", bv({1, 0, 3}), bv({1, 1, 0}), true});

    // Basic Cases
    tests.push_back({"case: 1 < 2", bv({1}), bv({2}), true});
    tests.push_back({"case: 2 > 1", bv({2}), bv({1}), false});
    tests.push_back({"case: [1,3] < [1,4]", bv({1, 3}), bv({1, 4}), true});
    tests.push_back({"case: [1,4] > [1,3]", bv({1, 4}), bv({1, 3}), false});
    tests.push_back({"case: [2,0,0] < [2,0,1]", bv({2, 0, 0}), bv({2, 0, 1}), true});
    tests.push_back({"case: [2,0,1] > [2,0,0]", bv({2, 0, 1}), bv({2, 0, 0}), false});
    tests.push_back({"case: equal (with 0xFF)", bv({0, 255}), bv({0, 255}), false});
    tests.push_back({"case: prefix shorter is less (normal)", bv({0, 10, 20}), bv({0, 10, 20, 0}), true});
    tests.push_back({"case: differ late [10,0] < [10,1]", bv({10, 0}), bv({10, 1}), true});
    tests.push_back({"case: [255,0] < [255,1]", bv({255, 0}), bv({255, 1}), true});

    int failed = 0;
    int total = 0;

    auto run = [&](const TestCase& tc) {
        ++total;
        const bool res = ByteVectorLess(tc.left, tc.right);
        const bool res_rev = ByteVectorLess(tc.right, tc.left);
        const bool eq = (tc.left == tc.right);

        bool ok = (res == tc.expectLess) && !(res && res_rev);
        if (eq) ok = ok && (!res && !res_rev);

        if (!ok) {
            std::cerr << "FAIL: " << tc.name << "\n"
                      << "  left  = " << vec_to_str(tc.left) << "\n"
                      << "  right = " << vec_to_str(tc.right) << "\n"
                      << "  expect left < right = " << (tc.expectLess ? "true" : "false") << "\n"
                      << "  got: left<right=" << (res ? "true" : "false")
                      << ", right<left=" << (res_rev ? "true" : "false") << "\n";
            ++failed;
        }
    };

    for (const auto& tc : tests) run(tc);

    // Доп. проверка транзитивности: a < b < c => a < c
    {
        ++total;
        auto a = bv({1});
        auto b = bv({1, 1});
        auto c = bv({2});
        bool ok = ByteVectorLess(a, b) && ByteVectorLess(b, c) && ByteVectorLess(a, c);
        if (!ok) {
            std::cerr << "FAIL: transitivity check a<b<c => a<c\n"
                      << "  a=" << vec_to_str(a) << ", b=" << vec_to_str(b)
                      << ", c=" << vec_to_str(c) << "\n";
            ++failed;
        }
    }

    if (failed == 0) {
        std::cout << "OK: all tests passed (" << total << ")\n";
        return 0;
    }
    std::cerr << "FAILED: " << failed << " of " << total << "\n";
    return 1;
}