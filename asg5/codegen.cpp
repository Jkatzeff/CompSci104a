//Jacob Katzeff :: jkatzeff@ucsc.edu :: 1426015
//
//Chris Hsiao :: chhsiao@ucsc.edu :: 1398305

#include "codegen.h"
#include "sym_table.h"
#include "astree.h"
#include "lyutils.h"
#include "auxlib.h"
#include <cstdlib>
#include <cstring>

size_t int_count = 0;
size_t ind_ptr_count = 0;
size_t ptr_count = 0;
size_t string_count = 0;

extern vector<symbol_table*> symbol_stack;
void emit_node(FILE* file, astree* node);
void emit_expr(FILE* file, astree* node);
void emit_new_vreg(FILE* file, astree* node);

void indent(FILE* file){
    fprintf(file, "        ");
}

string new_vreg(char type){
    string vreg_string = "";
    switch(type){
        case 'i':
            vreg_string += type; 
            vreg_string += to_string(++int_count);
            break;
        case 'p':
            vreg_string += type; 
            vreg_string += to_string(++ptr_count);
            break;
        case 's':
            vreg_string += type; 
            vreg_string += to_string(++string_count);
            break;
        case 'a':
            vreg_string += type; 
            vreg_string += to_string(++ind_ptr_count);
            break;
        default:
        errprintf("Invalid Virtual Register type: %c \n", type);
    }
    return vreg_string;
}

void emit_while(FILE* file, astree* node){
    fprintf(file, "while_%zu_%zu_%zu:;\n", node->lloc.filenr, 
        node->lloc.linenr, node->lloc.offset);
    switch(node->children[0]->symbol){
        case TOK_IDENT:
        case TOK_CHARCON:
        case TOK_INTCON:
            break;
        default:
            indent(file);
            emit_expr(file, node->children[0]);
    }
    indent(file);
    fprintf(file,
        "if (!%s) goto break_%zu_%zu_%zu\n",
        node->children[0]->vreg.c_str(),
        node->children[1]->lloc.filenr,
        node->children[1]->lloc.linenr,
        node->children[1]->lloc.offset);
    emit_node(file, node->children[1]);
    indent(file);
    fprintf(file,
        "goto while_%zu_%zu_%zu\n",
        node->lloc.filenr,
        node->lloc.linenr,
        node->lloc.offset);
    fprintf(file,
        "break_%zu_%zu_%zu):;\n",
        node->children[1]->lloc.filenr,
        node->children[1]->lloc.linenr,
        node->children[1]->lloc.offset);
}

void emit_if(FILE* file, astree* node){
    switch(node->children[0]->symbol){
        case TOK_IDENT:
        case TOK_CHARCON:
        case TOK_INTCON:
            break;
        default:
            indent(file);
            emit_expr(file, node->children[0]);
    }
    indent(file);
    fprintf(file,
        "if (!%s) goto fi_%zu_%zu_%zu;\n",
        node->children[0]->vreg.c_str(),
        node->children[0]->lloc.filenr,
        node->children[0]->lloc.linenr,
        node->children[0]->lloc.offset);
    for(auto child : node->children[0]->children){
        indent(file);
        emit_node(file, child);
    }
    fprintf(file,
        "fi_%zu_%zu_%zu:;\n",
        node->children[0]->lloc.filenr,
        node->children[0]->lloc.linenr,
        node->children[0]->lloc.offset);

}

void emit_ifelse(FILE* file, astree* node){
    switch(node->children[0]->symbol){
        case TOK_IDENT:
        case TOK_CHARCON:
        case TOK_INTCON:
            break;
        default:
            indent(file);
            emit_expr(file, node->children[0]);
    }
    indent(file);

    fprintf(file,
        "if (!%s) goto else_%zu_%zu_%zu;\n",
        node->children[0]->vreg.c_str(),
        node->children[0]->lloc.filenr,
        node->children[0]->lloc.linenr,
        node->children[0]->lloc.offset);

    for(auto child : node->children[0]->children){
        indent(file);
        emit_node(file, child);
    }

    fprintf(file,
        "goto fi_%zu_%zu_%zu:;\n",
        node->children[0]->lloc.filenr,
        node->children[0]->lloc.linenr,
        node->children[0]->lloc.offset);
    fprintf(file,
        "else_%zu_%zu_%zu:;\n",
        node->children[0]->lloc.filenr,
        node->children[0]->lloc.linenr,
        node->children[0]->lloc.offset);
    for(auto child : node->children[1]->children){
        indent(file);
        emit_node(file, child);
    }
    fprintf(file,
        "fi_%zu_%zu_%zu:;\n",
        node->children[0]->lloc.filenr,
        node->children[0]->lloc.linenr,
        node->children[0]->lloc.offset);
}

void emit_binop(FILE* file, astree* node){
    string* lexinfo = const_cast<string*>(node->lexinfo);
    auto sym = symbol_stack[node->block_nr]->find(
       const_cast<string*>(node->lexinfo));
    bool not_found = false;
    if(!symbol_stack[node->block_nr]->count(lexinfo)){
        not_found = true;
    }

    string vreg;
    if(not_found){
        vreg = sym->second->vreg;
        fprintf(file, "binop %s %s %s",
            node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(),
            node->children[1]->vreg.c_str());
    }else{
        fprintf(file, "binop %s %s %s",
            node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(),
            node->children[1]->vreg.c_str());
    }
}


void emit_ident(FILE* file, astree* node){
    //printf("Emit ident: %s\n", node->lexinfo->c_str());
    string ident = mangle_ident(node);
    fprintf(file,"ident %s", ident.c_str());
    node->vreg = ident;
}

void emit_expr(FILE* file, astree* node){
    if(node->attributes[ATTR_vreg]){
        emit_binop(file, node);
    }else{
        switch(node->symbol){
            case TOK_IDENT:
                emit_ident(file, node);
                break;
            case TOK_INTCON:
            case TOK_CHARCON:
                emit_new_vreg(file, node);
                break;
        }
    }
    fprintf(file, "\n");
}

void emit_rec(FILE* file, astree* node){
    for(auto child : node->children){
        emit_rec(file, child);
    }
    emit_node(file, node);
}

void emit_return(FILE* file, astree* node){
    if(node->children.empty()){
        indent(file);
        fprintf(file, "return;\n");
    }else{
        fprintf(file, "return %s;\n", node->children[0]->vreg.c_str());
    }
}

void emit_assignment(FILE* file, astree* node){
    string* lexinfo = const_cast<string*>(node->lexinfo);
    //auto the_sym = symbol_stack[0]->find(
    //    const_cast<string*>(node->lexinfo));

    bool not_found = false;
    if(!symbol_stack[node->block_nr]->count(lexinfo)){
        not_found = true;
    }
    char* type_buf = nullptr;
    indent(file);
    if(node->attributes[ATTR_int]){
        node->vreg = new_vreg('i');
        type_buf = strdup("int");
    }else if(node->attributes[ATTR_struct]){
        node->vreg = new_vreg('p');
        type_buf = (char*)node->children[0]->lexinfo->c_str();
    }

    if(not_found){
        fprintf(file, "assg %s %s %s %s;\n",
            type_buf, node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(), node->vreg.c_str());
    }else{
        fprintf(file, "assg%s %s %s;\n",
            node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(),
            node->vreg.c_str());
    }
}

void emit_new_vreg(FILE* file, astree* node){
    char* type_buf;
    if(node->attributes[ATTR_int]){
        node->vreg = new_vreg('i');
        type_buf = strdup("int");
    }else if(node->attributes[ATTR_struct]){
        node->vreg = new_vreg('p');
        type_buf = (char*)node->children[0]->lexinfo->c_str();
    }else{
        node->vreg = new_vreg('a');
        type_buf = strdup("unknown");
    }
    fprintf(file, "vreg %s %s",
        type_buf, node->vreg.c_str());
    symbol* sym = new_symbol(node);
    symbol_entry syment = symbol_entry(
        const_cast<string*>(node->lexinfo), sym);
    symbol_stack[0]->insert(syment);
}

void emit_call(FILE* file, astree* node){
    if(!node->attributes[ATTR_void]){
        emit_new_vreg(file, node);
    }
    fprintf(file, "%s ( ", node->children.back()->lexinfo->c_str());
    for(auto child = node->children.begin(); 
        child != node->children.end()-1; child++){
        fprintf(file, "%s", (*child)->vreg.c_str());
        if(child != node->children.end()-2){
            fprintf(file, ", ");
        }else{
            fprintf(file, " );\n");
        }
    }
}

void emit_node(FILE* file, astree* node){
    switch(node->symbol){
        case TOK_WHILE:
            emit_while(file, node);
            break;
        case TOK_IF:
            emit_if(file, node);
            break;
        case TOK_IFELSE:
            emit_ifelse(file, node);
            break;
        case TOK_VARDECL:
            emit_assignment(file, node);
            break;
        case TOK_RETURN:
            emit_return(file, node);
            break;
        case TOK_CALL:
            emit_call(file, node);
            break;
        case '+':
        case '-':
        case '*':
        case '%':
        case TOK_EQ:
        case TOK_NE:
        case TOK_LE:
        case TOK_GE:
        case TOK_LT:
        case TOK_GT:
        case TOK_POS:
        case TOK_NEG:
        case TOK_INTCON:
        case TOK_IDENT:
        case TOK_CHARCON:
            emit_expr(file, node);
            break;
    }
}

void emit_stringcon(FILE* file, astree* node){
    string vreg = new_vreg('s');
    node->vreg = vreg;
    fprintf(file, "char* %s = %s;\n",
        vreg.c_str(), node->lexinfo->c_str());
    symbol* sym = new_symbol(node);
    symbol_entry syment = symbol_entry(
        const_cast<string*>(node->lexinfo), sym);
    symbol_stack[0]->insert(syment);
    // AND HERE AJ;SLKJTA;SDLKFJAS;DLFASJD;FLKASDJF;ASLDKJFAS;LDFKJ
    // a;lskdjf;aslkdfjas;ldkfjasl;dkfjasdo;fairjg;lakd
    // st_insert(symbol_stack[0], node);
}

string mangle_struct(astree* node){
    string struct_str;
    struct_str += "struct s_" + *node->children[0]->lexinfo + "{\n";
    for(auto field = node->children.cbegin()+1;
        field != node->children.cend(); field++){
        struct_str += "        ";
        struct_str += *(*field)->lexinfo;
        struct_str += " _f";
        struct_str += *(*field)->children[0]->lexinfo;
        struct_str += "_";
        struct_str += *(*field)->children[0]->lexinfo;
        struct_str += ";\n";
    }
    struct_str += "};\n";
    return struct_str;
}

string mangle_ident(astree* node){
    string ident_str = *node->vreg.c_str() + " ";
    astree* child = nullptr;
    if (!node->children.empty()){
        child = node->children[0];
    }else{
        child = node;
    }
    // Global Identifier
    if(node->block_nr == 0){
        //printf("global identifier mangle\n");
        // Two underscore for global.
        ident_str += "__";
        ident_str += *child->lexinfo;
    }

    // Local Identifier (block_nr > 0)
    else{
        ident_str += "_";
        ident_str += child->block_nr;
        ident_str += "_";
        ident_str += *child->lexinfo;
    }
    return ident_str;
}

string mangle_param(astree* node){
    string param_str = *node->lexinfo + " ";
    astree* child = node->children[0];
    param_str += "_";
    param_str += to_string(child->block_nr);
    param_str += "_";
    param_str += *child->lexinfo;
    return param_str;
}

void emit_func(FILE* file, astree* node){
    string func_str = "__" + *node->children[0]->lexinfo + " ";
    func_str += *node->children[0]->children[0]->lexinfo + "(\n";
    // For every parameter mangle it
    for(auto param : node->children[1]->children){
        func_str += "        " + mangle_param(param) + ",\n";
    }

    // Correct last comma from above forloop on last param w/ ')'
    func_str.pop_back();
    func_str.pop_back();
    func_str += ")\n{\n";

    fprintf(file, func_str.c_str());
    for(auto func_block : node->children[2]->children){
        emit_rec(file, func_block);
    }
    fprintf(file, "}\n");
}

void emit_oil(FILE* file, astree* root){
    // Prolog
    fprintf(file, "#define __OCLIB_C__\n");
    fprintf(file, "#include \"oclib.oh\"\n");
    // First process structs
    for(auto child : root->children){
        if(child->symbol == TOK_STRUCT){
            fprintf(file, mangle_struct(child).c_str());
        }
    }

    // ocmain Function Header
    fprintf(file, "void __ocmain (void) {\n");

    // STRINGCON
    for(auto node : stringcons){
        indent(file);
        emit_stringcon(file, node);
    }

    // Global Variables
    for(auto child : root->children){
        if(child->symbol == TOK_VARDECL){
            fprintf(file, "        ");
            child->vreg = mangle_ident(child->children[0]);
            fprintf(file, child->vreg.c_str());
        }else if(child->symbol != TOK_FUNCTION && 
            child->symbol != TOK_PROTOTYPE){
            emit_rec(file, root);
        }
    }
    fprintf(file, "}\n");
    // Function Emission
    for(auto child : root->children){
        if(child->symbol == TOK_FUNCTION){
            emit_func(file, child);
        }
    }

}






