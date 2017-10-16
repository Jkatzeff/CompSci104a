//Jacob Katzeff :: jkatzeff@ucsc.edu :: 1426015
//
//Chris Hsiao :: chhsiao@ucsc.edu :: 1398305
// $Id: astree.h,v 1.10 2016-10-06 16:42:35-07 - - $

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <bitset>
#include <string>
#include <vector>
using namespace std;

#include "auxlib.h"
#include "sym_table.h"
using attr_bitset = bitset<15>;
struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

struct astree {

   // Fields.
   int symbol;               // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node
   string vreg;             // virtual register string

   attr_bitset attributes;   // attributes of this node for sym table
   int block_nr;             // which block, also for sym table
   const string* structtype; // struct type
   const string* functype;   // function type;
   const string* fieldtype;  // if its a field
   // Functions.
   astree (int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt (astree* child1, astree* child2 = nullptr);
   astree* adopt_sym (astree* child, int symbol);
   astree* adopt_sym_child (astree* child, int symbol);
   void dump_node (FILE*);
   void dump_tree (FILE*, int depth = 0);
   static void dump (FILE* outfile, astree* tree);
   static void print (FILE* outfile, astree* tree, int depth = 0);
};

extern vector<astree*> stringcons;

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif

