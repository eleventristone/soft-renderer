@REM chcp 65001
conan install . --output-folder=build --build=missing

@REM debug mode
@REM conan install . --output-folder=build --build=missing --settings build_type=Debug 
cd build

cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=RELEASE

@REM release mode with gprof
@REM cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_FLAGS=-pg

@REM debug mode
@REM cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=DEBUG
cmake --build .
main