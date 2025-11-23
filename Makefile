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
	src/asm/Asm.c \
	src/asm/generate_asm.c \
	src/asm/variable_set.c \
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
	
