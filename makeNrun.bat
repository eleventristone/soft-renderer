@REM chcp 65001
conan install . --output-folder=build --build=missing
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=RELEASE
cmake --build .
main