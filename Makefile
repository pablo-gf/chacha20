# Top-level Makefile: builds and tests every ChaCha20 implementation.
#
#   make            build everything
#   make scalar     build one implementation (scalar | v128 | v256 | cross)
#   make test       RFC 8439 test vectors for each version + cross enc/dec test
#   make bench V=v128   run the clock-cycle benchmark for one version
#   make clean

IMPLS = scalar v128 v256

all: $(IMPLS) cross

$(IMPLS):
	$(MAKE) -C $@

cross: $(IMPLS)
	$(MAKE) -C cross-enc-dec

test: all
	@for d in $(IMPLS); do echo "== $$d"; ./common/run_test_vectors.sh $$d || exit 1; done
	@echo "== cross-enc-dec"
	@cd cross-enc-dec && for o in --enc-chacha --enc-chacha128 --enc-chacha256; do \
		./chacha20-crossed-enc-dec $$o | grep -q "test working" \
		&& echo "  ok    $$o" || { echo "  FAIL  $$o"; exit 1; }; done

V ?= scalar
bench:
	cd $(V) && ../common/clock_cycles_tests.sh

clean:
	@for d in $(IMPLS) cross-enc-dec; do $(MAKE) -s -C $$d clean; done

.PHONY: all $(IMPLS) cross test bench clean
