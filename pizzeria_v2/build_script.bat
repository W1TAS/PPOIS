@echo off

REM Путь к CLion и CMake
set "CLION_PATH=D:\CLion2024.2.2\bin"
set "CMAKE_PATH=%CLION_PATH%\cmake\win\x64\bin\cmake.exe"
set "NINJA_PATH=%CLION_PATH%\ninja\win\x64\ninja.exe"

REM Путь к проекту
set "PROJECT_PATH=D:\BSUIR\course2\PPOIS\pizzeria_v2"
set "BUILD_PATH=%PROJECT_PATH%\cmake-build-debug"

REM Создание новой директории сборки, если она не существует
if not exist "%BUILD_PATH%" (
    mkdir "%BUILD_PATH%"
)

REM Генерация файлов сборки с использованием CMake
"%CMAKE_PATH%" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=%NINJA_PATH% -G Ninja -S "%PROJECT_PATH%" -B "%BUILD_PATH%"

REM Компиляция проекта с использованием Ninja
"%NINJA_PATH%" -C "%BUILD_PATH%"

echo Процесс сборки завершен.
pause
