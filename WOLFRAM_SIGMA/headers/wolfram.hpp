#ifndef WOLFRAM_HPP
#define WOLFRAM_HPP

#include "tree.hpp"
#include "math_func.hpp"
#include "is_zero.hpp"
#include "Factorial.hpp"
#include "colors.hpp"
#include "parser.hpp"
#include "dump.hpp"
#include "DSL.hpp"

#include "HashOp.hpp"


enum WolfErr_t
{
    WOLF_SUCCESS = 0,
    WOLF_ERROR   = 1
};

struct var_t
{
    hash_t hash;
    char   name[8];  // это прям bad, зачем мне тоскать и перекопировать эти 8 байт
    double value;
    bool   is_used;
};


struct op_t
{
	hash_t hash;
	char   name[8];  // это прям bad, зачем мне тоскать и перекопировать эти 8 байт
	int    num_args;
	calc_t calc;
	diff_t diff;
};


// extern var_t variables[MAX_NUM_VAR];


WolfErr_t VerifyOpInstrSetSort();

void EnterVar();
node_t *DerivativeNode(node_t *node, hash_t hash_indep_var);
node_t *CopyNode(node_t *node);

node_t * NDerivativeNode(node_t *node, hash_t hash_indep_var, int count);

void TaylorSeries(tree_t *tree, const char* indep_var, double point, int order);
node_t* Substitute_x0(node_t *node, hash_t var_hash, node_t *value);

void SimplifyTree(node_t* root);
double ConstFold(node_t *node);
node_t *RemoveNeutralElem(node_t *node);
void ReplaceNode(node_t *old_node, node_t *new_node);
node_t *SwapParentAndChild(node_t *parent, node_t *child);

double CalcExpression(node_t *node);

lexerErr_t parseWolfTree(const char* src, tree_t *tree);

WolfErr_t HashSearch(hash_t hash, size_t *index);
int CmpForBinSearch(const void *a, const void *b);

#define SKIP_SPACES(ptr) while (isspace((int)*ptr)) ptr++

#endif