#include "harness.hpp"

#include <iostream>

namespace fxtest {

namespace {
    int  checksRun          = 0;
    int  checksFailed       = 0;
    int  failedInCurrentTest = 0;
    bool skippedCurrentTest  = false;
    std::string skipReason;
    int  testsSkipped        = 0;
}

std::vector<TestCase>& Registry() {
    // Function-local static: constructed on first use, so it is guaranteed
    // to exist before any Registrar in any translation unit runs.
    static std::vector<TestCase> registry;
    return registry;
}

Registrar::Registrar(const char* name, void (*fn)()) {
    Registry().push_back({name, fn});
}

void CountCheck() {
    ++checksRun;
}

void ReportFailure(const std::string& what, const char* file, int line) {
    ++checksFailed;
    ++failedInCurrentTest;
    std::cerr << "  FAILED: " << what << "\n    at " << file << ":" << line << "\n";
}

void ReportSkip(const std::string& why) {
    skippedCurrentTest = true;
    skipReason = why;
}

int RunAll() {
    int testsFailed = 0;

    for (const auto& test : Registry()) {
        failedInCurrentTest = 0;
        skippedCurrentTest  = false;
        skipReason.clear();

        std::cout << "[ RUN  ] " << test.name << "\n";
        test.fn();

        // A test that failed a check before skipping is a failure, not a skip.
        if (failedInCurrentTest > 0) {
            ++testsFailed;
            std::cout << "[ FAIL ] " << test.name << " ("
                      << failedInCurrentTest << " checks)\n";
        } else if (skippedCurrentTest) {
            ++testsSkipped;
            std::cout << "[ SKIP ] " << test.name << " (" << skipReason << ")\n";
        } else {
            std::cout << "[  OK  ] " << test.name << "\n";
        }
    }

    std::cout << "\n" << (checksRun - checksFailed) << "/" << checksRun
              << " checks passed across " << Registry().size() << " tests\n";
    if (testsSkipped > 0) std::cout << testsSkipped << " test(s) skipped\n";
    if (testsFailed  > 0) std::cout << testsFailed  << " test(s) FAILED\n";

    return checksFailed;
}

}
