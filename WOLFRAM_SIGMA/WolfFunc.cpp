#include "wolfram.hpp"
#include "OpInstrSet.cpp"


// var_t variables[MAX_NUM_VAR] = //stack как вектор чтобы можно было for
// {
//     {119, "w", 0, false},
//     {120, "x", 0, false},
//     {121, "y", 0, false},
//     {122, "z", 0, false}
// };


void EnterVar()
{
    for (int i = 0; i < MAX_NUM_VAR; ++i)
    {
        if (variables[i].is_used)
        {
            printf("Enter the value of the %s: ", variables[i].name);
            scanf("%lg", &(variables[i].value));
        }
    }
}


node_t *DerivativeNode(node_t *node, hash_t hash_indep_var)
{
    if (node == NULL) return NULL;
    
    node_t *tmp = NULL; // rename

    switch (node->type)
    {
        case ARG_OP:
        {   
            hash_t op_hash = GetHash(node->item.op);
            size_t index   = 0;

            if (HashSearch(op_hash, &index) == WOLF_SUCCESS)
            {
                diff_context context = {node->left, node->right, hash_indep_var};
                tmp = op_instr_set[index].diff(&context);
                set_parents(tmp, NULL);

                // must simplify tree here
                SimplifyTree(tmp);

                return tmp;
            }
            
            return NULL;
        }
        case ARG_VAR:
        {
            hash_t hash_var = GetHash(node->item.var);
            
            if (hash_var == hash_indep_var)
                return NUM_(1.0);
            
            return NUM_(0.0);
        }
        case ARG_NUM:
            return NUM_(0.0);
            
        default:
            return NULL;
    }
}


node_t * NDerivativeNode(node_t *node, hash_t hash_indep_var, int count)
{
    if (count == 0) return CopyNode(node);

    node_t *current = DerivativeNode(CopyNode(node), hash_indep_var);
    
    for (int i = 1; i < count; ++i)
    {
        node_t *next = DerivativeNode(CopyNode(current), hash_indep_var);
        SimplifyTree(next);
        // LATEX(current);

        FreeNodes(current);
        current = next;        
    }
    
    return current;
}


node_t *CopyNode(node_t *node)
{
    if (node == NULL) return NULL;

    node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
    if (IS_BAD_PTR(new_node)) { free(new_node); return NULL; }

    new_node->type = node->type;
    new_node->parent = NULL;
    new_node->item = node->item;

    new_node->left  = CopyNode(node->left);
    new_node->right = CopyNode(node->right);

    return new_node;
}


void TaylorSeries(tree_t *tree, const char* indep_var, double point, int order)
{   
    ON_DEBUG( if (IS_BAD_PTR(tree)) return; )
    
    node_t *series_sum = NUM_(0.0);
    hash_t hash_indep_var = GetHash(indep_var);
    node_t *x0 = NUM_(point);

    node_t *current_derivative = CopyNode(tree->root);
    
    for (int n = 0; n <= order; ++n)
    {        
        node_t *derivative_at_point = Substitute_x0(current_derivative, hash_indep_var, x0);
        double derivative_value = CalcExpression(derivative_at_point);   
        // printf("derivative_value = %g\n", derivative_value);     
        double coeff_value = derivative_value / (double)Factorial(n);
        // printf("coeff_value = %g\n", coeff_value);
        node_t *coeff = NUM_(coeff_value);
        
        node_t *x_minus_x0 = SUB_(VAR_(indep_var), CopyNode(x0));
        node_t *power_term = POW_(x_minus_x0, NUM_((double)n));
        node_t *new_term = MUL_(coeff, power_term);
        
        node_t *old_sum = series_sum;
        series_sum = ADD_(old_sum, new_term);
        
        FreeNodes(derivative_at_point);
        
        if (n < order)
        {
            // SimplifyTree(current_derivative);
            node_t *next_derivative = DerivativeNode(current_derivative, hash_indep_var);
            FreeNodes(current_derivative);
            current_derivative = next_derivative;
        }        
    }
    FreeNodes(current_derivative);
    FreeNodes(x0);
    
    set_parents(series_sum, NULL);
    SimplifyTree(series_sum);
    
    // GenGraphs(series_sum, __func__);
    // TreeToLatex(series_sum);
    
    FreeNodes(series_sum);
}


node_t* Substitute_x0(node_t *node, hash_t var_hash, node_t *value)
{
    if (!node) return NULL;
    
    if (node->type == ARG_VAR && GetHash(node->item.var) == var_hash)
    {
        return CopyNode(value);
    }
    
    node_t *new_node = CopyNode(node);
    new_node->left  = Substitute_x0(node->left, var_hash, value);
    new_node->right = Substitute_x0(node->right, var_hash, value);

    return new_node;
}


WolfErr_t HashSearch(hash_t hash, size_t *index)
{
    ON_DEBUG( if (IS_BAD_PTR(index)) return WOLF_ERROR; )

    op_t *found = (op_t*)bsearch(&hash, op_instr_set, LEN_INSTR_SET, sizeof(op_instr_set[0]), CmpForBinSearch);
    if (found == NULL)
        return WOLF_ERROR;
    
    *index = (size_t)(found - op_instr_set);
    return WOLF_SUCCESS;
}


int CmpForBinSearch(const void *a, const void *b)
{
    const hash_t *hash_a = (const hash_t*)a;
    const op_t   *op_b   = (const op_t*)b;
    
    if (*hash_a > op_b->hash)
        return 1;
    if (*hash_a < op_b->hash)
        return -1;
    return 0;
}