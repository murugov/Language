#include "dump.hpp"

#define INSERT_TO_LATEX(str) fprintf(file_latex, "%s", str)


genErr_t LatexFileOpener(const char* path)
{
    if (IS_BAD_PTR(path)) return GEN_ERROR;

    FILE *file_latex = fopen(path, "w");
    if (IS_BAD_PTR(file_latex))
    { 
        printf(ANSI_COLOR_RED "Error: Cannot open file %s\n" ANSI_COLOR_RESET, path); 
        return WOLF_ERROR; 
    }

    INSERT_TO_LATEX("\\documentclass[a4paper,10pt]{extarticle}\n");
    INSERT_TO_LATEX("\\usepackage{icomma}\n");
    INSERT_TO_LATEX("\\usepackage{amsmath}\n");
    INSERT_TO_LATEX("\\usepackage[english,russian]{babel}\n");
    INSERT_TO_LATEX("\\usepackage{graphicx}\n");
    INSERT_TO_LATEX("\\graphicspath{{images/}}\n");
    INSERT_TO_LATEX("\\usepackage[export]{adjustbox}\n");
    INSERT_TO_LATEX("\\usepackage{subcaption}\n");
    INSERT_TO_LATEX("\\usepackage[T2A]{fontenc}\n");
    INSERT_TO_LATEX("\\usepackage[utf8]{inputenc}\n");
    INSERT_TO_LATEX("\\usepackage{amsfonts,amssymb,amsthm,mathtools}\n");
    INSERT_TO_LATEX("\\usepackage[paperheight=16cm,paperwidth=12cm]{geometry}\n");
    INSERT_TO_LATEX("\\usepackage{fancyhdr}\n");
    INSERT_TO_LATEX("\\usepackage{wrapfig}\n");
    INSERT_TO_LATEX("\\usepackage{caption}\n");
    INSERT_TO_LATEX("\\usepackage{titlesec}\n");
    INSERT_TO_LATEX("\n");
    
    INSERT_TO_LATEX("\\titleformat{\\section}\n");
    INSERT_TO_LATEX("  {\\normalfont\\fontsize{14}{16}\\bfseries}{\\thesection}{1em}{}\n");
    INSERT_TO_LATEX("\n");
    
    INSERT_TO_LATEX("\\begin{document}\n");
    INSERT_TO_LATEX("\\thispagestyle{empty}\n");
    INSERT_TO_LATEX("\\vspace*{80px}\n");
    
    INSERT_TO_LATEX("\\begingroup\n");
    INSERT_TO_LATEX("\\fontsize{16pt}{14pt}\n");
    INSERT_TO_LATEX("\\selectfont\n");
    INSERT_TO_LATEX("\\begin{center}\n");
    INSERT_TO_LATEX("Московский физико-технический институт\\\\\n");
    INSERT_TO_LATEX("\\textbf{\"Нахождение производной функции\"}\n");
    INSERT_TO_LATEX("\\end{center}\n");
    INSERT_TO_LATEX("\\endgroup\n");
    
    INSERT_TO_LATEX("\\begin{center}\n");
    INSERT_TO_LATEX("\\normalsize\n");
    INSERT_TO_LATEX("\\textit{Муругов А.Р., студент 1 курса ФРКТ}\\\\\n");
    INSERT_TO_LATEX("\\today\n");
    INSERT_TO_LATEX("\\end{center}\n");
    
    INSERT_TO_LATEX("\\newpage\n");
    INSERT_TO_LATEX("\\pagestyle{fancy}\n");
    INSERT_TO_LATEX("\\fancyhead{}\n");
    INSERT_TO_LATEX("\\fancyhead[LO]{\\text{Нахождение производной функции}\\hfill}\n");
    INSERT_TO_LATEX("\n");
    
    INSERT_TO_LATEX("\\begingroup\n");
    INSERT_TO_LATEX("\\fontsize{16pt}{14pt}\n");
    INSERT_TO_LATEX("\\section{Цели работы:}\n");
    INSERT_TO_LATEX("\\normalsize\n");
    INSERT_TO_LATEX("\\begin{enumerate}\n");
    INSERT_TO_LATEX("    \\item Найти производную функции\n");
    INSERT_TO_LATEX("\\end{enumerate}\n");
    INSERT_TO_LATEX("\\endgroup\n");
    INSERT_TO_LATEX("\n");
    
    INSERT_TO_LATEX("\\begingroup\n");
    INSERT_TO_LATEX("\\fontsize{16pt}{14pt}\n");
    INSERT_TO_LATEX("\\renewcommand{\\baselinestretch}{1.5}\n");
    INSERT_TO_LATEX("\\section{Оборудование:}\n");
    INSERT_TO_LATEX("\\normalsize\n");
    INSERT_TO_LATEX("\\begin{enumerate}\n");
    INSERT_TO_LATEX("    \\item Мой любимый Mac\n");
    INSERT_TO_LATEX("\\end{enumerate}\n");
    INSERT_TO_LATEX("\\endgroup\n");
    
    INSERT_TO_LATEX("\\begingroup\n");
    INSERT_TO_LATEX("\\renewcommand{\\baselinestretch}{1.5}\n");
    INSERT_TO_LATEX("\\section{Теоретическая часть}\n");
    INSERT_TO_LATEX("\\normalsize\n");

    return WOLF_SUCCESS;
}

WolfErr_t LatexFileCloser()
{
    INSERT_TO_LATEX("\\endgroup\n");
    INSERT_TO_LATEX("\n");
    
    INSERT_TO_LATEX("\\begingroup\n");
    INSERT_TO_LATEX("\\fontsize{16pt}{14pt}\n");
    INSERT_TO_LATEX("\\renewcommand{\\baselinestretch}{1.5}\n");
    INSERT_TO_LATEX("\\section{Список используемой литературы}\n");
    INSERT_TO_LATEX("\\normalsize\n");
    INSERT_TO_LATEX("\\begin{enumerate}\n");
    INSERT_TO_LATEX("    \\item А.Ю. Петрович \"Лекции по математическому анализу\"\n");
    INSERT_TO_LATEX("    \\item Л.Д. Кудрявцев \"Сборник задач по математическому анализу\"\n");
    INSERT_TO_LATEX("    \\item Мои записи с семинаров по матану\n");
    INSERT_TO_LATEX("\\end{enumerate}\n");
    INSERT_TO_LATEX("\\endgroup\n");
    
    INSERT_TO_LATEX("\n");
    INSERT_TO_LATEX("\\end{document}\n");

    fclose(file_latex);

    return WOLF_SUCCESS;
}