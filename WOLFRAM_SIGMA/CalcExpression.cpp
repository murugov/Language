#include "wolfram.hpp"
#include "OpInstrSet.cpp"


double CalcExpression(node_t *node)
{
    if (node == NULL) return NAN;
    
    switch (node->type) 
    {
        case ARG_OP:
        {
            hash_t op_hash = GetHash(node->item.op);
            size_t index   = 0;

            if (HashSearch(op_hash, &index) == WOLF_SUCCESS)
            {
                double left_val = NAN, right_val = NAN;
                
                if (op_instr_set[index].num_args >= 1) 
                {
                    right_val = CalcExpression(node->right);
                    if (isnan(right_val)) return NAN;
                }
                
                if (op_instr_set[index].num_args == 2) 
                {
                    left_val = CalcExpression(node->left);
                    if (isnan(left_val)) return NAN;
                }
                
                calc_context context = {left_val, right_val};
                return op_instr_set[index].calc(&context);
            }

            return NAN;
        }
        case ARG_VAR:
        {
            hash_t node_hash = GetHash(node->item.var);
            for (int i = 0; i < MAX_NUM_VAR; ++i)
            {
                if (variables[i].hash == node_hash)
                    return variables[i].value;
            }

            return NAN;
        }
        case ARG_NUM:
            return node->item.num;

        default:
            return NAN;
    }
}