#include <string.h>
#include <stdlib.h>
#include <ctype.h>  // isspace
#include <stdio.h>

#include "lex.h"

// wrapper for strcmp
static
int
ischar(char* s, char* cmp)
{

    char c[2] = "x"; // adds null terminator
    c[0] = *s;
    return !strcmp(c, cmp);
}

static
int
isvar(char* s)
{
    // valid variables:
    // {1..8,T, F}
    return ischar(s, "T") || ischar(s, "F") || (isdigit(*s) && 0 < atol(s) && atol(s) < 9);
}

// Read operation from input in bounds {ii, nn}
static
char*
read_op(char* input, int ii, int nn)
{
    char* validops[7] = {"and", "nand", "or", "nor", "xor", "xnor", "not"};
    char* op = malloc(4);
    // this is reading into unverified piece of input char*,
    // only invalid if user input is invalid (ops before args)
    memcpy(op, input, 4);

    for (int ii = 0; ii < 7; ii++) {
        // only check if first two letters are valid
        // (no operators share first two letters)
        // not ideal for input verification
        if (!strncmp(validops[ii], op, 2)) {
            // truncate string to appropriate length
            op[strlen(validops[ii])] = 0;
            return op;
        }
    }

    fprintf(stderr, "Invalid input: %s\n", op);
    exit(1);
}


sexp_ast*
make_sexp_ast(void)
{
    sexp_ast* node = malloc(sizeof(sexp_ast));
    node->left = 0;
    node->right = 0;
    return node;
}

void
free_sexp_ast(sexp_ast* node)
{
    if (node != 0) {
        free_sexp_ast(node->left);
        free_sexp_ast(node->right);
        free(node);
    }
}

char*
print_sexp_ast(sexp_ast* tree)
{
    char buf[1024] = "";
    char lstr[8] = "";
    char rstr[8] = "";
    
    if (tree->type == 1) { // op 
        if (tree->left)  { strcat(lstr, print_sexp_ast(tree->left)); }
        if (tree->right) { strcat(rstr, print_sexp_ast(tree->right)); }

        strcat(buf, "(");
        strcat(buf, tree->data);
        strcat(buf, " ");
        strcat(buf, lstr);
        strcat(buf, " ");
        strcat(buf, rstr);
        strcat(buf, ")");
    }
    if (tree->type == 1) { // atom

    }
    printf("%s\n", buf);
    return buf;
}

sexp_ast*
lex(char* input)
{
    sexp_ast *ast = make_sexp_ast();    // pointer to root of tree
    sexp_ast *head = ast; // current pointer into ast
    int depth = 0; // nested depth of parens

    int nn = strlen(input);
    int ii = 0;      // index in input string
    while (ii < nn) {

        if (isspace(input[ii])) {
            ii++;
            continue;
        }
        
        if (ischar(&input[ii], "(")) {
            ii++;
            depth++;
            continue;
        }

        if (ischar(&input[ii], ")")) {
            ii++;
            depth--;
            continue;
        }

        if (isvar(&input[ii])) {
            // leaves, non-recursive

            char* var = malloc(sizeof(char));
            strncpy(var, &input[ii], 1);
            var[1] = 0;
            ii++;

            if(!head->left) {
                head->left = make_sexp_ast();
                head->left->data = var;
            } else {
                head->right = make_sexp_ast();
                head->right->data = var;
            }
            continue;
        }
        
        char* op = read_op(&input[ii], ii, nn);
        // printf("%s\n", op);
        ii += strlen(op);
        head->data = op;
    }

    // unbalanced input sexp
    // unbalanced != incorrect lex, but prevents ambiguity
    if (depth) {
        fprintf(stderr, "Unbalanced sexp.\n");
        exit(1);
    }
    print_sexp_ast(ast);
    return ast;
}