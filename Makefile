.PHONY: build run clean check lint

build:
	cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build build

run: build
	./build/lab1/lab1_main

clean:
	rm -rf build

check:
	valgrind --leak-check=full --log-file=valgrind_report.txt ./build/lab1/lab1_main

lint:
	find . -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i -style=file
