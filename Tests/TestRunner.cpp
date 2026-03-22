#include <JuceHeader.h>

//==============================================================================
/**
 * Console application that runs all unit tests.
 */
class TestRunnerApplication : public juce::JUCEApplication
{
public:
    TestRunnerApplication() {}

    const juce::String getApplicationName() override { return "GuitarSoundMimic Tests"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);

        // Get all registered unit tests
        juce::Array<juce::UnitTest*> tests = juce::UnitTest::getAllTests();
        
        std::cout << "\n";
        std::cout << "=====================================" << std::endl;
        std::cout << "  Guitar Sound Mimic - Unit Tests   " << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "\n";
        std::cout << "Found " << tests.size() << " test suite(s)\n" << std::endl;

        // Create a test runner
        juce::UnitTestRunner runner;
        
        // Run all tests
        runner.runTestsInCategory("Audio");
        runner.runTestsInCategory("DSP");
        runner.runTestsInCategory("Data");
        
        // Print summary
        std::cout << "\n";
        std::cout << "=====================================" << std::endl;
        std::cout << "  Test Results                      " << std::endl;
        std::cout << "=====================================" << std::endl;
        
        int totalTests = 0;
        int totalFailures = 0;
        
        for (auto* test : tests)
        {
            int numTests = test->getResults().size();
            int numFailures = 0;
            
            for (auto& result : test->getResults())
            {
                if (result.failures > 0)
                    numFailures++;
            }
            
            totalTests += numTests;
            totalFailures += numFailures;
            
            std::cout << test->getName() << ": ";
            
            if (numFailures == 0)
                std::cout << "✓ ALL PASSED (" << numTests << " tests)" << std::endl;
            else
                std::cout << "✗ " << numFailures << " FAILED (of " << numTests << " tests)" << std::endl;
        }
        
        std::cout << "\n";
        std::cout << "Total: " << totalTests << " tests, ";
        
        if (totalFailures == 0)
        {
            std::cout << "ALL PASSED ✓" << std::endl;
        }
        else
        {
            std::cout << totalFailures << " FAILED ✗" << std::endl;
        }
        
        std::cout << "=====================================" << std::endl;
        std::cout << "\n";

        // Return appropriate exit code
        setApplicationReturnValue(totalFailures == 0 ? 0 : 1);
        quit();
    }

    void shutdown() override
    {
        // Cleanup if needed
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
    }
};

//==============================================================================
START_JUCE_APPLICATION(TestRunnerApplication)





