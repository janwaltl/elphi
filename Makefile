.PHONY: all debug release coverage junit test format sca docs clean

all: debug test

COMPILER=-DCMAKE_CXX_COMPILER=${CXX}

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug ${COMPILER}
	cmake --build build/debug -- -j`nproc`
	cp build/debug/compile_commands.json build/

release:
	cmake -DCMAKE_BUILD_TYPE=ReleaseWithDebInfo -S . -B build/release ${COMPILER}
	cmake --build build/release -- -j`nproc`
	cp build/release/compile_commands.json build/

coverage:
	cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/coverage -DELPHI_ENABLE_COVERAGE=ON  ${COMPILER} -DELPHI_BUILD_TEST=ON
	cmake --build build/coverage -- -j`nproc`
	scripts/gen_coverage.sh build/coverage

junit:
	cmake -DCMAKE_BUILD_TYPE=Release -DELPHI_JUNIT_TEST_OUTPUT=ON  ${COMPILER} -S . -B build/junit -DELPHI_BUILD_TEST=ON
	cmake --build build/junit -- -j`nproc`
	scripts/run_unit.sh build/junit

test:
	cmake -DCMAKE_BUILD_TYPE=ReleaseWithDebInfo ${COMPILER} -S . -B build/release -DELPHI_BUILD_TEST=ON
	cmake --build build/release -- -j`nproc`
	scripts/run_tests.sh build/release

format:
	scripts/format_codebase.sh

sca: debug
	./scripts/filter_libs.sh build/compile_commands.json
	run-clang-tidy -p build/

docs:
	doxygen

clean:
	rm -rf build
