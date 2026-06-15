@echo off
chcp 65001 >nul
echo ===================================================
echo [XMAKE] Clear old cache and generate VS-Project...
echo ===================================================

if exist vsxmake (
    echo Delete old vsxmake folder...
    rmdir /s /q vsxmake
)

call xmake project -k vsxmake -m "debug,release,releasedbg"

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Projectgeneration failed!
    pause
    exit /b %errorlevel%
)

echo.
echo [OK] Project build!
echo.
pause