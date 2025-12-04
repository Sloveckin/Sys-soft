CFLAGS+=\
	-g \
	#-fsanitize=leak \
	-fsanitize=address \
	-fsanitize=pointer-compare \
	-fsanitize=pointer-subtract \
	-fsanitize=undefined \
	-fsanitize-address-use-after-scope

YFLAGS+= -d
LFLAGS+=--header-file=generated/yylex.h

main: CFLAGS+= -Iinclude -Igenerated
main: \
	src/node.c src/converter.c \
	src/signature.c \
	src/control_graph/control_graph.c \
	src/control_graph/convert_to_dgml.c \
	src/asm/asm.c \
	src/asm/generate_asm.c \
	src/asm/variable_set.c \
	src/asm/instruction_list.c \
	src/asm/register_stack.c \
	src/asm/error_list.c \
	src/asm/label_generator.c \
	src/type.c \
	src/control_graph/context.c \
	src/operation_tree.c \
	generated/parser.tab.c generated/lex.yy.c

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

generated/parser.tab.c: src/parser.y
	bison $(YFLAGS) -o $@ $<

generated/lex.yy.c: src/lexer.lex
	$(LEX) $(LFLAGS) -o $@ $<
