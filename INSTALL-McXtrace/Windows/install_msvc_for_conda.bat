@echo off
:: =============================================================================
:: install_msvc_for_conda.bat
::
:: PURPOSE
::   Install VS 2022 Build Tools + MSVC toolchain so that a conda environment
::   containing the conda-forge "c-compiler" meta-package works out of the box.
::
:: REQUIREMENTS
::   - Run as Administrator (right-click -> "Run as administrator")
::   - winget must be available (Windows 10 1809+ / Windows 11)
::
:: BACKGROUND
::   conda-forge moved its default Windows compiler from VS2019 to VS2022 in
::   June 2025.  It cannot redistribute MSVC itself, so only activation scripts
::   are shipped; the compiler must be installed separately on the host.
::
::   Target scenario: ARM64 Windows VM running x86_64 (AMD64) conda.
::   The script installs:
::     - MSVC v143  x64/x86 toolset   (what conda-forge/c-compiler activates)
::     - MSVC v143  ARM64  toolset     (native ARM64 cl.exe for the host)
::     - MSVC v143  ARM64EC toolset    (needed for some hybrid ARM64 scenarios)
::     - VC Redistributable 14.x       (VCRUNTIME140.DLL, MSVCP140.DLL, etc.)
::     - Windows 11 SDK                (headers, libs, rc.exe, mt.exe, ...)
::     - VCTools workload              (MSBuild integration, CMake, Ninja)
::     - CoreBuildTools                (MSBuild, common targets)
::
:: WHAT "VCTools workload + includeRecommended" pulls in automatically:
::   Microsoft.VisualStudio.Component.VC.Tools.x86.x64
::   Microsoft.VisualStudio.Component.VC.Redist.14.Latest
::   Microsoft.VisualStudio.Component.Windows11SDK.22621 (or newer)
::   Microsoft.VisualStudio.Component.VC.CMake.Project
::   Microsoft.VisualStudio.Component.TestTools.BuildTools
::   Microsoft.VisualStudio.Component.VC.ASAN
::   ... plus MSBuild core
::
:: USAGE
::   1. Open cmd.exe as Administrator
::   2. install_msvc_for_conda.bat
::
::   After completion, verify in a new shell:
::     conda create -n test_c c-compiler -c conda-forge
::     conda activate test_c
::     cl /?
:: =============================================================================

setlocal EnableExtensions DisableDelayedExpansion

:: ---------------------------------------------------------------------------
:: 0. Admin check
:: ---------------------------------------------------------------------------
echo McCode helper-script for installation of MSVC and utilities
echo .
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING:
    echo .
    echo Your user is not Administrator
    echo .
    echo If possible, please attempt a rerun of this script as Administrator,
    echo         Right-click the file and choose "Run as administrator".
    echo .
    echo Use ^C to cancel / use any other key to attempt with current permissions
    pause
    echo ... Proceeding with current permissions...
)

echo.
echo ============================================================
echo  Installing VS 2022 Build Tools for conda-forge c-compiler
echo ============================================================
echo.

:: ---------------------------------------------------------------------------
:: 1. Detect whether winget is available
:: ---------------------------------------------------------------------------
where winget >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] winget not found.  Install the "App Installer" package from
    echo         the Microsoft Store, or upgrade to Windows 10 1809+ / Windows 11.
    pause
    exit /b 1
)

for /f "tokens=* usebackq" %%v in (`winget --version`) do set WINGET_VER=%%v
echo [OK] winget found: %WINGET_VER%

:: ---------------------------------------------------------------------------
:: 2. Build the component list
::
::    Core components (always installed):
::      Microsoft.VisualStudio.Workload.VCTools          – C++ build workload
::      Microsoft.VisualStudio.Component.VC.Tools.x86.x64 – MSVC x64/x86
::      Microsoft.VisualStudio.Component.VC.Redist.14.Latest – vc_redist DLLs
::      Microsoft.VisualStudio.Component.CoreBuildTools  – MSBuild core
::      Microsoft.VisualStudio.Component.Windows11SDK.26100 – latest Win11 SDK
::
::    ARM64 host additions (detected automatically):
::      Microsoft.VisualStudio.Component.VC.Tools.ARM64  – native ARM64 cl.exe
::      Microsoft.VisualStudio.Component.VC.Tools.ARM64EC – ARM64EC toolset
:: ---------------------------------------------------------------------------

:: Base override string (--wait blocks until installer exits; --passive shows
:: progress UI without requiring user interaction)
set OVERRIDE=--wait --passive --norestart

:: Core workload + individual components
set OVERRIDE=%OVERRIDE% --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended
set OVERRIDE=%OVERRIDE% --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64
set OVERRIDE=%OVERRIDE% --add Microsoft.VisualStudio.Component.VC.Redist.14.Latest
set OVERRIDE=%OVERRIDE% --add Microsoft.VisualStudio.Component.CoreBuildTools
:: Latest Windows 11 SDK (26100 == 10.0.26100, shipped with VS 17.x)
set OVERRIDE=%OVERRIDE% --add Microsoft.VisualStudio.Component.Windows11SDK.26100

:: ARM64 host detection: add ARM64 toolsets so the host can also build natively
if /i "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
    echo [INFO] ARM64 host detected – adding ARM64 and ARM64EC toolsets.
    set OVERRIDE=%OVERRIDE% --add Microsoft.VisualStudio.Component.VC.Tools.ARM64
    set OVERRIDE=%OVERRIDE% --add Microsoft.VisualStudio.Component.VC.Tools.ARM64EC
)

:: ---------------------------------------------------------------------------
:: 3. Install VS 2022 Build Tools
::
::    --accept-package-agreements / --accept-source-agreements skip the
::    interactive consent prompts so the script can run unattended.
:: ---------------------------------------------------------------------------
echo.
echo [STEP 1/2] Installing Visual Studio 2022 Build Tools ...
echo           (This downloads ~2 to 4 GB and may take 10 to 30 minutes.)
echo.

winget install ^
    --id Microsoft.VisualStudio.2022.BuildTools ^
    --accept-package-agreements ^
    --accept-source-agreements ^
    --override "%OVERRIDE%"

set VS_EXIT=%errorlevel%

:: winget/VS installer exit code 3010 = success, reboot recommended (not required)
if %VS_EXIT% equ 0 (
    echo [OK] VS 2022 Build Tools installed successfully.
) else if %VS_EXIT% equ 3010 (
    echo [OK] VS 2022 Build Tools installed successfully.
    echo      A reboot is recommended but not strictly required for conda use.
) else (
    echo [ERROR] VS installer returned exit code %VS_EXIT%.
    echo         Check %TEMP%\dd_*.log for details.
    pause
    exit /b %VS_EXIT%
)

:: ---------------------------------------------------------------------------
:: 4. Install / update the VC++ Redistributable packages
::
::    These ship the DLLs (VCRUNTIME140.DLL, MSVCP140.DLL, etc.) into
::    System32/SysWOW64, making them available for conda-packaged binaries
::    that were compiled with MSVC but do not bundle the runtime themselves.
:: ---------------------------------------------------------------------------
echo.
echo [STEP 2/2] Installing Visual C++ Redistributable (x64 + x86) ...
echo.

:: x64 redistributable
winget install ^
    --id Microsoft.VCRedist.2015+.x64 ^
    --accept-package-agreements ^
    --accept-source-agreements ^
    --silent

if %errorlevel% neq 0 if %errorlevel% neq 3010 (
    echo [WARN] x64 VC Redist installer returned %errorlevel%.
    echo        The compiler itself is still usable; only pre-built binaries
    echo        that depend on the system redist may be affected.
)

:: x86 redistributable (needed for 32-bit conda packages and some tools)
winget install ^
    --id Microsoft.VCRedist.2015+.x86 ^
    --accept-package-agreements ^
    --accept-source-agreements ^
    --silent

if %errorlevel% neq 0 if %errorlevel% neq 3010 (
    echo [WARN] x86 VC Redist installer returned %errorlevel%.
)

echo.
echo [OK] VC++ Redistributables installed.

:: ---------------------------------------------------------------------------
:: 5. Summary and next steps
:: ---------------------------------------------------------------------------
echo.
echo ============================================================
echo  Installation complete.
echo ============================================================
echo.
echo  Installed components:
echo    - MSVC v143  x64/x86 toolset   (cl.exe, link.exe, lib.exe, ...)
echo    - Windows 11 SDK 10.0.26100    (headers, rc.exe, mt.exe, ...)
echo    - VC++ Redistributable 2015+   (VCRUNTIME140.DLL, MSVCP140.DLL, ...)
echo    - MSBuild, CMake, Ninja        (via VCTools workload recommended set)
if /i "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
echo    - MSVC v143  ARM64  toolset     (native ARM64 build support)
echo    - MSVC v143  ARM64EC toolset
)
echo.
echo  Next steps:
echo    1. Open a NEW terminal (the PATH is updated by the installer).
echo    2. Create and test a conda environment:
echo.
echo       conda create -n myenv python c-compiler -c conda-forge
echo       conda activate myenv
echo       cl /?
echo.
echo    3. If you use conda-build or cmake in the env, also consider:
echo       conda install cmake ninja -c conda-forge
echo.
echo    NOTE: The conda-forge "c-compiler" package on Windows is a meta-package
echo    that only provides activation scripts (vcvarsall.bat wrappers).  The
echo    actual compiler lives in:
echo      %%ProgramFiles(x86)%%\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\
echo.

pause
endlocal
exit /b 0
