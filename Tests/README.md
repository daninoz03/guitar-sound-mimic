# Unit Tests

Comprehensive unit tests for the Guitar Sound Mimic audio processing engine.

## Test Coverage

### ToneProfilerTests (Audio)
Tests for the audio analysis engine:
- ✅ Initialization and preparation
- ✅ Audio block processing
- ✅ Sample accumulation
- ✅ Profile generation with sufficient data
- ✅ Profile generation failure with insufficient data
- ✅ Frequency content analysis
- ✅ Silence handling
- ✅ Edge cases

### ToneMatcherTests (Audio)
Tests for the real-time tone matching engine:
- ✅ Initialization and preparation
- ✅ Processing without profile (bypass mode)
- ✅ Processing with valid profile
- ✅ Different buffer sizes (64 - 2048 samples)
- ✅ Mono and stereo audio
- ✅ NaN/Inf prevention
- ✅ Audio level preservation
- ✅ Edge cases

### FrequencyAnalyzerTests (DSP)
Tests for FFT-based frequency analysis:
- ✅ Initialization and preparation
- ✅ Fundamental frequency detection
- ✅ Low frequency detection (guitar range)
- ✅ Harmonic extraction
- ✅ Silence handling
- ✅ White noise handling
- ✅ Spectrum output
- ✅ Multiple sequential analyses

### ToneProfileTests (Data)
Tests for the ToneProfile data structure:
- ✅ Default construction
- ✅ Validity checks
- ✅ Frequency response storage
- ✅ Harmonic content storage
- ✅ Compression parameter storage
- ✅ EQ band storage
- ✅ Metadata storage
- ✅ Copy construction
- ✅ Realistic data validation

## Running the Tests

### Build and Run Tests
```bash
cd /Users/dan.obrien/code/guitar-sound-mimic

# Build tests (if not already built)
mkdir -p build && cd build
cmake ..
cmake --build . --target GuitarSoundMimicTests

# Run tests
./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
```

### Expected Output
```
=====================================
  Guitar Sound Mimic - Unit Tests   
=====================================

Found 4 test suite(s)

Running tests in category: Audio
  ToneProfiler: Running test 1/9: ToneProfiler initialization
  ToneProfiler: Running test 2/9: ToneProfiler prepare
  ...
  
Running tests in category: DSP
  FrequencyAnalyzer: Running test 1/10: FrequencyAnalyzer initialization
  ...

Running tests in category: Data
  ToneProfile: Running test 1/10: ToneProfile default construction
  ...

=====================================
  Test Results                      
=====================================
ToneProfiler: ✓ ALL PASSED (9 tests)
ToneMatcher: ✓ ALL PASSED (8 tests)
FrequencyAnalyzer: ✓ ALL PASSED (10 tests)
ToneProfile: ✓ ALL PASSED (10 tests)

Total: 37 tests, ALL PASSED ✓
=====================================
```

## Running Tests Automatically During Build

Add to your build script:
```bash
# Build
cmake --build .

# Run tests automatically
if ./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests; then
    echo "✅ All tests passed!"
else
    echo "❌ Some tests failed!"
    exit 1
fi
```

## Continuous Integration

### GitHub Actions Example
```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v2
      - name: Clone JUCE
        run: git clone https://github.com/juce-framework/JUCE.git
      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          cmake --build .
      - name: Run Tests
        run: ./build/GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
```

## Test Structure

Each test suite follows this pattern:

```cpp
class MyTests : public juce::UnitTest
{
public:
    MyTests() : juce::UnitTest("TestName", "Category") {}

    void runTest() override
    {
        beginTest("Test description");
        {
            // Setup
            MyClass obj;
            
            // Execute
            obj.doSomething();
            
            // Verify
            expect(obj.getValue() == expectedValue, "Description");
        }
    }
};

static MyTests myTests;  // Register with test runner
```

## Adding New Tests

1. **Create test file** in `Tests/` directory:
   ```cpp
   #include <JuceHeader.h>
   #include "../Source/MyClass.h"
   
   class MyClassTests : public juce::UnitTest
   {
       // ... implementation
   };
   
   static MyClassTests myClassTests;
   ```

2. **Add to CMakeLists.txt**:
   ```cmake
   target_sources(GuitarSoundMimicTests
       PRIVATE
           Tests/MyClassTests.cpp
           # ... other files
   )
   ```

3. **Rebuild and run**:
   ```bash
   cmake --build .
   ./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
   ```

## Test Categories

Tests are organized into categories:

- **Audio**: Audio processing and analysis (ToneProfiler, ToneMatcher)
- **DSP**: Digital signal processing algorithms (FrequencyAnalyzer)
- **Data**: Data structures and utilities (ToneProfile)

Run specific categories:
```cpp
runner.runTestsInCategory("Audio");   // Only audio tests
runner.runTestsInCategory("DSP");     // Only DSP tests
runner.runTestsInCategory("Data");    // Only data structure tests
```

## Writing Good Tests

### DO:
✅ Test one thing per test case  
✅ Use descriptive test names  
✅ Test edge cases (empty, zero, negative, max)  
✅ Test error conditions  
✅ Make tests independent (no shared state)  
✅ Use meaningful assertion messages  

### DON'T:
❌ Test multiple unrelated things in one test  
❌ Rely on test execution order  
❌ Use magic numbers (document or use constants)  
❌ Ignore edge cases  
❌ Skip assertions (always verify results)  

## Example Test Cases

### Testing Audio Processing
```cpp
beginTest("Process generates valid output");
{
    AudioProcessor processor;
    processor.prepare(44100.0, 512);
    
    AudioBuffer<float> buffer(2, 512);
    // Fill with test data
    
    processor.process(buffer);
    
    // Verify no NaN/Inf
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        const float* data = buffer.getReadPointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            expect(!std::isnan(data[i]), "No NaN");
            expect(!std::isinf(data[i]), "No Inf");
        }
    }
}
```

### Testing Frequency Detection
```cpp
beginTest("Detects 440Hz correctly");
{
    FrequencyAnalyzer analyzer;
    analyzer.prepare(44100.0, 4096);
    
    // Generate 440Hz sine wave
    AudioBuffer<float> buffer(1, 4096);
    // ... fill buffer ...
    
    analyzer.analyze(buffer);
    float detected = analyzer.getFundamental();
    
    // Allow 5% tolerance
    expect(std::abs(detected - 440.0f) < 22.0f, 
           "Detected: " + String(detected) + " Hz");
}
```

## Debugging Failed Tests

If tests fail:

1. **Check the output** - failure messages show what went wrong
2. **Run in debugger**:
   ```bash
   lldb ./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
   ```
3. **Add logging**:
   ```cpp
   DBG("Value: " + String(myValue));
   ```
4. **Isolate the test** - comment out other tests temporarily
5. **Check assumptions** - verify test setup is correct

## Performance Testing

While these are functional tests, you can add performance checks:

```cpp
beginTest("Processing is fast enough");
{
    const int numIterations = 1000;
    auto start = Time::getMillisecondCounterHiRes();
    
    for (int i = 0; i < numIterations; ++i)
    {
        processor.process(buffer);
    }
    
    auto elapsed = Time::getMillisecondCounterHiRes() - start;
    double avgTime = elapsed / numIterations;
    
    // Should process 512 samples in less than 10ms
    expect(avgTime < 10.0, "Average time: " + String(avgTime) + "ms");
}
```

## Test Coverage Goals

Target coverage by component:

- **ToneProfiler**: 90%+ (critical audio analysis)
- **ToneMatcher**: 90%+ (critical real-time processing)
- **FrequencyAnalyzer**: 85%+ (DSP algorithms)
- **ToneProfile**: 95%+ (simple data structure)
- **UI Components**: 50%+ (harder to test, less critical)

## Resources

- [JUCE UnitTest Documentation](https://docs.juce.com/master/classUnitTest.html)
- [JUCE Testing Tutorial](https://docs.juce.com/master/tutorial_unit_tests.html)
- [Catch2 (alternative framework)](https://github.com/catchorg/Catch2)

## Future Test Improvements

- [ ] Add integration tests (full workflow)
- [ ] Add performance benchmarks
- [ ] Add memory leak detection
- [ ] Add thread safety tests
- [ ] Add GUI tests with JUCE's test harness
- [ ] Add audio file comparison tests
- [ ] Add CI/CD pipeline integration





