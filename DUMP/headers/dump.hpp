#ifndef DUMP_HPP
#define DUMP_HPP

#include "tree.hpp"

enum genErr_t
{
    GEN_SUCCESS = 0,
    GEN_ERROR   = 1
};

// HTML dump
genErr_t GenHTML(const char* title);
genErr_t GenTrees(node_t *node, const char *func);
genErr_t GenDot(FILE *src, tree_t *tree, const char *func);

// LATEX dump
// WolfErr_t LatexFileOpener(const char* path);
// WolfErr_t LatexFileCloser();

// void TreeToLatex(node_t *node);
// void NodeToLatex(node_t *node);


// #define LATEX(node) do{ \
//                         fprintf(file_latex, "\\[\n"); \
//                         NodeToLatex(node); \
//                         fprintf(file_latex, "\n\\]\n\n"); \
//                     } while(0)


#endif