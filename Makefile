cc = gcc

default: assembler interpreter

assembler:
	$(cc) assembler.c -o assembler

debug_asm:
	$(cc) -g -Wall assembler.c -o assembler

interpreter:
	$(cc) interpreter.c -o interpreter

debug_int:
	$(cc) -g -Wall interpreter.c -o interpreter

