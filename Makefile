# ChaCha20: scalar, 128-bit and 256-bit SIMD implementations.
#
#   make                  build everything into build/
#   make test             RFC 8439 vectors for each implementation + cross test
#   make bench V=v128     clock-cycle benchmark for one implementation (CSV in results/)
#   make clean

CC      ?= gcc
CFLAGS  ?= -O3 -march=native -Wall -Wextra -flto
override CFLAGS += -Isrc -Itest
LDFLAGS ?= -flto

IMPLS = scalar v128 v256
BUILD = build

ifeq ($(OS),Windows_NT)
    EXE = .exe
endif

COMMON_SRC = src/chacha20_common.c
TEST_SRC   = test/test_vectors.c test/custom_input.c

BINS = $(foreach i,$(IMPLS),$(BUILD)/chacha20-$(i)$(EXE)) $(BUILD)/chacha20-cross$(EXE)

all: $(BINS)

# One test binary per implementation: same driver, different -DIMPL.
$(BUILD)/chacha20-%$(EXE): test/main.c src/chacha20_%.c $(COMMON_SRC) $(TEST_SRC) src/chacha20.h test/test_vectors.h | $(BUILD)
	$(CC) $(CFLAGS) -DIMPL=$* test/main.c src/chacha20_$*.c $(COMMON_SRC) $(TEST_SRC) $(LDFLAGS) -o $@

# Cross test links all three implementations together.
$(BUILD)/chacha20-cross$(EXE): test/cross.c $(foreach i,$(IMPLS),src/chacha20_$(i).c) $(COMMON_SRC) test/test_vectors.c src/chacha20.h | $(BUILD)
	$(CC) $(CFLAGS) test/cross.c $(foreach i,$(IMPLS),src/chacha20_$(i).c) $(COMMON_SRC) test/test_vectors.c $(LDFLAGS) -o $@

$(BUILD):
	mkdir $(BUILD)

test: all
	@for i in $(IMPLS); do echo "== $$i"; ./$(BUILD)/chacha20-$$i$(EXE) --all-tests | grep -E "tests passed" || exit 1; \
		./$(BUILD)/chacha20-$$i$(EXE) --all-tests > /dev/null || { echo "FAIL: $$i"; exit 1; }; done
	@echo "== cross"
	@./$(BUILD)/chacha20-cross$(EXE)

V ?= scalar
bench: $(BUILD)/chacha20-$(V)$(EXE)
	./bench/clock_cycles_tests.sh ./$(BUILD)/chacha20-$(V)$(EXE) results/generated_results_$(V).csv

clean:
	rm -rf $(BUILD)

.PHONY: all test bench clean
