#include "harness.hpp"

/**********************************************************************
 *  Test runner. Nothing to edit here when you add tests - every TEST
 *  registers itself, so this just runs whatever was linked in.
 *
 *  Returns non-zero when anything failed, which is what makes ctest and
 *  the CI workflow notice.
 **********************************************************************/

int main() {
    return fxtest::RunAll() > 0 ? 1 : 0;
}
