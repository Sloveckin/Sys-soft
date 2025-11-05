enums = [
  "INT", "UINT", "LONG", 
  "ULONG", "STRING", 
  "BYTE", "BOOLEAN", "BREAK", 
  "STR", "HEX", "BITS", 
  "BOOLEAN", "WHILE", "UNTIL", 
  "CHAR", "Identifier", "Number",
  "VoidType", "Array", "PartOfArray",
  "ArgDef", "EmptyListOpt", "SourceItemList", "FuncSignature",
  "FuncDef", "ListStatement", "ElseBlock", "Do", "Var", "Assigment",
  "Plus", "Minus", "Multiply", "Divide", "Equals", "NotEquals", "More", "Less", "Or", "And",
  "UnaryPlus", "UnaryMinus", "Not", "ListExpr", "CallOrIndexer"]

def print_sign():
  print("static void type_to_string(const enum Type type, char *buffer)")

def open():
  print("{")

def close():
  print("}")

def switch():
  print("switch(type)")

def cases():
  for en in enums:
    s = en[0] + en[1:].lower()
    pr = "case {}: strcpy(buffer, \"{}\"); break;".format(en, s)
    print(pr)

def en_start():
  print("#ifndef H_NODE_TYPE")
  print("#define H_NODE_TYPE")
  print("enum Type {")

def en():
  print(",\n".join(enums))

def en_end():
  print("};")

  print("#endif")

if __name__ == "__main__":

  funs = [en_start, en, en_end]
  for f in funs:
    f()

  input()

  funs = [print_sign, open, switch, open, cases, close, close]

  for f in funs:
    f()
