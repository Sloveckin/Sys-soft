%{

#include "parser.tab.h"
#include "node.h"

%}

%option yylineno
%option noyywrap
%%

int { struct Node *node = create_node(0, "", "int"); yylval.node = node; return nativee; }

uint { struct Node *node = create_node(0, "", "uint"); yylval.node = node; return nativee; }

long { struct Node *node = create_node(0, "", "long"); yylval.node = node; return nativee; }

ulong { struct Node *node = create_node(0, "", "ulong"); yylval.node = node; return nativee; }

char { struct Node *node = create_node(0, "", "char"); yylval.node = node; return nativee; }

string { struct Node *node = create_node(0, "", "string"); yylval.node = node; return nativee; }

byte { struct Node *node = create_node(0, "", "byte"); yylval.node = node; return nativee; }

bool { struct Node *node = create_node(0, "", "bool"); yylval.node = node; return nativee; }

as { return AS; }

dim { return DIM; }

end { return END; }

break { struct Node *node = create_node(0, "", "Break"); yylval.node = node; return BREAK; }

function { return FUNCTION; }

\"[^\"\\]*(?:\\.[^\"\\]*)*\" {
  *strchr(++yytext, '\"') = 0;
  struct Node *node = create_node(0, yytext, "Str");
  yylval.node = node;
  return STR;
}

0[xX][0-9A-Fa-f]+ {
  struct Node *node = create_node(0, yytext, "Hex");
  yylval.node = node;
  return HEX;
}

0[bB][01]+ {
  struct Node *node = create_node(0, yytext, "Bits");
  yylval.node = node;
  return BITS;
}

true|false { 
  struct Node *node = create_node(0, yytext, "Bool");

  yylval.node = node; 

  return BOOL;
}

\= { return ASSIGMENT; }
\== { return EQ; }
\<> { return NOT_EQ; }
\> { return MORE; }
\< { return LESS; }
or { return OR; }
and { return AND; }

while { struct Node *node = create_node(0, yytext, "While"); yylval.node = node; return WHILE; }
until { struct Node *node = create_node(0, yytext, "Until"); yylval.node = node; return UNTIL; }
loop { return LOOP; }
wend { return WEND; }
do { return DO; }
if { return IF; }
then { return THEN; }
else { return ELSE; }

; { return SEMICOLON; }

'[^']' {
  struct Node *node = create_node(0, yytext, "Char");
  yylval.node = node;
  return CHAR;
}

[a-zA-Z_][a-zA-Z_0-9]* { 
  struct Node *node = create_node(0, yytext, "Identifier");
  yylval.node = node;
  return identifier; 
}

[0-9]+ {
  struct Node *node = create_node(0, yytext, "Number");
  yylval.node = node;
  return DEC;
}

\( {return BR_OPEN; }

\) {return BR_CLOSE;}

\, { return COMMA; }

\+ { return PLUS; }
\- { return MINUS; }
\* { return MULTIPLY; }
\/ { return DIVIDE; }
\! { return NOT; }

[ \t\n] {}


%%

