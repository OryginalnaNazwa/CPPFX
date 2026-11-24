#include "harness.hpp"

#include <exception>
#include <iostream>

namespace fxtest {

namespace {
    int  checksRun          = 0;
    int  checksFailed       = 0;
    int  failedInCurrentTest = 0;
    bool skippedCurrentTest  = false;
    std::string skipReason;
    int  testsSkipped        = 0;

    /// Counts an exception that escaped a test body as one failed check.
    void ReportEscape(const std::string& what) {
        ++checksRun;
        ++checksFailed;
        ++failedInCurrentTest;
        std::cerr << "  FAILED: " << what
                  << "\n    thrown out of the test body, so the rest of it didn't run\n";
    }
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
        // One test throwing must not take the rest of the run down with it.
        try {
            test.fn();
        } catch (const std::exception& e) {
            ReportEscape(std::string("uncaught exception: ") + e.what());
        } catch (...) {
            ReportEscape("uncaught exception of a non-standard type");
        }

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
