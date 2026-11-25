emulator: main.cpp cpu.cpp memory.cpp bin_file.cpp
	g++ main.cpp cpu.cpp memory.cpp bin_file.cpp -o emulator 

test1: emulator
	$(MAKE) test_cases/01_case
	./emulator test_cases/01_case/test.bin

test2: emulator
	$(MAKE) test_cases/02_case
	./emulator test_cases/02_case/test.bin

test3: emulator
	$(MAKE) test_cases/03_case
	./emulator test_cases/03_case/test.bin


