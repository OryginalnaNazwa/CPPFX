#ifndef CPPFX_TEST_HARNESS_HPP
#define CPPFX_TEST_HARNESS_HPP

#include <sstream>
#include <string>
#include <vector>

/**********************************************************************
 *  @file harness.hpp
 *  @brief Minimal test harness. No dependencies.
 *
 *  Write a test like this, in any .cpp file under tests/:
 *
 *      TEST(colour_fades) {
 *          Colour c(RED);
 *          c.Fade(0.0f);
 *          CHECK_EQ(int(c.GetColour().a), 0);
 *      }
 *
 *  The TEST macro registers the function automatically, so a new test
 *  file needs no changes to main.cpp or anywhere else. Drop it in
 *  tests/ and the CMake glob picks it up.
 **********************************************************************/

namespace fxtest {

/// One registered test.
struct TestCase {
    std::string name;
    void (*fn)();
};

/// The list every TEST adds itself to.
std::vector<TestCase>& Registry();

/// Runs every registered test. Returns the number of failed checks.
int RunAll();

/// Counts a check that ran. Called by the macros, not directly.
void CountCheck();
/// Records a failed check. Called by the macros, not directly.
void ReportFailure(const std::string& what, const char* file, int line);

/// Adds a test to the registry at static initialisation time.
struct Registrar {
    Registrar(const char* name, void (*fn)());
};

}

/// Declares and registers a test. The body follows the macro.
#define TEST(name)                                                             \
    static void name();                                                        \
    static ::fxtest::Registrar registrar_##name(#name, name);                  \
    static void name()

/// Checks a condition. On failure prints the expression as you wrote it.
#define CHECK(expr)                                                            \
    do {                                                                       \
        ::fxtest::CountCheck();                                                \
        if (!(expr)) ::fxtest::ReportFailure(#expr, __FILE__, __LINE__);       \
    } while (0)

/// Compares two values. On failure prints both sides, so you do not have
/// to add a print and rerun to find out what you actually got.
#define CHECK_EQ(a, b)                                                         \
    do {                                                                       \
        ::fxtest::CountCheck();                                                \
        auto&& _a = (a);                                                       \
        auto&& _b = (b);                                                       \
        if (!(_a == _b)) {                                                     \
            std::ostringstream _os;                                            \
            _os << #a " == " #b "\n    got:      " << _a                       \
                << "\n    expected: " << _b;                                   \
            ::fxtest::ReportFailure(_os.str(), __FILE__, __LINE__);            \
        }                                                                      \
    } while (0)

/// Checks that an expression throws a particular exception type.
#define CHECK_THROWS_AS(expr, ex)                                              \
    do {                                                                       \
        ::fxtest::CountCheck();                                                \
        bool _caught = false;                                                  \
        bool _wrong  = false;                                                  \
        try { (void)(expr); }                                                  \
        catch (const ex&) { _caught = true; }                                  \
        catch (...) { _wrong = true; }                                         \
        if (_wrong)                                                            \
            ::fxtest::ReportFailure(#expr " threw the wrong type, expected "   \
                                    #ex, __FILE__, __LINE__);                  \
        else if (!_caught)                                                     \
            ::fxtest::ReportFailure(#expr " did not throw " #ex,               \
                                    __FILE__, __LINE__);                       \
    } while (0)

#endif // CPPFX_TEST_HARNESS_HPP
