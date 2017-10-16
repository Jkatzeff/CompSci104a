#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <bitset>
#include <string.h>

#include "sym_table.h"
#include "astree.h"
#include "auxlib.h"
#include "lyutils.h"
#include "string_set.h"
//To do: figure out how to make it ok to reference structs and fields in diff blocks


//*symbol_stack[0] is the symbol_table with struct "node" for example
//*(symbol_stack[0])['c'] gives symbol table for "node"
//(*(symbol_stack[0])['c']->fields)['link'] gives symbol table for "node"

vector<symbol_table*> symbol_stack={new symbol_table,nullptr};
int next_block = 1;
vector<int> block_count={0};

symbol* new_symbol(astree* node){
	// if(symbol_stack.empty()){
	// 	symbol_stack.push_back(new symbol_table);
	// 	symbol_stack.push_back(nullptr);
	// }
	symbol *sym = new symbol();

	sym->fields=nullptr;
	sym->parameters=nullptr;
	sym->structtype=nullptr;
	sym->fieldtype=nullptr;
	sym->attributes=node->attributes;
	sym->block_nr=block_count.back();
	sym->filenr=node->lloc.filenr;
	sym->linenr=node->lloc.linenr;
	sym->offset=node->lloc.offset;
	return sym;
}
	// else if(sym=='='){
	// 	const string* identname= (node->children[0]->lexinfo);
	// 	// symbol_table symtable = *symbol_stack[0];
	// 	printf("hi!\n");
	// 	symbol_table symtbl = *symbol_stack[0];
	// 	printf("hi!\n\n");
	// 	symbol* symx =symtbl[identname];
	// 	printf("hi!\n");
	// 	if(symx->structtype){
	// 		node->children[0]->attributes[ATTR_struct]=true;
	// 		node->children[0]->structtype=symx->structtype;
	// 	}
	// }
const char* check_attr(astree* node){
	// printf("%s\n",parser::get_tname(node->symbol));
	attr_bitset attr_arr= node->attributes;
	string s = "";
	// if(attr_arr[ATTR_ident]){
	// 	const string* identname=node->lexinfo;
	// 	symbol_table symtable=*symbol_stack[0];
	// 	symbol* symx = symtable[identname];
	// 	if(symx->structtype){
	// 		printf("hi!\n");
	// 		node->attributes[ATTR_struct]=true;
	// 		node->children[0]->structtype=symx->structtype;
	// 	}
	// }
	if(attr_arr[ATTR_field]){
		s+="field ";
		if(node->fieldtype){
			s=s+"{"+*(node->fieldtype)+"} ";
		}
	}
	if(attr_arr[ATTR_void]){
		s+="void ";
	}
	if(attr_arr[ATTR_int]){
		s+="int ";
	}
	if(attr_arr[ATTR_null]){
		s+="null ";
	}
	if(attr_arr[ATTR_string]){
		s+="string ";
	}
	if(attr_arr[ATTR_struct]){
		s+="struct ";
		if(node->structtype){
			s=s+"\"" +*(node->structtype)+"\" ";	
		}
	}
	if(attr_arr[ATTR_array]){
		s+="array ";
	}
	if(attr_arr[ATTR_function]){
		s+="function ";
	}
	if(attr_arr[ATTR_variable]){
		s+="variable ";
	}
	if(attr_arr[ATTR_typeid]){
		s+="typeid ";
	}
	if(attr_arr[ATTR_lval]){
		s+="lval ";
	}
	if(attr_arr[ATTR_param]){
		s+="param ";
	}
	if(attr_arr[ATTR_const]){
		s+="const ";
	}
	return s.c_str();
}
void set_attributes(astree* node){
	int sym = node->symbol;
	// printf("%s, %s\n",parser::get_tname(sym),node->lexinfo->c_str());
	if(sym==TOK_VOID){
		node->attributes[ATTR_void]=true;
		if(node->children.size()>0){
			node->children[0]->attributes[ATTR_void]=true;
		}
	}
	else if(sym==TOK_INT){
		node->attributes[ATTR_int]=true;
		node->attributes[ATTR_const]=true;
		if(node->children.size()>0){
			node->children[0]->attributes[ATTR_int]=true;
			node->children[0]->structtype=node->lexinfo;
		}
	}
	else if(sym==TOK_CHAR){
		//no char attribute?
	}
	else if(sym==TOK_NULL){
		node->attributes[ATTR_null]=true;
		node->attributes[ATTR_const]=true;
	}
	else if(sym==TOK_STRING){
		node->attributes[ATTR_string]=true;
		node->attributes[ATTR_lval]=true;
		if(node->children.size()>0){
			node->children[0]->attributes[ATTR_string]=true;
		}
	}
	else if(sym==TOK_STRUCT){
		node->attributes[ATTR_struct]=true;
		node->structtype=node->children[0]->lexinfo;
		node->attributes[ATTR_variable]=false;
		node->attributes[ATTR_lval]=false;

		node->children[0]->attributes[ATTR_struct]=true;
		node->children[0]->attributes[ATTR_variable]=false;
		node->children[0]->attributes[ATTR_lval]=false;
	}
	else if(sym==TOK_ARRAY){
		node->attributes[ATTR_array]=true;
	}
	else if(sym==TOK_FUNCTION){
		node->attributes[ATTR_function]=true;
		node->attributes[ATTR_variable]=false;
		node->attributes[ATTR_lval]=false;

		node->children[0]->attributes[ATTR_function]=true;
		node->children[0]->attributes[ATTR_variable]=false;
		node->children[0]->attributes[ATTR_lval]=false;
	}
	else if(sym==TOK_VARDECL){
		node->attributes[ATTR_variable]=true;
	}
	// else if(sym==('.')){
	// 	//look up child 0, see if its a valid identifier
	// 	//look up child 1, see if its a valid field
	// 	//when the ast finds something like c=c.link,
	// 	//need to be able to say what type link is
	// 	auto stk = *symbol_stack[block_count.back()];
	// 	symbol* c1=stk[node->children[0]->lexinfo];
	// 	symbol* c2=stk[node->children[1]->lexinfo];
	// 	if(c1!=NULL && c2!=NULL){
	// 		for(size_t i=0;i<c1->parameters->size();i++){
	// 			if(c1->parameters[i]->lexinfo==node->children[1]->lexinfo){
	// 				node->children[1]->structtype=c1->parameters[i]->structtype;
	// 				node->children[1]->fieldtype=c1->parameters[i]->fieldtype;
	// 			}
	// 		}
	// 	}
	// }
	else if(sym==('.')){
		node->children[0]->attributes[ATTR_struct]=true;
		// const string unsuretype0="unsure";
		// node->children[0]->structtype=&unsuretype0;
		node->children[1]->attributes[ATTR_field]=true;
		// const string unsurefield1="unsure";
		// const string unsuretype1="unsure";
		// node->children[1]->fieldtype=&unsurefield1;
		// node->children[1]->structtype=&unsuretype1;
		// symbol_table* base_tbl=symbol_stack[0];
		// auto c1=base_tbl->find(node->children[0]->lexinfo);
		// if(c1 != base_tbl->end()){
		// 	node->children[0]->attributes[ATTR_struct]=true;
		// 	node->children[0]->structtype=c1->second->structtype;
		// }
		// printf("%s\n\n",c1->second->structtype->c_str());
		// printf("%s\n\n",node->children[0]->lexinfo->c_str());
		// if(c1->second){
		// 	printf("%s\n\n",c1->second->structtype->c_str());
		// }
		// // if(c1!=base_tbl->end() and c1->second->structtype!=nullptr){
			// printf("hi!\n\n");
			
		// }
	}
	else if(sym==TOK_FIELD){
		node->attributes[ATTR_field]=true;
		// node->attributes[ATTR_struct]=true;
	}
	else if(sym==TOK_TYPEID){
		node->attributes[ATTR_typeid]=true;
		for(size_t i=0;i<node->children.size();i++){
			node->children[i]->attributes[ATTR_struct]=true;
			node->children[i]->structtype=node->lexinfo;
		}
	}
	else if(sym==TOK_PARAMLIST){
		for(size_t i=0;i<node->children.size();i++){
			node->children[i]->attributes[ATTR_param]=true;
		}
	}
	//bc no specification for distinction between char & int?
	else if(sym==TOK_INTCON || sym==TOK_CHARCON){
		node->attributes[ATTR_int]=true;
		node->attributes[ATTR_const]=true;
		node->attributes[ATTR_lval]=true;
	}
	else if(sym==TOK_STRINGCON){
		node->attributes[ATTR_string]=true;
		node->attributes[ATTR_const]=true;
		node->attributes[ATTR_lval]=true;
	}
	else if(sym==TOK_DECLID){
		node->attributes[ATTR_variable]=true;
		node->attributes[ATTR_lval]=true;
	}
	else if(sym==TOK_IDENT){
		node->attributes[ATTR_variable]=true;
		node->attributes[ATTR_lval]=true;
		// node->attributes[ATTR_ident]=true;
	}
	else if(sym==TOK_PROTOTYPE){
		node->children[0]->children[0]->attributes[ATTR_variable]=false;
		node->children[0]->children[0]->attributes[ATTR_lval]=false;
	}

}
void insert_sym(symbol_table table, const string* key,
				symbol* sym, astree* node){
	table[key]=sym;
	for(size_t i=1;i<block_count.size(); i++){
		fprintf(sym_file, "  ");
	}
	fprintf(sym_file, "%s (%zd.%zd.%zd) {%zd} %s\n",
			key->c_str(),sym->filenr, sym->linenr, sym->offset,
			sym->block_nr,check_attr(node));
}
void make_block(astree* root){
	block_count.push_back(next_block);
	next_block++;
	symbol_stack[block_count.back()]=new symbol_table;
	symbol_stack.push_back(nullptr);
	traversal(root);
	block_count.pop_back();
}
void trav_attr(astree* root){
	for(size_t i=0;i<root->children.size();i++){
		trav_attr(root->children[i]);
	}
	set_attributes(root);
}
void make_vardecl(astree *root){
	astree *vardecl = root->children[0]->children[0];
	vardecl->structtype=root->children[0]->lexinfo;
	symbol* sym = new_symbol(vardecl);
	sym->structtype=vardecl->structtype;
	insert_sym(*symbol_stack[block_count.back()], vardecl->lexinfo,
		sym, vardecl);
}
void ins_struct_sym(astree* root){
	if(root->children.size()>0){
		root->children[0]->structtype=root->children[0]->lexinfo;
		root->children[0]->attributes[ATTR_typeid]=false;
	}
	symbol* sym = new_symbol(root->children[0]);
	symbol_table fieldloc;
	sym->fields=&fieldloc;
	sym->structtype=root->children[0]->lexinfo;
	insert_sym(*symbol_stack[0], root->children[0]->lexinfo,
				sym,root->children[0]);
	for(size_t i=0;i<root->children[1]->children.size();i++){
		fprintf(sym_file,"  ");
		astree* field = root->children[1]->children[i]->children[0];
		field->fieldtype=root->children[0]->lexinfo;
		field->structtype=root->children[1]->children[i]->lexinfo;
		sym=new_symbol(field);
		insert_sym(fieldloc,field->lexinfo,sym,field);
	}
}
void ins_func_sym(astree *root){
	root->children[0]->children[0]->functype=root->children[0]->lexinfo;
	root->children[0]->children[0]->attributes[ATTR_function]=true;
	root->children[0]->children[0]->attributes[ATTR_lval]=false;
	root->children[0]->children[0]->attributes[ATTR_variable]=false;
	symbol *sym = new_symbol(root->children[0]->children[0]);
	vector<symbol*> paramloc;
	sym->parameters=&paramloc;
	insert_sym(*symbol_stack[0],
		root->children[0]->children[0]->lexinfo,sym,
		root->children[0]->children[0]);
	astree* params = root->children[1];
	for(size_t i=0;i<params->children.size();i++){
		params->children[i]->children[0]->structtype=params->children[i]->lexinfo;
		params->children[i]->children[0]->attributes[ATTR_param]=true;
		sym=new_symbol(params->children[i]->children[0]);
		sym->structtype=params->children[i]->children[0]->structtype;
		sym->block_nr++;
		paramloc.push_back(sym);
		fprintf(sym_file,"  ");
		insert_sym(*symbol_stack[0],
			params->children[i]->children[0]->lexinfo,sym,
			params->children[i]->children[0]);
	}
	make_block(root->children[2]);
}
void ins_proto_sym(astree *root){
	root->children[0]->children[0]->functype=root->children[0]->lexinfo;
	root->children[0]->children[0]->attributes[ATTR_function]=true;
	root->children[0]->children[0]->attributes[ATTR_lval]=false;
	root->children[0]->children[0]->attributes[ATTR_variable]=false;
	symbol *sym = new_symbol(root->children[0]->children[0]);
	vector<symbol*> paramloc;
	sym->parameters=&paramloc;
	insert_sym(*symbol_stack[0], 
			root->children[0]->children[0]->lexinfo, sym,
			root->children[0]->children[0]);
	astree* params = root->children[1];
	for(size_t i=0;i<params->children.size();i++){
		params->children[i]->children[0]->structtype=params->children[i]->lexinfo;
		params->children[i]->children[0]->attributes[ATTR_param]=true;
		sym = new_symbol(params->children[i]->children[0]);
		sym->block_nr++;
		paramloc.push_back(sym);
	}
}
void check_fieldop(astree *root){
	symbol_table* base_tbl=symbol_stack[0];
	auto c1=base_tbl->find(root->children[0]->lexinfo);
	if(c1 != base_tbl->end()){
		root->children[0]->attributes[ATTR_struct]=true;
		root->children[0]->structtype=c1->second->structtype;
		root->children[1]->attributes[ATTR_field]=true;
		root->children[1]->fieldtype=c1->second->structtype;
	}
}
		// symbol_table* base_tbl=symbol_stack[0];
		// auto c1=base_tbl->find(node->children[0]->lexinfo);
		// if(c1 != base_tbl->end()){
		// 	node->children[0]->attributes[ATTR_struct]=true;
		// 	node->children[0]->structtype=c1->second->structtype;
		// }
		// printf("%s\n\n",c1->second->structtype->c_str());
		// printf("%s\n\n",node->children[0]->lexinfo->c_str());
		// if(c1->second){
		// 	printf("%s\n\n",c1->second->structtype->c_str());
		// }
		// // if(c1!=base_tbl->end() and c1->second->structtype!=nullptr){
			// printf("hi!\n\n");
			

void traversal(astree *root){
	for(size_t i=0;i<root->children.size();i++){
		int sym = root->children[i]->symbol;
		// printf("index: %zu children.size: %zu sym: %d ",i,root->children.size(), sym);
		// printf("symbol: %s \n\n",parser::get_tname(sym));
		switch(sym){
			case '.':
				check_fieldop(root->children[i]);
				fprintf(sym_file, "\n");
			case TOK_STRUCT:
				ins_struct_sym(root->children[i]);
				fprintf(sym_file, "\n");
				break;
			case TOK_FUNCTION:
				ins_func_sym(root->children[i]);
				fprintf(sym_file, "\n");
				break;
			case TOK_VARDECL:
				make_vardecl(root->children[i]);
				break;
			case TOK_PROTOTYPE:
				ins_proto_sym(root->children[i]);
				fprintf(sym_file, "\n");
				break;
			case TOK_IF:
				make_block(root->children[i]->children[1]);
				break;
			case TOK_IFELSE:
				make_block(root->children[i]->children[1]);
				make_block(root->children[i]->children[2]);
				break;
			default:
				break;
		}
	}
}
