RELEASE_DIR := $(CURDIR)/build/release
RESEARCH_DIR := $(CURDIR)/build/artifacts/lab3
RESEARCH_WORDS ?= 100000
RESEARCH_INPUT := $(CURDIR)/lab2/benchmark/tests.txt

.PHONY: bench research

bench:
	@test -n "$(LAB)" || (echo "LAB is required" >&2; exit 2)
	@test -f $(LAB)/benchmark/run.sh || (echo "$(LAB) has no benchmark" >&2; exit 2)
	cmake --preset release
	cmake --build --preset release --target $(LAB)_benchmark
	bash $(LAB)/benchmark/run.sh $(RELEASE_DIR)/$(LAB)/$(LAB)_benchmark

research:
	cmake --preset release
	cmake --build --preset release --target lab3
	python3 lab2/benchmark/generator.py $(RESEARCH_WORDS) 16 42
	mkdir -p $(RESEARCH_DIR)
	cd $(RESEARCH_DIR) && $(RELEASE_DIR)/lab3/lab3_profile < $(RESEARCH_INPUT)
	gprof $(RELEASE_DIR)/lab3/lab3_profile $(RESEARCH_DIR)/gmon.out > $(RESEARCH_DIR)/gprof.txt
	valgrind --leak-check=full --error-exitcode=1 --log-file=$(RESEARCH_DIR)/memcheck.txt \
		$(RELEASE_DIR)/lab3/lab3_main < $(RESEARCH_INPUT) > /dev/null
	valgrind --tool=massif --massif-out-file=$(RESEARCH_DIR)/massif.out \
		$(RELEASE_DIR)/lab3/lab3_main < $(RESEARCH_INPUT) > /dev/null
	ms_print $(RESEARCH_DIR)/massif.out > $(RESEARCH_DIR)/massif.txt
