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

vector<symbol_table*> symbol_stack = {new symbol_table, nullptr};

int next_block = 1;
vector<int> block_count={0};

symbol* new_symbol(astree* node){
    symbol *sym = new symbol();
    sym->fields = nullptr;
    sym->parameters = nullptr;
    sym->structtype = nullptr;
    sym->fieldtype = nullptr;
    sym->attributes = node->attributes;
    sym->block_nr = block_count.back();
    sym->filenr = node->lloc.filenr;
    sym->linenr = node->lloc.linenr;
    sym->offset = node->lloc.offset;
    return sym;
}

const char* check_attr(astree* node){
    attr_bitset attr_arr= node->attributes;
    string s = "";
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
        }
    }
    else if(sym==TOK_CHAR){
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
    else if(sym==TOK_FIELD){
        node->attributes[ATTR_field]=true;
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
    }
    else if(sym==TOK_PROTOTYPE){
        node->children[0]->children[0]->attributes[ATTR_variable]=false;
        node->children[0]->children[0]->attributes[ATTR_lval]=false;
    }

}

void insert_sym(symbol_table *table, const string* key,
                symbol* sym, astree* node){
    table->insert({key,sym});
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
    if(root->children[0]->lexinfo->compare("int")!=0 and
        root->children[0]->lexinfo->compare("string")!=0){
            vardecl->structtype=root->children[0]->lexinfo;
    }
    symbol* sym = new_symbol(vardecl);
    if(root->children[0]->lexinfo->compare("int")!=0 and
        root->children[0]->lexinfo->compare("string")!=0){
        sym->structtype=vardecl->structtype;
    }
    insert_sym(symbol_stack[block_count.back()], vardecl->lexinfo,
        sym, vardecl);
}
void ins_struct_sym(astree* root){
    if(root->children.size()>0){
        root->children[0]->structtype=root->children[0]->lexinfo;
        root->children[0]->attributes[ATTR_typeid]=false;
    }
    symbol* sym = new_symbol(root->children[0]);
    symbol_table *fieldloc = new symbol_table;
    sym->structtype=root->children[0]->lexinfo;
    insert_sym(symbol_stack[0], root->children[0]->lexinfo,
                sym,root->children[0]);
    if(root->children[1]==nullptr){ return; }
    for(size_t i=0;i<root->children[1]->children.size();i++){
        fprintf(sym_file,"  ");
        astree* field = root->children[1]->children[i]->children[0];
        field->fieldtype=root->children[0]->lexinfo;
        field->structtype=root->children[1]->children[i]->lexinfo;
        symbol* sym2=new_symbol(field);
        sym2->structtype=root->children[1]->children[i]->lexinfo;
        insert_sym(fieldloc,field->lexinfo,sym2,field);
    }
    sym->fields=fieldloc;
}

void ins_func_sym(astree *root){
    root->children[0]->children[0]->functype=root->children[0]->lexinfo;
    root->children[0]->children[0]->attributes[ATTR_function]=true;
    root->children[0]->children[0]->attributes[ATTR_lval]=false;
    root->children[0]->children[0]->attributes[ATTR_variable]=false;
    symbol *sym = new_symbol(root->children[0]->children[0]);
    vector<symbol*> paramloc;
    sym->parameters=&paramloc;
    insert_sym(symbol_stack[0],
        root->children[0]->children[0]->lexinfo,sym,
        root->children[0]->children[0]);
    astree* params = root->children[1];
    for(size_t i=0;i<params->children.size();i++){
        params->children[i]->children[0]->structtype = 
        params->children[i]->lexinfo;
        params->children[i]->children[0]->attributes[ATTR_param]=true;
        sym=new_symbol(params->children[i]->children[0]);
        sym->structtype=params->children[i]->children[0]->structtype;
        sym->block_nr++;
        paramloc.push_back(sym);
        fprintf(sym_file,"  ");
        insert_sym(symbol_stack[0],
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
    insert_sym(symbol_stack[0], 
            root->children[0]->children[0]->lexinfo, sym,
            root->children[0]->children[0]);
    astree* params = root->children[1];
    for(size_t i=0;i<params->children.size();i++){
        params->children[i]->children[0]->structtype = 
                        params->children[i]->lexinfo;
        params->children[i]->children[0]->attributes[ATTR_param]=true;
        sym = new_symbol(params->children[i]->children[0]);
        sym->block_nr++;
        paramloc.push_back(sym);
    }
}
void check_fieldop(astree *root){
    symbol_table* base_tbl=symbol_stack[0];
    auto symbol_match=base_tbl->find(root->children[0]->lexinfo);
    if(symbol_match != base_tbl->end()){
        if(!symbol_match->second->structtype){
            fprintf(stderr, "Tried to use '.' wrongly\n");
            exit(EXIT_FAILURE);            
        }
        else if(symbol_match->second->structtype->compare("int")==0
            ){
            fprintf(stderr, "Tried to use '.' with an int!\n");
            exit(EXIT_FAILURE);            
        }
        else{
            root->children[0]->attributes[ATTR_struct]=true;
        }
        root->children[0]->structtype=symbol_match->second->structtype;
        if(symbol_match->second->fields==nullptr){ return; }
    }
}

void traversal(astree *root){
    for(size_t i=0;i<root->children.size();i++){
        int sym = root->children[i]->symbol;
        switch(sym){
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
