FLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

COMMON_INCLUDES		= -I./COMMON/headers
CONFIG_INCLUDES 	= -I./CONFIG
STK_INCLUDES		= -I./STACK
TREE_INCLUDES		= -I./TREE/headers
HT_INCLUDES			= -I./HASH_TABLE
GEN_INCLUDES		= -I./GENERATOR/headers -I./GENERATOR/src -I./GENERATOR/reports
DUMP_INCLUDES		= -I./DUMP/headers
FRONT_INCLUDES		= -I./FRONTEND/headers
MIDLE_INCLUDES		= -I./MIDLEEND/headers
BACK_INCLUDES		= -I./BACKEND/headers


COMMON_FILES  = COMMON/GetHash.cpp COMMON/IsBadPtr.cpp COMMON/LineCounter.cpp COMMON/logger.cpp COMMON/SizeFile.cpp COMMON/TXTreader.cpp COMMON/math_func.cpp COMMON/is_zero.cpp COMMON/Factorial.cpp
TREE_FILES 	  = TREE/TreeFunc.cpp
DUMP_FILES	  = DUMP/GenGraphs.cpp 
FRONT_FILES	  = FRONTEND/lexer.cpp FRONTEND/token.cpp FRONTEND/parser.cpp FRONTEND/PrintAST.cpp
BACK_FILES	  = BACKEND/DataReader.cpp BACKEND/translator.cpp


all: help

gen: GENERATOR/main_gen.cpp $(COMMON_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o gen_program $(FLAGS) GENERATOR/main_gen.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) $(STK_INCLUDES) $(GEN_INCLUDES) $(COMMON_FILES)
	@echo "-----------------------------------------------------------------------------------------"

front: FRONTEND/main_front.cpp $(COMMON_FILES) $(TREE_FILES) $(DUMP_FILES) $(FRONT_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o front_program $(FLAGS) FRONTEND/main_front.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
	$(STK_INCLUDES) $(TREE_INCLUDES) $(HT_INCLUDES) $(DUMP_INCLUDES) $(GEN_INCLUDES) $(FRONT_INCLUDES) \
	$(COMMON_FILES) $(TREE_FILES) $(DUMP_FILES) $(FRONT_FILES)
	@echo "-----------------------------------------------------------------------------------------"

back: BACKEND/main_back.cpp $(COMMON_FILES) $(TREE_FILES) $(DUMP_FILES) $(BACK_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o back_program $(FLAGS) BACKEND/main_back.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
	$(STK_INCLUDES) $(TREE_INCLUDES) $(HT_INCLUDES) $(DUMP_INCLUDES) $(GEN_INCLUDES) $(BACK_INCLUDES) \
	$(COMMON_FILES) $(TREE_FILES) $(DUMP_FILES) $(BACK_FILES)
	@echo "-----------------------------------------------------------------------------------------"

asm: ASSEMBLER/main_asm.cpp $(COMMON_FILES) $(ASM_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ -o asm_program $(FLAGS) ASSEMBLER/main_asm.cpp $(COMMON_INCLUDES) $(CONFIG_INCLUDES) \
	$(STK_INCLUDES) $(HT_INCLUDES) $(ASM_INCLUDES) $(GEN_INCLUDES) \
	$(COMMON_FILES) $(ASM_FILES)
	@echo "-----------------------------------------------------------------------------------------"


run-gen: gen
	./gen_program

run-front: front
	./front_program

run-back: back
	./back_program
	
run: run-back

clean:
	rm -f wolf_program gen_program front_program

help:
	@echo "Available commands:"
	@echo ""
	@echo "  make front                    - compile a frontend"
	@echo "  make run-front                - compile and run frontend"
	@echo ""
	@echo "  make back                     - compile a backend"
	@echo "  make run-back                 - compile and run backend"
	@echo ""
	@echo "  make run                      - compile and run backend"
	@echo ""
	@echo "  make clean                    - remove compiled programs"

.PHONY: gen front back asm run-gen run-front run-back run clean help