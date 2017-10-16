//Jacob Katzeff :: jkatzeff@ucsc.edu :: 1426015
//
//Chris Hsiao :: chhsiao@ucsc.edu :: 1398305

#ifndef __CODE_GEN__
#define __CODE_GEN__

#include <bitset>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>


#include "auxlib.h"
#include "astree.h"
#include "sym_table.h"


// return appropriate oil type, or could print directly to oil file
string mangle_struct(astree* node);
string mangle_ident(astree* node);
string new_vreg(char type);

void emit_oil(FILE* file, astree* root);

#endif
