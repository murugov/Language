#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include "CONFIG.hpp"

#include "GetHash.hpp"
#include "stack.hpp"
#include "tree.hpp"
#include "hash_table.hpp"
#include "HashOp.hpp"
#include "dump.hpp"
#include "colors.hpp"

#define ADR_REG "r0"

#define GLOBAL_OFFSET     1000
#define MAX_NUM_FUNC_ARGS 8

extern FILE *ReportFile;

enum backErr_t
{
    BACK_SUCCESS = 0,
    BACK_ERROR   = 1
};

struct ntElem_t
{
    const char*  name;
    type_t 		 type;
    int          offset;
    int          size_of_stk_frm;
};

struct trans_t
{
    node_t*                  node;
    stk_t<ht_t<ntElem_t*>*>* name_tables;
    int                      cur_name_table;
    int                      global_offset;
};

char *DataReader(FILE *SourceFile);
node_t* NodeReader(char** cur_ptr, node_t* parent);

backErr_t transCtor(trans_t *trans, const char *src);
backErr_t transDtor(trans_t *trans);

backErr_t TranslateTree(trans_t *trans, const char* report);
backErr_t CompileOnlyFunc(trans_t *trans);
backErr_t CompileNotFunc(trans_t *trans);
backErr_t FindAndCompileInits(trans_t *trans);

backErr_t TranslatePrimary(trans_t *trans);
backErr_t TranslateCalc(trans_t *trans);
backErr_t TranslateVarInit(trans_t *trans);
backErr_t TranslateFuncDef(trans_t *trans);
backErr_t TranslateAssignment(trans_t *trans);
backErr_t TranslateCondition(trans_t *trans);
backErr_t TranslateFuncCall(trans_t *trans);

backErr_t TranslateIf(trans_t * trans);
backErr_t TranslateWhile (trans_t *trans);
backErr_t TranslateReturn(trans_t *trans);

backErr_t TranslateOp(trans_t *trans);
backErr_t TranslateVar(trans_t *trans);


#define TR_NODE      (trans->node)
#define TR_NODE_NUM  (trans->node->item.num)
#define TR_NODE_HASH (trans->node->item.op)
#define TR_NODE_VAR  (trans->node->item.var)
#define TR_NODE_FUNC (trans->node->item.func)
#define TR_NODE_TYPE (trans->node->type)
#define TR_NODE_L    (trans->node->left)
#define TR_NODE_R    (trans->node->right)

#define CUR_NAME_TABLE      (trans->name_tables->data[trans->cur_name_table])
#define CUR_NAME_TABLE_POS  (trans->cur_name_table)
#define CUR_NAME_TABLE_SIZE (trans->name_tables->size)

#endif