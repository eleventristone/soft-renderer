conan install . --output-folder=build --build=missing # release mode
# conan install . --output-folder=build --build=missing --settings build_type=Debug # debug mode
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=RELEASE # release mode
# cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_FLAGS=-pg # release mode with gprof
# cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=DEBUG # debug mode
cmake --build .

read -p "Press any key to execute the executable program..." -n1 -s
echo

./main