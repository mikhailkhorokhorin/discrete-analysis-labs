REPORT_LABS := lab1 lab2 lab3 lab4 lab5
REPORT_BUILD_DIR := $(CURDIR)/build/report
REPORT_ARTIFACTS_DIR := $(CURDIR)/build/artifacts/reports
LATEXMK := latexmk -pdf -interaction=nonstopmode -halt-on-error
RESEARCH_DIR := $(CURDIR)/build/artifacts/lab3
RESEARCH_WORDS ?= 100000

CI_EXTRA := reports

.PHONY: reports report research

reports: $(addprefix report-,$(REPORT_LABS))

report:
	@test -n "$(LAB)" || (echo "LAB is required" >&2; exit 2)
	$(MAKE) report-$(LAB)

report-%:
	mkdir -p $(REPORT_BUILD_DIR)/$* $(REPORT_ARTIFACTS_DIR)
	cd $*/report && epoch=$$(git log -1 --format=%ct -- . 2>/dev/null) && \
		SOURCE_DATE_EPOCH=$${epoch:-$$(date +%s)} FORCE_SOURCE_DATE=1 \
		$(LATEXMK) -outdir=$(REPORT_BUILD_DIR)/$* report.tex
	cp $(REPORT_BUILD_DIR)/$*/report.pdf $*/report/report.pdf
	cp $(REPORT_BUILD_DIR)/$*/report.pdf $(REPORT_ARTIFACTS_DIR)/$*.pdf

research:
	cmake --preset release
	cmake --build --preset release --target lab3
	python3 lab3/benchmark/generator.py $(RESEARCH_WORDS) 16 42
	mkdir -p $(RESEARCH_DIR)
	cd $(RESEARCH_DIR) && $(CURDIR)/build/release/lab3/lab3_profile < $(CURDIR)/lab3/benchmark/tests.txt
	gprof $(CURDIR)/build/release/lab3/lab3_profile $(RESEARCH_DIR)/gmon.out > $(RESEARCH_DIR)/gprof.txt
	valgrind --leak-check=full --error-exitcode=1 --log-file=$(RESEARCH_DIR)/memcheck.txt \
		$(CURDIR)/build/release/lab3/lab3_main < lab3/benchmark/tests.txt > /dev/null
	valgrind --tool=massif --massif-out-file=$(RESEARCH_DIR)/massif.out \
		$(CURDIR)/build/release/lab3/lab3_main < lab3/benchmark/tests.txt > /dev/null
	ms_print $(RESEARCH_DIR)/massif.out > $(RESEARCH_DIR)/massif.txt
