# Cross encryption/decryption test.
# Builds directly from the real sources in ../scalar, ../v128 and ../v256,
# so it always tests the current code of every implementation.

ifeq ($(OS),Windows_NT)
    DEL = cmd /C del /Q
else
    DEL = rm -f
endif

CC     = gcc
CFLAGS = -O3 -march=native -Wall -I../common -I../scalar -I../v128 -I../v256
TARGET = chacha20-crossed-enc-dec

SCALAR = ../scalar/quarter_round.c ../scalar/state_init.c ../scalar/permute_state.c \
         ../scalar/encrypt.c ../scalar/decrypt.c
V128   = $(addprefix ../v128/, double_whole_round_v128.c state_to_vectors_v128.c \
         permute_state_v128.c vectors_to_state_v128.c encrypt_v128.c decrypt_v128.c)
V256   = $(addprefix ../v256/, double_whole_round_v256.c state_to_vectors_v256.c \
         permute_state_v256.c vectors_to_state_v256.c encrypt_v256.c decrypt_v256.c)

SRCS = main.c $(SCALAR) $(V128) $(V256)
OBJS = $(patsubst %.c,build/%.o,$(notdir $(SRCS)))

vpath %.c . ../scalar ../v128 ../v256

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir build

test: $(TARGET)
	./$(TARGET) --enc-chacha
	./$(TARGET) --enc-chacha128
	./$(TARGET) --enc-chacha256

clean:
	$(DEL) $(TARGET) $(TARGET).exe
	$(DEL) build/*.o

.PHONY: clean test
