# Detect the Operating System
ifeq ($(OS),Windows_NT)
    DEL = cmd /C del /Q
else
    DEL = rm -f
endif

CC = gcc
CFLAGS = -mavx512f -O3 -Wall -c -Iheaders
TARGET = chacha20-crossed-enc-dec
OBJS = decrypt_v128.o decrypt_v256.o decrypt.o \
       double_whole_round_v128.o double_whole_round_v256.o \
       encrypt_v128.o encrypt_v256.o encrypt.o \
       permute_state_v128.o permute_state_v256.o permute_state.o \
       quarter_round.o state_init.o \
       state_to_vectors_v128.o state_to_vectors_v256.o \
       vectors_to_state_v128.o vectors_to_state_v256.o \
       main.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -mavx512f -O3 -Wall -o $(TARGET)

decrypt_v128.o: decrypt_v128.c
	$(CC) $(CFLAGS) decrypt_v128.c

decrypt_v256.o: decrypt_v256.c
	$(CC) $(CFLAGS) decrypt_v256.c

decrypt.o: decrypt.c
	$(CC) $(CFLAGS) decrypt.c

double_whole_round_v128.o: double_whole_round_v128.c
	$(CC) $(CFLAGS) double_whole_round_v128.c

double_whole_round_v256.o: double_whole_round_v256.c
	$(CC) $(CFLAGS) double_whole_round_v256.c

encrypt_v128.o: encrypt_v128.c
	$(CC) $(CFLAGS) encrypt_v128.c

encrypt_v256.o: encrypt_v256.c
	$(CC) $(CFLAGS) encrypt_v256.c

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) encrypt.c

permute_state_v128.o: permute_state_v128.c
	$(CC) $(CFLAGS) permute_state_v128.c

permute_state_v256.o: permute_state_v256.c
	$(CC) $(CFLAGS) permute_state_v256.c

permute_state.o: permute_state.c
	$(CC) $(CFLAGS) permute_state.c

quarter_round.o: quarter_round.c
	$(CC) $(CFLAGS) quarter_round.c

state_init.o: state_init.c
	$(CC) $(CFLAGS) state_init.c

state_to_vectors_v128.o: state_to_vectors_v128.c
	$(CC) $(CFLAGS) state_to_vectors_v128.c

state_to_vectors_v256.o: state_to_vectors_v256.c
	$(CC) $(CFLAGS) state_to_vectors_v256.c

vectors_to_state_v128.o: vectors_to_state_v128.c
	$(CC) $(CFLAGS) vectors_to_state_v128.c

vectors_to_state_v256.o: vectors_to_state_v256.c
	$(CC) $(CFLAGS) vectors_to_state_v256.c

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean: 
	$(DEL) $(TARGET) $(OBJS) chacha20-crossed-enc-dec.exe

.PHONY: clean
