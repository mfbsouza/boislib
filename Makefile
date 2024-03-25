# project name
PROJECT = boislib

# paths
TESTDIR = ./tests

# targets
.PHONY: tests clean coverage

tests:
	make -C $(TESTDIR) run_tests

clean:
	make -C $(TESTDIR) clean

coverage:
	make -C $(TESTDIR) coverage

format:
	find . \
		-regex '.*\.\(cpp\|hpp\|cc\|cxx\|c\|h\)' \
		-exec clang-format -style=file -i {} \;
