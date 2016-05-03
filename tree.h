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
typedef struct TreeNode{                //语法树结点
	int type;							//结点语法文法类型
	int lineno;							//行号
	char string_value[50];				//字符串属性值
	char inh_func[50];					//如果是函数的形参，所属函数名
	int int_value;						//整型值
	float float_value;					//浮点型值
	int children_number;				//子结点个数
	int inh_type;						//用来标记是函数的作用域还是结构体的定义域
	struct TreeNode* children[10];		//指向各子节点的指针
	Type* inh_kind;						//用来传递继承属性
	Type* syn_kind;						//用来传递综合属性
	arg_node* arg;						//如果是函数(或形参），用来传递形参链表
}TreeNode;
typedef enum bool{false,true}bool;


typedef struct{
	Type *type;
}var_symbol;							//普通变量结构

typedef struct{
	Type* structure;
}struct_symbol;							//结构体变量结构


 struct arg_node_{
	Type *type;
        int num;
	struct arg_node_ *next;
};										// 函数形参链表


typedef struct{
	arg_node *args;						//函数形参链表
	Type* ret_type;						//返回类型
	int num;							//参数个数
	int is_defined;						//声明还是定义
	int lineno;							//行号
}func_symbol;							//函数结构

struct func_list_{
	char id[32];						//函数名
	int num;						
	func_list* next;
};										//所有声明过的函数名组成的链表（用来判断是否只声明，未定义）
typedef struct {
	char key[32];						//变量名
	int is_func;						//是否是函数的形参
	char func_name[32];					//所属函数名
	enum{Var,Struct,Func} type;			//变量所属类型
	union{
		var_symbol var_val;
		struct_symbol struct_val;
		func_symbol  func_val;
	}u;									//联合体（对于不同类型变量，使用不同类型结构）
}symbol_node;							//符号表存储类型



struct Type_{
	enum{Int,Float,Array,Structure,Error}kind;//5种类型，其中Error类型是在已经检测出类型错误后，防止不传递类型导致段错误设置的
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
	char id[32];					//域名
	int lineno;						//行号
	Type* type;						//类型
	FieldList* next;				
};									//结构体的域




typedef struct hash_node_{
	symbol_node *data;
	struct hash_node_ *prev,*next;
}hash_node;							//哈希表的单个结点

#define NR_SYMBOL_TABLE 0X4000
hash_node symbol_table[NR_SYMBOL_TABLE];	//定义哈希表
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
