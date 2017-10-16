#ifndef __SYM_TABLE__
#define __SYM_TABLE__

#include <bitset>
#include <string.h>
#include <vector>
#include <unordered_map>

#include "auxlib.h"
#include "astree.h"

enum { ATTR_void, ATTR_int, ATTR_null, ATTR_string, 
	ATTR_struct, ATTR_array, ATTR_function, ATTR_variable, 
	ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval, ATTR_const, 
	ATTR_vreg, ATTR_vaddr, ATTR_bitset_size,
};
using attr_bitset = bitset<ATTR_bitset_size>;

struct astree;
struct symbol; 
using symbol_table = unordered_map<const string*,symbol*>;
using symbol_entry = symbol_table::value_type;

struct symbol {
	attr_bitset attributes;
	symbol_table* fields;
	size_t filenr, linenr, offset;
	size_t block_nr;
	vector<symbol*>* parameters;
	const string* structtype;
	const string* fieldtype;
};

symbol* new_symbol(astree* node);
const char* check_attr(astree* node);
void set_attributes(astree* node);
void insert_sym(symbol_table table, const string *key,
				symbol *sym, astree* node);
void make_block(astree* root);
void trav_attr(astree* root);
void make_vardecl(astree *root);
void ins_struct_sym(astree *root);
void ins_func_sym(astree *root);
void ins_proto_sym(astree *root);
void traversal(astree *root);
void check_fieldop(astree *root);
#endif
