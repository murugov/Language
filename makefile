FLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

COMMON_INCLUDES		= -I./COMMON/headers
CONFIG_INCLUDES 	= -I./CONFIG
STK_INCLUDES		= -I./STACK
GEN_INCLUDES		= -I./GENERATOR/headers -I./GENERATOR/src -I./GENERATOR/reports
TREE_INCLUDES		= -I./TREE/headers
HT_INCLUDES			= -I./HASH_TABLE/headers
DUMP_INCLUDES		= -I./DUMP/headers
LEX_INCLUDES		= -I./LEXER/headers
WOLFRAM_INCLUDES    = -I./WOLFRAM_SIGMA/headers
ASM_INCLUDES  		= -I./ASSEMBLER/headers
SPU_INCLUDES  		= -I./SPU/headers

COMMON_FILES  = COMMON/GetHash.cpp COMMON/IsBadPtr.cpp COMMON/LineCounter.cpp COMMON/logger.cpp COMMON/SizeFile.cpp COMMON/TXTreader.cpp COMMON/math_func.cpp COMMON/is_zero.cpp COMMON/Factorial.cpp
TREE_FILES 	  = TREE/TreeFunc.cpp
HT_FILES	  = HASH_TABLE/HashTableFunc.cpp
DUMP_FILES	  = DUMP/GenGraphs.cpp 
LEX_FILES	  = LEXER/lexer.cpp LEXER/token.cpp LEXER/parser.cpp
WOLFRAM_FILES = WOLFRAM_SIGMA/VerifyInstrSet.cpp WOLFRAM_SIGMA/WolfFunc.cpp WOLFRAM_SIGMA/CalcFunc.cpp WOLFRAM_SIGMA/SimplifyTree.cpp WOLFRAM_SIGMA/CalcExpression.cpp WOLFRAM_SIGMA/parseWolfTree.cpp
# ASM_FILES 	  = ASSEMBLER/ArrPtrCtor.cpp ASSEMBLER/AsmArgParser.cpp ASSEMBLER/AsmErrPrint.cpp ASSEMBLER/AsmVerifySort.cpp ASSEMBLER/CodeWriter.cpp ASSEMBLER/Compiler.cpp ASSEMBLER/HashCmd.cpp ASSEMBLER/OpcodeReader.cpp ASSEMBLER/SpecTxtReader.cpp
# SPU_FILES 	  = SPU/CalcFunc.cpp SPU/spuCtor.cpp SPU/spuErrPrint.cpp SPU/spuExecutor.cpp SPU/SpuVerifySort.cpp SPU/SpuArgParser.cpp

DEFAULT_SRC   ?=  src/data.txt
DEFAULT_LATEX ?=  reports/LatexDump.tex


all: help

wolf: WOLFRAM_SIGMA/main_wolf.cpp $(COMMON_FILES) $(LEX_FILES) $(TREE_FILES) $(HT_FILES) $(DUMP_FILES) $(WOLFRAM_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o wolf_program $(FLAGS) WOLFRAM_SIGMA/main_wolf.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) $(GEN_INCLUDES) \
	$(STK_INCLUDES) $(TREE_INCLUDES) $(HT_INCLUDES) $(DUMP_INCLUDES) $(LEX_INCLUDES) $(WOLFRAM_INCLUDES) \
	$(COMMON_FILES) $(LEX_FILES) $(TREE_FILES) $(HT_FILES) $(DUMP_FILES) $(WOLFRAM_FILES)
	@echo "-----------------------------------------------------------------------------------------"

gen: GENERATOR/main_gen.cpp $(COMMON_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o gen_program $(FLAGS) GENERATOR/main_gen.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) $(STK_INCLUDES) $(GEN_INCLUDES) $(COMMON_FILES)
	@echo "-----------------------------------------------------------------------------------------"

lex: LEXER/main_lex.cpp $(COMMON_FILES) $(TREE_FILES) $(HT_FILES) $(DUMP_FILES) $(LEX_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o lex_program $(FLAGS) LEXER/main_lex.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
	$(STK_INCLUDES) $(TREE_INCLUDES) $(HT_INCLUDES) $(DUMP_INCLUDES) $(GEN_INCLUDES) $(LEX_INCLUDES) \
	$(COMMON_FILES) $(TREE_FILES) $(HT_FILES) $(DUMP_FILES) $(LEX_FILES)
	@echo "-----------------------------------------------------------------------------------------"


run-wolf: wolf
	./wolf_program $(DEFAULT_SRC) $(DEFAULT_LATEX)

run-wolf-args: wolf
	@if [ "$(ARGS)" = "" ]; then \
		echo "Usage: make run-wolf-args ARGS=\"data.txt latex.tex\""; \
	fi
	./wolf_program $(ARGS)

run-gen: gen
	./gen_program

run-lex: lex
	./lex_program

run: run-wolf

clean:
	rm -f wolf_program gen_program lex_program

help:
	@echo "Available commands:"
	@echo ""
	@echo "  make wolf                     - compile a wolfram"
	@echo "  make run-wolf                 - compile and run wolfram"
	@echo "  make run                      - compile and run wolfram"
	@echo ""
	@echo "  make clean                    - remove compiled programs"

.PHONY: wolf gen run-wolf run-wolf-args run-gen run-lex run clean help