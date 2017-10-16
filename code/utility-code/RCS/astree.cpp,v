head	1.8;
access;
symbols;
locks;
comment	@// @;


1.8
date	2016.09.22.00.13.03;	author -;	state -;
branches;
next	1.7;

1.7
date	2016.04.07.20.04.52;	author -;	state -;
branches;
next	1.6;

1.6
date	2015.04.10.02.31.47;	author -;	state -;
branches;
next	1.5;

1.5
date	2015.04.10.02.30.10;	author -;	state -;
branches;
next	1.4;

1.4
date	2015.04.10.02.29.20;	author -;	state -;
branches;
next	1.3;

1.3
date	2015.04.10.02.28.15;	author -;	state -;
branches;
next	1.2;

1.2
date	2015.04.10.02.27.42;	author -;	state -;
branches;
next	1.1;

1.1
date	2014.10.04.01.22.05;	author -;	state -;
branches;
next	;


desc
@@


1.8
log
@-
@
text
@// $Id: astree.cpp,v 1.14 2016-08-18 15:05:42-07 - - $

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "string_set.h"
#include "lyutils.h"

astree::astree (int symbol_, const location& lloc_, const char* info) {
   symbol = symbol_;
   lloc = lloc_;
   lexinfo = string_set::intern (info);
   // vector defaults to empty -- no children
}

astree::~astree() {
   while (not children.empty()) {
      astree* child = children.back();
      children.pop_back();
      delete child;
   }
   if (yydebug) {
      fprintf (stderr, "Deleting astree (");
      astree::dump (stderr, this);
      fprintf (stderr, ")\n");
   }
}

astree* astree::adopt (astree* child1, astree* child2) {
   if (child1 != nullptr) children.push_back (child1);
   if (child2 != nullptr) children.push_back (child2);
   return this;
}

astree* astree::adopt_sym (astree* child, int symbol_) {
   symbol = symbol_;
   return adopt (child);
}


void astree::dump_node (FILE* outfile) {
   fprintf (outfile, "%p->{%s %zd.%zd.%zd \"%s\":",
            this, parser::get_tname (symbol),
            lloc.filenr, lloc.linenr, lloc.offset,
            lexinfo->c_str());
   for (size_t child = 0; child < children.size(); ++child) {
      fprintf (outfile, " %p", children.at(child));
   }
}

void astree::dump_tree (FILE* outfile, int depth) {
   fprintf (outfile, "%*s", depth * 3, "");
   dump_node (outfile);
   fprintf (outfile, "\n");
   for (astree* child: children) child->dump_tree (outfile, depth + 1);
   fflush (NULL);
}

void astree::dump (FILE* outfile, astree* tree) {
   if (tree == nullptr) fprintf (outfile, "nullptr");
                   else tree->dump_node (outfile);
}

void astree::print (FILE* outfile, astree* tree, int depth) {
   fprintf (outfile, "; %*s", depth * 3, "");
   fprintf (outfile, "%s \"%s\" (%zd.%zd.%zd)\n",
            parser::get_tname (tree->symbol), tree->lexinfo->c_str(),
            tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
   for (astree* child: tree->children) {
      astree::print (outfile, child, depth + 1);
   }
}

void destroy (astree* tree1, astree* tree2) {
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
}

void errllocprintf (const location& lloc, const char* format,
                    const char* arg) {
   static char buffer[0x1000];
   assert (sizeof buffer > strlen (format) + strlen (arg));
   snprintf (buffer, sizeof buffer, format, arg);
   errprintf ("%s:%zd.%zd: %s", 
              lexer::filename (lloc.filenr), lloc.linenr, lloc.offset,
              buffer);
}
@


1.7
log
@-
@
text
@d1 1
d11 1
a11 1
#include "stringset.h"
d14 5
a18 21
astree::astree (int symbol, int filenr, int linenr,
                int offset, const char* clexinfo):
        symbol (symbol), filenr (filenr), linenr (linenr),
        offset (offset), lexinfo (intern_stringset (clexinfo)) {
   DEBUGF ('f', "astree %p->{%d:%d.%d: %s: \"%s\"}\n",
           (void*) this, filenr, linenr, offset,
           get_yytname (symbol), lexinfo->c_str());
}

astree* adopt1 (astree* root, astree* child) {
   root->children.push_back (child);
   DEBUGF ('a', "%p (%s) adopting %p (%s)\n",
           root, root->lexinfo->c_str(),
           child, child->lexinfo->c_str());
   return root;
}

astree* adopt2 (astree* root, astree* left, astree* right) {
   adopt1 (root, left);
   adopt1 (root, right);
   return root;
d21 10
a30 12

static void dump_node (FILE* outfile, astree* node) {
   fprintf (outfile, "%p->{%s(%d) %ld:%ld.%03ld \"%s\" [",
            node, get_yytname (node->symbol), node->symbol,
            node->filenr, node->linenr, node->offset,
            node->lexinfo->c_str());
   bool need_space = false;
   for (size_t child = 0; child < node->children.size();
        ++child) {
      if (need_space) fprintf (outfile, " ");
      need_space = true;
      fprintf (outfile, "%p", node->children.at(child));
a31 1
   fprintf (outfile, "]}");
d34 19
a52 11
static void dump_astree_rec (FILE* outfile, astree* root,
                             int depth) {
   if (root == NULL) return;
   fprintf (outfile, "%*s%s ", depth * 3, "",
            root->lexinfo->c_str());
   dump_node (outfile, root);
   fprintf (outfile, "\n");
   for (size_t child = 0; child < root->children.size();
        ++child) {
      dump_astree_rec (outfile, root->children[child],
                       depth + 1);
d56 5
a60 2
void dump_astree (FILE* outfile, astree* root) {
   dump_astree_rec (outfile, root, 0);
d64 3
a66 9
void yyprint (FILE* outfile, unsigned short toknum,
              astree* yyvaluep) {
   if (is_defined_token (toknum)) {
      dump_node (outfile, yyvaluep);
   }else {
      fprintf (outfile, "%s(%d)\n",
               get_yytname (toknum), toknum);
   }
   fflush (NULL);
d69 7
a75 6

void free_ast (astree* root) {
   while (not root->children.empty()) {
      astree* child = root->children.back();
      root->children.pop_back();
      free_ast (child);
a76 4
   DEBUGF ('f', "free [%p]-> %d:%d.%d: %s: \"%s\")\n",
           root, root->filenr, root->linenr, root->offset,
           get_yytname (root->symbol), root->lexinfo->c_str());
   delete root;
d79 3
a81 3
void free_ast2 (astree* tree1, astree* tree2) {
   free_ast (tree1);
   free_ast (tree2);
d84 9
@


1.6
log
@-
@
text
@a99 2
RCSC("$Id: astree.cpp,v 1.5 2015-04-09 19:30:10-07 - - $")

@


1.5
log
@-
@
text
@a35 6
astree* adopt1sym (astree* root, astree* child, int symbol) {
   root = adopt1 (root, child);
   root->symbol = symbol;
   return root;
}

d100 1
a100 1
RCSC("$Id: astree.cpp,v 1.4 2015-04-09 19:29:20-07 - - $")
@


1.4
log
@-
@
text
@d14 1
a14 1
                int offset, const char* lexinfo):
d16 1
a16 1
        offset (offset), lexinfo (intern_stringset (lexinfo)) {
d106 1
a106 1
RCSC("$Id: astree.cpp,v 1.3 2015-04-09 19:28:15-07 - - $")
@


1.3
log
@-
@
text
@d18 2
a19 2
           tree, tree->filenr, tree->linenr, tree->offset,
           get_yytname (tree->symbol), tree->lexinfo->c_str());
d106 1
a106 1
RCSC("$Id: astree.cpp,v 1.2 2015-04-09 19:27:42-07 - - $")
@


1.2
log
@-
@
text
@d13 2
a14 2
astree (int symbol, int filenr, int linenr,
        int offset, const char* lexinfo):
d106 1
a106 1
RCSC("$Id: astree.cpp,v 1.1 2014-10-03 18:22:05-07 - - $")
@


1.1
log
@-
@
text
@d13 4
a16 8
astree* new_astree (int symbol, int filenr, int linenr,
                    int offset, const char* lexinfo) {
   astree* tree = new astree();
   tree->symbol = symbol;
   tree->filenr = filenr;
   tree->linenr = linenr;
   tree->offset = offset;
   tree->lexinfo = intern_stringset (lexinfo);
a19 1
   return tree;
d106 1
a106 1
RCSC("$Id: astree.cc,v 1.1 2013-10-11 18:52:46-07 - - $")
@
