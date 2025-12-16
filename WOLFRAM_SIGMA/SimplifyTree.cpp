#include "wolfram.hpp"
#include "OpInstrSet.cpp"

static int global_number_changes = 0;


void SimplifyTree(node_t* root)
{
    int global_number_changes_before = global_number_changes;
    do {
        global_number_changes_before = global_number_changes;
        ConstFold(root);
        RemoveNeutralElem(root);
    } while (global_number_changes_before != global_number_changes);
}


node_t *node_debug = NULL;
#define NODE_DEBUG(node) node_debug = node


double ConstFold(node_t *node)
{
    if (IS_BAD_PTR(node)) return NAN;
    
    NODE_DEBUG(node);

    // printf(ANSI_COLOR_BLUE "NODE: [%p]\n" ANSI_COLOR_RESET, node);
    switch (node->type) 
    {
        case ARG_OP:
        {
            double left_val  = ConstFold(node->left);
            double right_val = ConstFold(node->right);

            hash_t op_hash = GetHash(node->item.op);
            size_t index   = 0;
            
            if ((op_instr_set[index].num_args == 2 && isnan(left_val)) || isnan(right_val)) return NAN;
            
            if (HashSearch(op_hash, &index) == WOLF_SUCCESS)
            {
                calc_context context = {left_val, right_val};
                double result = op_instr_set[index].calc(&context);
                ReplaceNode(node, NUM_(result));
                return result;
            }

            return NAN;
        }
        case ARG_VAR:
            return NAN;

        case ARG_NUM:
            return node->item.num;

        default:
            return NAN;
    }
}


node_t *RemoveNeutralElem(node_t *node)
{
    if (node == NULL) return NULL;
    
    // NODE_DEBUG(node->left);
    node->left  = RemoveNeutralElem(node->left);
    node->right = RemoveNeutralElem(node->right);
    
    if (node->type != ARG_OP) return node;
    
    hash_t op_hash = GetHash(node->item.op);
    
    int left_is_num  = (node->left  != NULL && node->left->type  == ARG_NUM);
    int right_is_num = (node->right != NULL && node->right->type == ARG_NUM);
   
    switch (op_hash)
    {
        case HASH_MUL:
            if (left_is_num && is_zero(node->left->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->right);
            }
            else if (right_is_num && is_zero(node->right->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->left);
            }
            else if ((left_is_num  && is_zero(node->left->item.num)) || 
                        (right_is_num && is_zero(node->right->item.num)))
            {
                node_t* zero_node = NUM_(0.0);
                ReplaceNode(node, zero_node);
                return zero_node;
            }
            break;

        case HASH_DIV:
            if (right_is_num && is_zero(node->right->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->left);
            }
            break;

        case HASH_ADD:
            if (left_is_num && is_zero(node->left->item.num))
            {
                return SwapParentAndChild(node, node->right);
            }
            else if (right_is_num && is_zero(node->right->item.num))
            {
                return SwapParentAndChild(node, node->left);
            }
            break;

        case HASH_SUB:
            if (left_is_num && is_zero(node->left->item.num))
            {
                return MUL_(SwapParentAndChild(node, node->right), NUM_(-1.0));
            }
            else if (right_is_num && is_zero(node->right->item.num))
            {
                return SwapParentAndChild(node, node->left);
            }
            break;

        case HASH_POW:
            if (right_is_num && is_zero(node->right->item.num))
            {
                node_t* one_node = NUM_(1.0);
                ReplaceNode(node, one_node);
                return one_node;
            }
            else if (left_is_num && is_zero(node->left->item.num - 1.0))
            {
                node_t* one_node = NUM_(1.0);
                ReplaceNode(node, one_node);
                return one_node;
            }
            else if (right_is_num && is_zero(node->right->item.num - 1.0))
            {
                return SwapParentAndChild(node, node->left);
            }
            break;
        
        default:
            break;
    }
    
    return node; 
}


void ReplaceNode(node_t *old_node, node_t *new_node)
{
    if (IS_BAD_PTR(old_node) || IS_BAD_PTR(new_node)) return;
    
    new_node->parent = old_node->parent;
    if (old_node->parent != NULL)
    {
        if (old_node->parent->left == old_node)
            old_node->parent->left = new_node;
        else
            old_node->parent->right = new_node;
            
        FreeNodes(old_node);
        global_number_changes++;
    }
    
}


node_t *SwapParentAndChild(node_t *parent, node_t *child)
{
    ON_DEBUG( if (IS_BAD_PTR(parent) || IS_BAD_PTR(child)) return NULL; )

    node_t *new_node = CopyNode(child);         //improve recursion
    ReplaceNode(parent, new_node);
    return new_node;
}