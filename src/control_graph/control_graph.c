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
  if (!node->def && node->connect_to_end == false)
    return node;

  if (node->def)
    return find_last_cgn_node(node->def);

  return node->end;
}

static ControlGraphNode *create_empty_node()
{
  ControlGraphNode *node = malloc(sizeof(ControlGraphNode));
  node->text = malloc(6 * sizeof(char));
  sprintf(node->text, "%s", "Empty");

  node->id = -1;
  node->visited = false;
  node->connect_to_end = false;
  node->end = NULL;
  node->def = NULL;
  node->cond = NULL;

  return node;
}


static ControlGraphNode *binary_operation1(struct Node *node, const char *sign)
{
    assert (node->children_amount == 2);
    ControlGraphNode *left = foo(node->children[0]);
    ControlGraphNode *right = foo(node->children[1]);

    const size_t left_len = strlen(left->text);
    const size_t right_len = strlen(right->text);

    ControlGraphNode *cgn = create_empty_node();
    cgn->operation_node = create_operation_tree_node(node);

    cgn->text = malloc((left_len + right_len + 5 + strlen(sign)) * sizeof(char));
    sprintf(cgn->text, "(%s %s %s)", left->text, sign, right->text);


    return cgn;
}

static ControlGraphNode *unary_operation(struct Node* node, const char *sign)
{
  assert (node->children_amount == 1);
  ControlGraphNode *child = foo(node->children[0]);

  const size_t text_len = strlen(child->text);
  const size_t sign_len = strlen(sign);

  ControlGraphNode *cgn = create_empty_node();
  cgn->operation_node = create_operation_tree_node(node);

  cgn->text = malloc((text_len + sign_len + 1) * sizeof(char));
  sprintf(cgn->text, "%s%s", sign, child->text);

  return cgn;
}

static ControlGraphNode *if_condition(struct Node *node)
{
    ControlGraphNode *cond = foo(node->children[0]);
    ControlGraphNode *body = foo(node->children[1]);
    ControlGraphNode *else_body = foo(node->children[2]);

    ControlGraphNode *end = create_empty_node();

    ControlGraphNode *body_last = find_last_cgn_node(body);

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

    cond->cond = body;
    body_last->def = end;

    return cond;
}

static ControlGraphNode *while_cycle(struct Node* node)
{

  if (node->children_amount == 0)
  {
    ControlGraphNode *cgn = create_empty_node();
    cgn->text = malloc(6 * sizeof(char));
    sprintf(cgn->text, "while");
    return cgn;
  }

  ControlGraphNode *cond = foo(node->children[1]);
  ControlGraphNode *stats = foo(node->children[2]);

  ControlGraphNode *end = create_empty_node();

  cond->def = end;
  
  ControlGraphNode *last_stats = find_last_cgn_node(stats);
  cond->cond = stats;

  last_stats->def = cond;

  return cond;
}

static ControlGraphNode *do_until_cycle(struct Node *node) 
{
  ControlGraphNode *statments = foo(node->children[0]);
  ControlGraphNode *while_or_until = foo(node->children[1]);
  ControlGraphNode *cond = foo(node->children[2]);

  ControlGraphNode *end = create_empty_node();

  cond->def = end;
  ControlGraphNode *last_stats = find_last_cgn_node(statments);

  assert ((strcmp(while_or_until->text, "until") == 0) || (strcmp(while_or_until->text, "while")) == 0);

  cond->cond = statments;
  last_stats->def = cond;

  return statments; 
}


static ControlGraphNode *dim(struct Node *node)
{

    ControlGraphNode **cgns = malloc(node->children_amount * sizeof(ControlGraphNode*));
    for (size_t i = 0; i < node->children_amount; i++)
    {
      cgns[i] = foo(node->children[i]);
    }

    size_t need_len = 0;
    for (size_t i = 0; i < node->children_amount; i++)
    {
      need_len += strlen(cgns[i]->text) * 2;
    }
    // For commas
    need_len += node->children_amount;

    ControlGraphNode *cgn = create_empty_node();
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

    return cgn;
}

ControlGraphNode *foo(struct Node *node)
{

  if (!node)
    return NULL;

  if (strcmp(node->type, "ListStatement") == 0)
  {
    ControlGraphNode *first = foo(node->children[0]);
    ControlGraphNode *second = foo(node->children[1]);

    if (!second)
      return first;

    ControlGraphNode *last_first = find_last_cgn_node(first);
    //ControlGraphNode *last_second = find_last_cgn_node(second);

    last_first->def = second;

    return first;
  }

  if (strcmp(node->type, "Assigment") == 0)
  {

    assert (node->children_amount == 2);

    ControlGraphNode *left = foo(node->children[0]);
    ControlGraphNode *right = foo(node->children[1]);

    const size_t left_len = strlen(left->text);
    const size_t right_len = strlen(right->text);

    ControlGraphNode *control_node = create_empty_node();

    control_node->text = malloc((left_len + right_len + 4) * sizeof(char));
    sprintf(control_node->text, "%s = %s", left->text, right->text);

    control_node->operation_node = create_operation_tree_node(node);

    return control_node;
    
  }

  if (strcmp(node->type, "SourceItemList") == 0)
  {
    // TODO: rewrite when children amount > 1

    for (size_t i = 0; i < node->children_amount; i++)
    {
      if (node->children[i] == NULL)
        continue;

      return foo(node->children[i]);
    }

  }

  if (strcmp(node->type, "FuncDef") == 0)
  {
    assert (node->children_amount == 2);
    ControlGraphNode *list_statement = foo(node->children[1]);
    return list_statement;
  }

  if (strcmp(node->type, "Plus") == 0)
    return binary_operation1(node, "+");

  if (strcmp(node->type, "Minus") == 0) 
    return binary_operation1(node, "-");

  if (strcmp(node->type, "Multiply") == 0) 
    return binary_operation1(node, "*");

  if (strcmp(node->type, "Divide") == 0)
    return binary_operation1(node, "/");

  if (strcmp(node->type, "More") == 0)
    return binary_operation1(node, ">");

  if (strcmp(node->type, "Less") == 0)
    return binary_operation1(node, "<");

  if (strcmp(node->type, "Equals") == 0)
    return binary_operation1(node, "==");

  if (strcmp(node->type, "NotEquals") == 0)
    return binary_operation1(node, "<>");

  if (strcmp(node->type, "And") == 0)
    return binary_operation1(node, "and");

  if (strcmp(node->type, "Or") == 0)
    return binary_operation1(node, "or");

  if (strcmp(node->type, "UnaryPlus") == 0)
    return unary_operation(node, "+");

  if (strcmp(node->type, "UnaryMinus") == 0)
    return unary_operation(node, "-");

  if (strcmp(node->type, "Not") == 0)
    return unary_operation(node, "not");

  if (strcmp(node->type, "Identifier") == 0 || strcmp(node->type, "Number") == 0 || strcmp(node->type, "Char") == 0 || strcmp(node->type, "Bool") == 0
                                                   || strcmp(node->type, "Hex") == 0 || strcmp(node->type, "Bits") == 0)
  {
    ControlGraphNode *cgn = create_empty_node();
    cgn->text = malloc((strlen(node->text) + 1) * sizeof(char));
    sprintf(cgn->text, "%s", node->text);
    return cgn;
  }

  if (strcmp(node->type, "Str") == 0)
  {
    ControlGraphNode *cgn = create_empty_node();
    cgn->text = malloc((strlen(node->text) + 3) * sizeof(char));
    sprintf(cgn->text, "\"%s\"", node->text);
    return cgn;
  }

  if (strcmp(node->type, "Var") == 0)
  {
    assert (node->children_amount == 2);
    ControlGraphNode *cgn = create_empty_node();
    ControlGraphNode *variables = foo(node->children[0]);
    ControlGraphNode *type = foo(node->children[1]);

    const size_t text_len = strlen(variables->text) + strlen(type->text) + 2;
    cgn->text = malloc(text_len * sizeof(char));
    sprintf(cgn->text, "%s %s", type->text, variables->text);

    return cgn;
  }

  if (strcmp(node->type, "Variables") == 0)
    return dim(node);

  if  (strcmp(node->type, "int") == 0 || strcmp(node->type, "string") == 0 || strcmp(node->type, "bool") == 0 || strcmp(node->type, "byte") == 0
                                                  || strcmp(node->type, "uint") == 0 || strcmp(node->type, "long") == 0 || strcmp(node->type, "ulong") == 0 || strcmp(node->type, "char") == 0)
  {
    ControlGraphNode *cgn = create_empty_node();
    cgn->text = malloc((strlen(node->type) + 1) * sizeof(char));
    sprintf(cgn->text, "%s", node->type);
    return cgn;
  }

  if (strcmp(node->type, "If") == 0)
    return if_condition(node);

  if (strcmp(node->type, "ElseBlock") == 0)
  {
    assert (node->children_amount == 1);
    return foo(node->children[0]);
  }

  if (strcmp(node->type, "While") == 0)
    return while_cycle(node);

  if (strcmp(node->type, "Do") == 0)
    return do_until_cycle(node);

  if (strcmp(node->type, "CallOrIndexer") == 0)
  {
    ControlGraphNode *cgn = create_empty_node();

    ControlGraphNode *proc = foo(node->children[0]);
    ControlGraphNode *var = foo(node->children[1]);

    const size_t proc_len = strlen(proc->text);
    const size_t var_len = strlen(var->text);
    cgn->text = malloc((proc_len + var_len + 3) * sizeof(char));
    sprintf(cgn->text, "%s(%s)", proc->text, var->text);

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "ListExpr") == 0)
  {
    ControlGraphNode *cgn = create_empty_node();

    ControlGraphNode *first = foo(node->children[0]);
    ControlGraphNode *second = foo(node->children[1]);

    const size_t proc_len = strlen(first->text);
    const size_t var_len = strlen(second->text);

    cgn->text = malloc((proc_len + var_len + 2) * sizeof(char));
    sprintf(cgn->text, "%s %s", first->text, second->text);

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "Break") == 0)
  {
    ControlGraphNode *cgn = create_empty_node();
    cgn->text = malloc(6 * sizeof(char));
    sprintf(cgn->text, "%s", "Break");

    cgn->operation_node = create_operation_tree_node(node);

    return cgn;
  }

  if (strcmp(node->type, "Until") == 0)
  {
    ControlGraphNode *cgn = create_empty_node();
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

void free_control_node(ControlGraphNode *node)
{
  if (!node)
    return;

  free_control_node(node->def);
  free_control_node(node->cond);
  free_control_node(node->end);

  if (node->text)
    free(node->text);

  if (node->operation_node)
    free_operation_tree(node->operation_node);

  free(node);
}