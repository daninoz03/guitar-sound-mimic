# Testing Guide

## Quick Test Run

```bash
./run_tests.sh
```

That's it! This builds and runs all unit tests.

## What Gets Tested

### 🎵 Audio Processing (ToneProfiler)
- Audio block processing and accumulation
- Profile generation from audio
- Frequency content analysis
- Handling of various audio conditions (silence, noise, etc.)

**9 tests covering:**
- Initialization and lifecycle
- Audio processing pipeline
- Profile generation logic
- Edge cases and error conditions

### 🎛️ Real-time Processing (ToneMatcher)
- Real-time audio processing with tone profiles
- Different buffer sizes (64-2048 samples)
- Mono and stereo audio handling
- Audio quality preservation

**8 tests covering:**
- Processing with and without profiles
- Various buffer configurations
- Output validation (no NaN/Inf)
- Level preservation

### 📊 DSP Algorithms (FrequencyAnalyzer)
- FFT-based frequency detection
- Fundamental frequency detection
- Harmonic content extraction
- Guitar frequency range (82Hz - 2kHz)

**10 tests covering:**
- Frequency detection accuracy
- Low-frequency guitar notes
- Harmonic analysis
- Edge cases (silence, noise)

### 📋 Data Structures (ToneProfile)
- Profile data storage and validation
- Compression parameters
- EQ band configuration
- Metadata handling

**10 tests covering:**
- Construction and copying
- Validity checks
- Parameter storage
- Realistic guitar tone data

## Total: 37 Unit Tests

## Interpreting Test Results

### ✅ All Tests Pass
```
Total: 37 tests, ALL PASSED ✓
✅ All tests passed!
```
**You're good to go!** The audio engine is working correctly.

### ❌ Some Tests Fail
```
ToneProfiler: ✗ 1 FAILED (of 9 tests)
Total: 37 tests, 1 FAILED ✗
❌ Some tests failed!
```

**What to do:**
1. Check the detailed error messages above
2. Look for the specific test that failed
3. See common issues below

## Common Test Issues

### Build Errors
```
error: cannot find symbol ToneProfiler
```
**Solution:** Make sure you built the main app first:
```bash
./setup.sh
```

### FFT-Related Failures
Frequency detection tests might be slightly off due to FFT bin resolution.

**Expected:** Detects 440Hz  
**Actual:** Detects 438Hz or 442Hz  

This is **normal** - FFT has limited frequency resolution. The tests allow for tolerance.

### Platform-Specific Issues

**macOS:** Should work out of the box  
**Linux:** May need audio drivers (ALSA/JACK)  
**Windows:** May need ASIO SDK for full audio support

## Running Specific Tests

To run only certain categories, modify `Tests/TestRunner.cpp`:

```cpp
// Run only audio tests
runner.runTestsInCategory("Audio");

// Or only DSP tests
runner.runTestsInCategory("DSP");
```

Then rebuild and run:
```bash
cd build
cmake --build . --target GuitarSoundMimicTests
./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
```

## Adding Your Own Tests

1. **Create a test file** in `Tests/` directory
2. **Add it to CMakeLists.txt**
3. **Run `cmake ..` again to regenerate build files**
4. **Build and run tests**

Example test:
```cpp
#include <JuceHeader.h>
#include "../Source/MyClass.h"

class MyClassTests : public juce::UnitTest
{
public:
    MyClassTests() : juce::UnitTest("MyClass", "MyCategory") {}
    
    void runTest() override
    {
        beginTest("MyClass does something");
        {
            MyClass obj;
            obj.doSomething();
            expect(obj.getValue() == 42, "Should return 42");
        }
    }
};

static MyClassTests myClassTests;
```

## Debugging Failed Tests

### Method 1: Add Logging
```cpp
beginTest("My test");
{
    DBG("Value: " + String(myValue));  // Prints to console
    expect(myValue == expected, "Check value");
}
```

### Method 2: Run in Debugger (Xcode)
```bash
cd build
cmake .. -G Xcode
open GuitarSoundMimicTests.xcodeproj
# Set breakpoints in test code
# Run in Xcode
```

### Method 3: Run in lldb
```bash
lldb ./build/GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
(lldb) run
# When it fails:
(lldb) bt  # backtrace
(lldb) frame variable  # inspect variables
```

## Continuous Testing

### During Development
```bash
# Watch for changes and re-run tests
while true; do
    inotifywait -e modify Source/**/*.cpp
    ./run_tests.sh
done
```

### Pre-commit Hook
Add to `.git/hooks/pre-commit`:
```bash
#!/bin/bash
./run_tests.sh
if [ $? -ne 0 ]; then
    echo "Tests failed! Commit aborted."
    exit 1
fi
```

Make it executable:
```bash
chmod +x .git/hooks/pre-commit
```

## Performance Testing

While these are functional tests, they also provide basic performance verification:

- Audio processing should complete in real-time
- No memory leaks
- No crashes with various input conditions

For detailed performance profiling:
```bash
# macOS
instruments -t "Time Profiler" ./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests

# Linux
valgrind --tool=callgrind ./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
```

## Test-Driven Development

When adding new features:

1. **Write the test first** (it will fail)
2. **Implement the feature**
3. **Run tests** (should pass now)
4. **Refactor** if needed
5. **Tests still pass** ✅

Example workflow:
```bash
# 1. Add test in Tests/MyNewFeatureTests.cpp
# 2. Run tests (should fail)
./run_tests.sh

# 3. Implement feature in Source/
# 4. Run tests (should pass)
./run_tests.sh
```

## CI/CD Integration

### GitHub Actions
See `.github/workflows/tests.yml` (if added):
```yaml
- name: Run Tests
  run: ./run_tests.sh
```

### GitLab CI
```yaml
test:
  script:
    - ./setup.sh
    - ./run_tests.sh
```

## Test Coverage Analysis

To see which code is covered by tests (macOS with Xcode):

```bash
cd build
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Debug
xcodebuild test -scheme GuitarSoundMimicTests -enableCodeCoverage YES
```

Then view coverage in Xcode.

## Why Testing Matters

✅ **Confidence** - Know your code works  
✅ **Refactoring** - Change code without fear  
✅ **Documentation** - Tests show how to use the code  
✅ **Regression Prevention** - Catch bugs before users do  
✅ **Quality** - Professional audio software needs reliable DSP  

## Test Philosophy

> "Code without tests is legacy code" - Michael Feathers

For audio processing:
- **Correctness** is critical (bad audio = unusable)
- **Performance** matters (real-time constraints)
- **Stability** is essential (no crashes during performance)

Our tests ensure all three.

## Further Reading

- [JUCE UnitTest Class](https://docs.juce.com/master/classUnitTest.html)
- [Test-Driven Development](https://en.wikipedia.org/wiki/Test-driven_development)
- [Writing Testable Code](https://testing.googleblog.com/2008/08/by-miko-hevery-so-you-decided-to.html)

---

**Questions?** See `Tests/README.md` for detailed test documentation.





