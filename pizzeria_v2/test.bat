@echo off

REM Путь к CLion и CMake
set "CLION_PATH=D:\CLion2024.2.2\bin"
set "CMAKE_PATH=%CLION_PATH%\cmake\win\x64\bin\cmake.exe"
set "NINJA_PATH=%CLION_PATH%\ninja\win\x64\ninja.exe"

REM Путь к проекту
set "PROJECT_PATH=D:\BSUIR\course2\PPOIS\pizzeria_v2"
set "BUILD_PATH=%PROJECT_PATH%\cmake-build-debug-coverage"

REM Создание новой директории сборки, если она не существует
if not exist "%BUILD_PATH%" (
    mkdir "%BUILD_PATH%"
)

REM Генерация файлов сборки с использованием CMake
"%CMAKE_PATH%" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=%NINJA_PATH% -G Ninja -S "%PROJECT_PATH%" -B "%BUILD_PATH%" -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_C_FLAGS="--coverage"

REM Компиляция проекта с использованием Ninja
"%NINJA_PATH%" -C "%BUILD_PATH%" --target Google_test_run

echo Процесс сборки завершен.

REM Установите переменные окружения для покрытия
set LLVM_PROFILE_FILE=%BUILD_PATH%\default.profraw

REM Запуск тестов с покрытием
"%BUILD_PATH%\Google_tests\Google_test_run.exe" --gtest_color=no

REM Генерация отчета о покрытии кода с использованием gcovr
gcovr -r "%PROJECT_PATH%" --html --html-details -o "%BUILD_PATH%\coverage.html"

echo Отчет о покрытии кода создан в %BUILD_PATH%\coverage.html.
pause
