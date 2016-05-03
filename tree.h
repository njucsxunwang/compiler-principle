#ifndef _TREE_H_
#define _TREE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int ERROR;
typedef struct Type_ Type;
typedef struct FieldList_ FieldList;
typedef struct arg_node_ arg_node;
typedef struct func_list_ func_list;
typedef struct TreeNode{
	int type;
	int lineno;
	char string_value[50];
	char inh_func[50];
	int int_value;
	float float_value;
	int children_number;
	int inh_type;
	struct TreeNode* children[10];
	Type* inh_kind;
	Type* syn_kind;
	arg_node* arg;
}TreeNode;
typedef enum bool{false,true}bool;


typedef struct{
	Type *type;
}var_symbol;

typedef struct{
	Type* structure;
}struct_symbol;


 struct arg_node_{
	Type *type;
        int num;
	struct arg_node_ *next;
};


typedef struct{
	arg_node *args;
	Type* ret_type;
	int num;
	int is_defined;
	int lineno;
}func_symbol;

struct func_list_{
	char id[32];
	int num;
	func_list* next;
};
typedef struct {
	char key[32];
	int is_func;
	char func_name[32];
	enum{Var,Struct,Func} type;
	union{
		var_symbol var_val;
		struct_symbol struct_val;
		func_symbol  func_val;
	}u;
}symbol_node;



struct Type_{
	enum{Int,Float,Array,Structure,Error}kind;
	union{
		int int_val;
		float float_val;
		struct {
			Type* elem;
			int size;
		}array;
		FieldList* structure;
	}u;
};

struct FieldList_{
	char id[32];
	int lineno;
	Type* type;
	FieldList* next;
};




typedef struct hash_node_{
	symbol_node *data;
	struct hash_node_ *prev,*next;
}hash_node;

#define NR_SYMBOL_TABLE 0X4000
hash_node symbol_table[NR_SYMBOL_TABLE];
extern unsigned int hash_pjw(char* name);
extern void insert_symbol(symbol_node *p_symbol);
extern symbol_node *get_symbol(char *name);
extern int cmp_args(arg_node* arg1,arg_node* arg2);
extern int cmp_types(Type* t1,Type* t2);
extern int cmp_types_struct(Type* t1,Type* t2);
extern void DFS(struct TreeNode* root);
extern void print_err(int num,int lineno,char* message);
func_list *head;
struct TreeNode *treeroot;
#endif
