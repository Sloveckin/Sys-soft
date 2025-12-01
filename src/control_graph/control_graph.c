#include "control_graph/control_graph.h"
#include "node.h"
#include "operation_tree.h"

#include <endian.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

size_t function_find_index = 0;

void find_func_def_(struct Node *root, struct Node **functions)
{
  if (root == NULL)
    return;

  if (strcmp(root->type, "FuncDef") == 0)
  {
    functions[function_find_index++] = root;
    return;
  }

  for (size_t i = 0; i < root->children_amount; i++)
    find_func_def_(root->children[i], functions);
}

void find_func_def(struct Node *root, struct Node **functions)
{
  function_find_index = 0;
  find_func_def_(root, functions);
}

static ControlGraphNode *find_last_cgn_node(ControlGraphNode *node)
{
  if (!node)
    return NULL;

  if (!node->def && node->connect_to_end == false)
    return node;

  if (node->def)
    return find_last_cgn_node(node->def);

  return node->end;
}

static ControlGraphNode *create_control_node(Contex *context)
{
  ControlGraphNode *node = malloc(sizeof(ControlGraphNode));
  node->id = -1;
  node->visited = false;
  node->parent_amount = 0;
  node->connect_to_end = false;
  node->end = NULL;
  node->def = NULL;
  node->cond = NULL;
  node->operation_node = NULL;
  node->text = NULL;
  node->generate_asm = false;

  context_add_node(context, node);

  return node;
}

#if 1
static ControlGraphNode *create_empty_node(Contex *context)
{
  ControlGraphNode *node = malloc(sizeof(ControlGraphNode));
  node->text = malloc(6 * sizeof(char));
  sprintf(node->text, "%s", "Empty");

  node->id = -1;
  node->parent_amount = 0;
  node->parent_accum = 0;
  node->visited = false;
  node->connect_to_end = false;
  node->end = NULL;
  node->def = NULL;
  node->cond = NULL;
  node->operation_node = NULL;

  context_add_node(context, node);

  return node;
}
#endif


static char *text_from_binary_operation(struct Node *node, const char *sign)
{
    assert (node->children_amount == 2);
    char *left_text = get_text(node->children[0]);
    char *right_text = get_text(node->children[1]);

    const size_t left_len = strlen(left_text);
    const size_t right_len = strlen(right_text);

    char *text = malloc((left_len + right_len + 5 + strlen(sign)) * sizeof(char));
    sprintf(text, "(%s %s %s)", left_text, sign, right_text); 

    free(left_text);
    free(right_text);

    return text;
}

static ControlGraphNode *operation(Contex *context, struct Node *node)
{
  ControlGraphNode *cgn = create_control_node(context);
  cgn->text = get_text(node);
  cgn->operation_node = create_operation_tree_node(node);
  return cgn;
}

static char *get_text_form_unary_operation(struct Node* node, const char *sign)
{
  assert(node->children_amount == 1);
  char *child_text = get_text(node->children[0]);

  const size_t child_text_len = strlen(child_text);

  char *text = malloc((child_text_len + 2) * sizeof(char));
  sprintf(text, "%s%s", sign, child_text);

  free(child_text);
  
  return text;
}

static ControlGraphNode *if_condition(Contex *context, struct Node *node)
{
    ControlGraphNode *cond = foo(context, node->children[0]);
    ControlGraphNode *body = foo(context, node->children[1]);
    ControlGraphNode *else_body = foo(context, node->children[2]);

    ControlGraphNode *end = create_empty_node(context);

    ControlGraphNode *body_last = find_last_cgn_node(body);
    if (!body_last)
      body_last = create_empty_node(context);

    if (else_body)
    {
      ControlGraphNode *else_last = find_last_cgn_node(else_body);
      else_last->def = end;
      cond->def = else_body;
    }
    else 
    {
      cond->def = end;
    }

    if (body)
      cond->cond = body;
    else
      cond->cond = end;

    body_last->def = end;

    end->parent_amount = 2;

    return cond;
}

static ControlGraphNode *while_cycle(Contex *context, struct Node* node)
{

  if (node->children_amount == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);
    cgn->text = malloc(6 * sizeof(char));
    sprintf(cgn->text, "while");
    return cgn;
  }

  ControlGraphNode *cond = foo(context, node->children[1]);
  ControlGraphNode *stats = foo(context, node->children[2]);

  ControlGraphNode *end = create_empty_node(context);

  cond->def = end;
  
  ControlGraphNode *last_stats = find_last_cgn_node(stats);
  cond->cond = stats;

  last_stats->def = cond;

  return cond;
}

static ControlGraphNode *do_until_cycle(Contex *context, struct Node *node) 
{
  ControlGraphNode *statments = foo(context, node->children[0]);
  ControlGraphNode *while_or_until = foo(context, node->children[1]);
  ControlGraphNode *cond = foo(context, node->children[2]);

  ControlGraphNode *end = create_empty_node(context);

  ControlGraphNode *last_stats = find_last_cgn_node(statments);

  assert ((strcmp(while_or_until->text, "until") == 0) || (strcmp(while_or_until->text, "while")) == 0);

  if (strcmp(while_or_until->text, "while") == 0)
  {
    cond->def = end;
    cond->cond = statments;
  }
  else
  {
    cond->def = statments;
    cond->cond = end;
  }

  last_stats->def = cond;

  return statments; 
}


static ControlGraphNode *dim(Contex *context, struct Node *node)
{

    ControlGraphNode **cgns = malloc(node->children_amount * sizeof(ControlGraphNode*));
    for (size_t i = 0; i < node->children_amount; i++)
    {
      cgns[i] = foo(context, node->children[i]);
    }

    size_t need_len = 0;
    for (size_t i = 0; i < node->children_amount; i++)
    {
      need_len += strlen(cgns[i]->text) * 2;
    }
    // For commas
    need_len += node->children_amount;

    ControlGraphNode *cgn = create_control_node(context);
    cgn->text = malloc(need_len * sizeof(char));

    strcpy(cgn->text, cgns[0]->text);

    if (need_len != 1)
        strcat(cgn->text, ", ");

    for (size_t i = 1; i < node->children_amount; i++)
    {
      if (i != node->children_amount - 1)
        strcat(cgn->text, ", ");

      strcat(cgn->text, cgns[i]->text);
    }

    //cgn->operation_node = create_operation_tree_node(node);

    free(cgns);

    return cgn;
}

ControlGraphNode *foo(Contex *context, struct Node *node)
{

  if (!node)
    return NULL;

  if (strcmp(node->type, "ListStatement") == 0)
  {
    ControlGraphNode *first = foo(context, node->children[0]);
    ControlGraphNode *second = foo(context, node->children[1]);

    if (!second)
      return first;

    ControlGraphNode *last_first = find_last_cgn_node(first);
    last_first->def = second;

    return first;
  }

  if (strcmp(node->type, "Assigment") == 0)
  {

    assert (node->children_amount == 2);

    char *left_text = get_text(node->children[0]);
    char *right_text = get_text(node->children[1]);

    const size_t left_len = strlen(left_text);
    const size_t right_len = strlen(right_text);

    ControlGraphNode *cgn = create_control_node(context);

    cgn->text = malloc((left_len + right_len + 4) * sizeof(char));
    sprintf(cgn->text, "%s = %s", left_text, right_text);

    free(left_text);
    free(right_text);

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "Plus") == 0)
    return operation(context,node);

  if (strcmp(node->type, "Minus") == 0) 
    return operation(context, node);

  if (strcmp(node->type, "Multiply") == 0) 
    return operation(context, node);

  if (strcmp(node->type, "Divide") == 0)
    return operation(context, node);

  if (strcmp(node->type, "More") == 0)
    return operation(context, node);

  if (strcmp(node->type, "Less") == 0)
    return operation(context, node);

  if (strcmp(node->type, "Equals") == 0)
    return operation(context, node);

  if (strcmp(node->type, "NotEquals") == 0)
    return operation(context, node);

  if (strcmp(node->type, "And") == 0)
    return operation(context, node);

  if (strcmp(node->type, "Or") == 0)
    return operation(context, node);

  if (strcmp(node->type, "UnaryPlus") == 0)
    return operation(context, node);

  if (strcmp(node->type, "UnaryMinus") == 0)
    return operation(context, node);

  if (strcmp(node->type, "Not") == 0)
    return operation(context, node); 

  if (strcmp(node->type, "Identifier") == 0 || strcmp(node->type, "Number") == 0 || strcmp(node->type, "Char") == 0 || strcmp(node->type, "Bool") == 0
                                                   || strcmp(node->type, "Hex") == 0 || strcmp(node->type, "Bits") == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);

    cgn->operation_node = create_operation_tree_node(node);

    cgn->text = get_text(node);
    return cgn;
  }

  if (strcmp(node->type, "Str") == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);
    cgn->text = malloc((strlen(node->text) + 3) * sizeof(char));
    sprintf(cgn->text, "\"%s\"", node->text);
    return cgn;
  }

  if (strcmp(node->type, "Var") == 0)
  {
    assert (node->children_amount == 2);
    ControlGraphNode *cgn = create_control_node(context);
    ControlGraphNode *variables = foo(context, node->children[0]);
    ControlGraphNode *type = foo(context, node->children[1]);

    const size_t text_len = strlen(variables->text) + strlen(type->text) + 2;
    cgn->text = malloc(text_len * sizeof(char));
    sprintf(cgn->text, "%s %s", type->text, variables->text);

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "Variables") == 0)
    return dim(context, node);

  if  (strcmp(node->type, "int") == 0 || strcmp(node->type, "string") == 0 || strcmp(node->type, "bool") == 0 || strcmp(node->type, "byte") == 0
                                                  || strcmp(node->type, "uint") == 0 || strcmp(node->type, "long") == 0 || strcmp(node->type, "ulong") == 0 || strcmp(node->type, "char") == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);
    cgn->text = malloc((strlen(node->type) + 1) * sizeof(char));
    sprintf(cgn->text, "%s", node->type);
    return cgn;
  }

  if (strcmp(node->type, "If") == 0)
    return if_condition(context, node);

  if (strcmp(node->type, "ElseBlock") == 0)
  {
    assert (node->children_amount == 1);
    return foo(context, node->children[0]);
  }

  if (strcmp(node->type, "While") == 0)
    return while_cycle(context, node);

  if (strcmp(node->type, "Do") == 0)
    return do_until_cycle(context, node);

  if (strcmp(node->type, "CallOrIndexer") == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);

    ControlGraphNode *proc = foo(context, node->children[0]);
    ControlGraphNode *var = foo(context, node->children[1]);

    const size_t proc_len = strlen(proc->text);
    const size_t var_len = strlen(var->text);
    cgn->text = malloc((proc_len + var_len + 3) * sizeof(char));
    sprintf(cgn->text, "%s(%s)", proc->text, var->text);

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "ListExpr") == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);

    ControlGraphNode *first = foo(context, node->children[0]);
    ControlGraphNode *second = foo(context, node->children[1]);

    const size_t proc_len = strlen(first->text);
    const size_t var_len = strlen(second->text);

    cgn->text = malloc((proc_len + var_len + 2) * sizeof(char));
    sprintf(cgn->text, "%s %s", first->text, second->text);

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "Break") == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);
    cgn->text = malloc(6 * sizeof(char));
    sprintf(cgn->text, "%s", "Break");

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "Until") == 0)
  {
    ControlGraphNode *cgn = create_control_node(context);
    cgn->text = malloc(6 * sizeof(char));
    sprintf(cgn->text, "until");
    return cgn;
  }

  assert (0);
}

size_t dgml_id = 1;

void init_control_graph_id_(ControlGraphNode *node)
{
  if (!node)
    return;

  if (node->visited)
    return;

  node->visited = true;
  
  node->id = dgml_id++;
  
  init_control_graph_id_(node->def);
  init_control_graph_id_(node->cond);
  init_control_graph_id_(node->end);
}

void init_control_graph_id(ControlGraphNode *node)
{
  dgml_id = 1;
  init_control_graph_id_(node);
}



char *get_text(struct Node *node)
{
  if (strcmp(node->type, "Identifier") == 0 || strcmp(node->type, "Number") == 0 || strcmp(node->type, "Char") == 0 || strcmp(node->type, "Bool") == 0
                                                   || strcmp(node->type, "Hex") == 0 || strcmp(node->type, "Bits") == 0)
  {
    char *text = malloc((strlen(node->text) + 1) * sizeof(char));
    sprintf(text, "%s", node->text);
    return text;
  }


  if (strcmp(node->type, "Plus") == 0)
    return text_from_binary_operation(node, "+");

  if (strcmp(node->type, "Minus") == 0) 
    return text_from_binary_operation(node, "-");

  if (strcmp(node->type, "Multiply") == 0) 
    return text_from_binary_operation(node, "*");

  if (strcmp(node->type, "Divide") == 0)
    return text_from_binary_operation(node, "/");

  if (strcmp(node->type, "More") == 0)
    return text_from_binary_operation(node, ">");

  if (strcmp(node->type, "Less") == 0)
    return text_from_binary_operation(node, "<");

  if (strcmp(node->type, "Equals") == 0)
    return text_from_binary_operation(node, "==");

  if (strcmp(node->type, "NotEquals") == 0)
    return text_from_binary_operation(node, "<>");

  if (strcmp(node->type, "And") == 0)
    return text_from_binary_operation(node, "and");

  if (strcmp(node->type, "Or") == 0)
    return text_from_binary_operation(node, "or");

  if (strcmp(node->type, "UnaryPlus") == 0)
    return get_text_form_unary_operation(node, "+");

  if (strcmp(node->type, "UnaryMinus") == 0)
    return get_text_form_unary_operation(node, "-");

  if (strcmp(node->type, "Not") == 0)
    return get_text_form_unary_operation(node, "!"); 

  assert (0);
}


void free_control_graph_node(ControlGraphNode *node)
{
  assert(node);

  if (node->operation_node)
    free_operation_tree(node->operation_node);

  if (node->text)
    free(node->text);

  free(node);
}

