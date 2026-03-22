@echo off
REM Setup script for Guitar Sound Mimic (Windows)

echo.
echo 🎸 Guitar Sound Mimic - Setup Script
echo ====================================
echo.

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ❌ Error: CMake is not installed.
    echo.
    echo Please install CMake from https://cmake.org/download/
    echo Or use: choco install cmake
    exit /b 1
)

echo ✅ CMake found
echo.

REM Clone JUCE if not already present
if not exist "JUCE" (
    echo 📦 Cloning JUCE framework...
    git clone --depth 1 https://github.com/juce-framework/JUCE.git
    echo ✅ JUCE cloned successfully
) else (
    echo ✅ JUCE already exists
)
echo.

REM Create build directory
if not exist "build" (
    echo 📁 Creating build directory...
    mkdir build
    echo ✅ Build directory created
) else (
    echo ✅ Build directory already exists
)
echo.

REM Run CMake
echo ⚙️  Configuring CMake...
cd build
cmake ..
if %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake configuration failed
    exit /b 1
)
echo ✅ CMake configuration complete
echo.

REM Build
echo 🔨 Building project...
echo    (This may take a few minutes...)
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo ❌ Build failed
    exit /b 1
)
echo ✅ Build complete!
echo.

REM Success message
echo ==========================================
echo ✨ Setup complete! ✨
echo.
echo To run the application:
echo   .\build\GuitarSoundMimic_artefacts\Release\GuitarSoundMimic.exe
echo.
echo See README.md for usage instructions.
echo ==========================================

pause





