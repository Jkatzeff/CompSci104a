head	1.7;
access;
symbols;
locks;
comment	@ * @;


1.7
date	2016.10.06.23.13.39;	author -;	state -;
branches;
next	1.6;

1.6
date	2016.09.22.00.13.03;	author -;	state -;
branches;
next	1.5;

1.5
date	2016.04.07.20.04.52;	author -;	state -;
branches;
next	1.4;

1.4
date	2015.04.10.02.31.47;	author -;	state -;
branches;
next	1.3;

1.3
date	2015.04.10.00.45.26;	author -;	state -;
branches;
next	1.2;

1.2
date	2013.10.12.01.52.46;	author -;	state -;
branches;
next	1.1;

1.1
date	2012.10.10.01.32.37;	author -;	state -;
branches;
next	;


desc
@@


1.7
log
@-
@
text
@// $Id: astree.h,v 1.6 2016-09-21 17:13:03-07 - - $

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
using namespace std;

#include "auxlib.h"

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

   // Functions.
   astree (int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt (astree* child1, astree* child2 = nullptr);
   astree* adopt_sym (astree* child, int symbol);
   void dump_node (FILE*);
   void dump_tree (FILE*, int depth = 0);
   static void dump (FILE* outfile, astree* tree);
   static void print (FILE* outfile, astree* tree, int depth = 0);
};

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif

@


1.6
log
@-
@
text
@d1 1
a1 1
// $Id: astree.h,v 1.9 2015-07-09 18:25:23-07 - - $
d10 1
a10 1
#include "syslib.h"
@


1.5
log
@-
@
text
@d1 2
d10 7
a16 1
#include "auxlib.h"
d19 2
d22 1
a22 3
   size_t filenr;            // index into filename stack
   size_t linenr;            // line number from source code
   size_t offset;            // offset of token with current line
d25 10
a34 2
   astree (int symbol, int filenr, int linenr,
           int offset, const char* clexinfo);
d37 1
a37 15
// Append one child to the vector of children.
astree* adopt1 (astree* root, astree* child);

// Append two children to the vector of children.
astree* adopt2 (astree* root, astree* left, astree* right);

// Dump an astree to a FILE.
void dump_astree (FILE* outfile, astree* root);

// Debug print an astree.
void yyprint (FILE* outfile, unsigned short toknum,
              astree* yyvaluep);

// Recursively free an astree.
void free_ast (astree* tree);
d39 1
a39 2
// Recursively free two astrees.
void free_ast2 (astree* tree1, astree* tree2);
@


1.4
log
@-
@
text
@a39 1
RCSH("$Id: astree.h,v 1.3 2015-04-09 17:45:26-07 - - $")
d41 1
@


1.3
log
@-
@
text
@d17 2
a20 5

// Create and initialize a new childless astree.
astree* new_astree (int symbol, int filenr, int linenr,
                    int offset, const char* lexinfo);

a26 3
// Adopt one child and change the symbol in the root.
astree* adopt1sym (astree* root, astree* child, int symbol);

d40 1
a40 1
RCSH("$Id: astree.h,v 1.2 2013-10-11 18:52:46-07 - - $")
@


1.2
log
@-
@
text
@d20 1
d23 2
d26 2
d29 2
d32 2
d35 2
d39 2
d42 2
d46 1
a46 1
RCSH("$Id: astree.h,v 1.3 2013-09-20 12:23:31-07 - - $")
@


1.1
log
@-
@
text
@d4 4
d10 8
a17 1
typedef struct astree_rep *astree;
a18 10
bool is_astree (void *object);
astree new_astree (int symbol, int filenr, int linenr, int offset,
                   char *lexinfo);
astree adopt (astree root, /*ASTree*/ ... /*, NULL */);
astree adopt1 (astree root, astree child);
astree adopt2 (astree root, astree left, astree right);
astree adopt1sym (astree root, astree child, int symbol);
void dump_astree (FILE *outfile, astree root);
void yyprint (FILE *outfile, unsigned short toknum, astree yyvaluep);
void freeast (astree tree);
d20 10
a29 1
#define freeast2(T1,T2) { freeast (T1); freeast (T2); }
d31 1
a31 2
// LINTED(static unused)
RCSH(ASTREE_H,"$Id: astree.h,v 1.6 2011-10-06 22:05:30-07 - - $")
@
