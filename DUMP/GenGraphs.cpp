#include "dump.hpp"
#include "KeywordSet.cpp"


static size_t number_trees = 0;

genErr_t GenHTML(const char* title)
{
    FILE *GraphsHTML = fopen(PATH_TO_HTML, "w");
    if (IS_BAD_PTR(GraphsHTML)) return GEN_ERROR;

    fprintf(GraphsHTML, "<!DOCTYPE html>\n"
                        "<html>\n"
                        "<head>\n"
                        "\t<title>%s</title>\n"
                        "\t<style>\n"
                        "\t\tbody {\n"
                        "\t\t\tfont-family: Arial;\n"
                        "\t\t\tdisplay: flex;\n"
                        "\t\t\tflex-direction: column;\n"
                        "\t\t\talign-items: center;\n"
                        "\t\t\tpadding: 20px;\n"
                        "\t\t\tbackground-color: #f5f5f5;\n"
                        "\t\t}\n"
                        "\n"
                        "\t\t.tree-container {\n"
                        "\t\t\tmargin-top: 20px;\n"
                        "\t\t\tbackground: white;\n"
                        "\t\t\tpadding: 20px;\n"
                        "\t\t\tborder-radius: 8px;\n"
                        "\t\t\tbox-shadow: 0 2px 10px rgba(0,0,0,0.1);\n"
                        "\t\t\tmax-width: auto;\n"
                        "\t\t}\n"
                        "\n"
                        "\t\t.tree-container img {\n"
                        "\t\t\tmax-width: 100%%;\n"
                        "\t\t\theight: auto;\n"
                        "\t\t\tborder-radius: 4px;\n"
                        "\t\t}\n"
                        "\n"
                        "\t\t.tree-row {\n"
                        "\t\t\talign-items: center;\n"
                        "\t\t\tgap: 10px;\n"
                        "\t\t}\n"
                        "\t</style>\n"
                        "</head>\n"
                        "<body>\n"
                        "\t<h1 text-align: center;>%s</h1>\n", title, title);

    for (size_t i = 0; i < number_trees; ++i)
    {
        fprintf(GraphsHTML, "\t<div class=\"tree-container\" id=\"tree%zu\">\n", i);
        fprintf(GraphsHTML, "\t\t<div class=\"tree-row\">\n");
        fprintf(GraphsHTML, "\t\t\t<h4>tree%zu/tree.dot.png:</h4>\n", i);
        fprintf(GraphsHTML, "\t\t\t<img src=\"trees/tree%zu/tree.dot.png\">\n", i);
        fprintf(GraphsHTML, "\t\t</div>\n");
        fprintf(GraphsHTML, "\t</div>\n");
    }

    fprintf(GraphsHTML, "</body>\n"
                        "</html>");

    fclose(GraphsHTML);
    return GEN_SUCCESS;
}


genErr_t GenTrees(node_t *node, const char *func)
{
    ON_DEBUG( if (IS_BAD_PTR(node) || IS_BAD_PTR(func)) return GEN_ERROR; )

    tree_t *tree = (tree_t*)calloc(1, sizeof(tree_t));
    if (IS_BAD_PTR(tree)) return GEN_ERROR;

    tree->root     = node;
    tree->size     = 0;
    tree->capacity = 0;

    char folder[64] = {0};
    snprintf(folder, sizeof(folder), "mkdir -p %s/tree%zu", PATH_TO_TREES_FOLDER, number_trees);
    system(folder);

    char file_path[64] = {0};
    snprintf(file_path, sizeof(file_path), "%s/tree%zu/tree.dot", PATH_TO_TREES_FOLDER, number_trees);

    FILE *graph = fopen(file_path, "w");
    if (IS_BAD_PTR(graph)) return GEN_ERROR;

    if (GenDot(graph, tree, func))  return GEN_ERROR;
    
    fclose(graph);
    
    char dot_cmd[128] = {0};
    snprintf(dot_cmd, sizeof(dot_cmd), "dot -Tpng %s -o %s.png", file_path, file_path);
    system(dot_cmd);

    number_trees++;
    return GEN_SUCCESS;
}

//Надо вставить в код, будет классно работать

// WolfErr_t CreateNode(FILE *src, node_t *node, int node_id)
// {
//     ON_DEBUG(
//         if (IS_BAD_PTR(src))  return WOLF_ERROR;
//         if (IS_BAD_PTR(node)) return WOLF_ERROR;
//     )

//     fprintf(src, "\tn%d [shape=record, label=\"{ptr = %p | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", node_id, (void*)node, node->item, (void*)node->left, (void*)node->right);
    
//     return WOLF_SUCCESS;
// }


static const char* get_op_string(hash_t op_hash)
{
    for (size_t i = 0; i < LEN_KEYWORD_SET; ++i)
    {
        if (keyword_set[i].hash == op_hash)
        {
            return keyword_set[i].name;
        }
    }
    return "UNKNOWN_OP";
}


genErr_t GenDot(FILE *src, tree_t *tree, const char *func)
{
    ON_DEBUG( if (IS_BAD_PTR(src) || IS_BAD_PTR(tree))  return GEN_ERROR; )

    fprintf(src,"digraph G {\n"
                "\tlabel=<<B>Tree from %s()</B>>;\n"
                "\tfontcolor=\"%s\";\n"
                "\tfontname=\"Arial\";\n"
                "\tlabelloc=\"top\";\n"
                "\tlabeljust=\"center\";\n"
                "\tfontsize=20;\n"
                "\trankdir=TB;\n"
                "\tcenter=true;\n"
                "\tnodesep=0.5;\n"
                "\tranksep=0.5;\n"
                "\tnode[fontsize=9, shape=box, width=0.7, height=0.4, style=\"filled, rounded\", fontname=\"Arial\"];\n"
                "\n", func, (tree->error == 0) ? "#008000" : "#B22222");

    if (tree->root == NULL) { fprintf(src, "}"); return GEN_SUCCESS; }

    stk_t<node_t*> stk_ret = {};
    STACK_CTOR(&stk_ret, tree->size + 1);

    stk_t<size_t> stk_ids = {};
    STACK_CTOR(&stk_ids, tree->size + 1);

    size_t number_nodes = 0;

    StackPush(&stk_ret, tree->root);
    StackPush(&stk_ids, number_nodes);
    

    
    #pragma GCC diagnostic push          
    #pragma GCC diagnostic ignored "-Wformat"

    if (tree->root->type == ARG_NUM)
        fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_NUM | data = %g | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
            number_nodes, tree->root, tree->root->parent, tree->root->item, tree->root->left, tree->root->right);
    else if (tree->root->type == ARG_OP)
        fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_OP | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
            number_nodes, tree->root, tree->root->parent, get_op_string(tree->root->item.op), tree->root->left, tree->root->right);
    else if (tree->root->type == ARG_LINK)
        fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_LINK | data = %c | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
            number_nodes, tree->root, tree->root->parent, tree->root->item.link, tree->root->left, tree->root->right);
    else if (tree->root->type == ARG_VAR)
        fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_VAR | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
            number_nodes, tree->root, tree->root->parent, tree->root->item.var, tree->root->left, tree->root->right);
    else
        fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_FUNC | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
            number_nodes, tree->root, tree->root->parent, tree->root->item.func, tree->root->left, tree->root->right);
    
    #pragma GCC diagnostic pop  

    number_nodes++;

    while (stk_ret.size > 0)
    {
        node_t* current_node = NULL;
        size_t current_id = 0;
        
        StackPop(&stk_ret, &current_node);
        StackPop(&stk_ids, &current_id);

        if (current_node == NULL) continue;

        if (current_node->left != NULL)
        {
            #pragma GCC diagnostic push          
            #pragma GCC diagnostic ignored "-Wformat"

            if (current_node->left->type == ARG_NUM)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_NUM | data = %g | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->left, current_node->left->parent, current_node->left->item, current_node->left->left, current_node->left->right);
            else if (current_node->left->type == ARG_OP)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_OP | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->left, current_node->left->parent, get_op_string(current_node->left->item.op), current_node->left->left, current_node->left->right);
            else if (current_node->left->type == ARG_LINK)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_LINK | data = %c | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->left, current_node->left->parent, current_node->left->item.link, current_node->left->left, current_node->left->right);
            else if (current_node->left->type == ARG_VAR)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_VAR | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->left, current_node->left->parent, current_node->left->item.var, current_node->left->left, current_node->left->right);
            else
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_FUNC | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->left, current_node->left->parent, current_node->left->item.func, current_node->left->left, current_node->left->right);
            
            #pragma GCC diagnostic pop  

            fprintf(src, "\tn%zu:left -> n%zu\n", current_id, number_nodes);
            
            StackPush(&stk_ret, current_node->left);
            StackPush(&stk_ids, number_nodes);
            number_nodes++;
        }

        if (current_node->right != NULL)
        {
            #pragma GCC diagnostic push          
            #pragma GCC diagnostic ignored "-Wformat"

            if (current_node->right->type == ARG_NUM)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_NUM | data = %g | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->right, current_node->right->parent, current_node->right->item, current_node->right->left, current_node->right->right);
            else if (current_node->right->type == ARG_OP)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_OP | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->right, current_node->right->parent, get_op_string(current_node->right->item.op), current_node->right->left, current_node->right->right);
            else if (current_node->right->type == ARG_LINK)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_LINK | data = %c | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->right, current_node->right->parent, current_node->right->item.link, current_node->right->left, current_node->right->right);
            else if (current_node->right->type == ARG_VAR)
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_VAR | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->right, current_node->right->parent, current_node->right->item.var, current_node->right->left, current_node->right->right);
            else
                fprintf(src, "\tn%zu [shape=record, label=\"{ptr = %p | parent = %p | type = ARG_FUNC | data = %s | {<left> left = %p | <right> right = %p}}\", fillcolor=\"#87CEEB\", color=\"black\"]\n", 
                    number_nodes, current_node->right, current_node->right->parent, current_node->right->item.func, current_node->right->left, current_node->right->right);
            
            #pragma GCC diagnostic pop  

            fprintf(src, "\tn%zu:right -> n%zu\n", current_id, number_nodes);
            
            StackPush(&stk_ret, current_node->right);
            StackPush(&stk_ids, number_nodes);
            number_nodes++;
        }
    }

    STACK_DTOR(&stk_ret);
    STACK_DTOR(&stk_ids);

    fprintf(src, "}");
    return GEN_SUCCESS;
}