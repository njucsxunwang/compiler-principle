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
typedef struct TreeNode{                //�﷨�����
	int type;							//����﷨�ķ�����
	int lineno;							//�к�
	char string_value[50];				//�ַ�������ֵ
	char inh_func[50];					//����Ǻ������βΣ�����������
	int int_value;						//����ֵ
	float float_value;					//������ֵ
	int children_number;				//�ӽ�����
	int inh_type;						//��������Ǻ������������ǽṹ��Ķ�����
	struct TreeNode* children[10];		//ָ����ӽڵ��ָ��
	Type* inh_kind;						//�������ݼ̳�����
	Type* syn_kind;						//���������ۺ�����
	arg_node* arg;						//����Ǻ���(���βΣ������������β�����
}TreeNode;
typedef enum bool{false,true}bool;


typedef struct{
	Type *type;
}var_symbol;							//��ͨ�����ṹ

typedef struct{
	Type* structure;
}struct_symbol;							//�ṹ������ṹ


 struct arg_node_{
	Type *type;
        int num;
	struct arg_node_ *next;
};										// �����β�����


typedef struct{
	arg_node *args;						//�����β�����
	Type* ret_type;						//��������
	int num;							//��������
	int is_defined;						//�������Ƕ���
	int lineno;							//�к�
}func_symbol;							//�����ṹ

struct func_list_{
	char id[32];						//������
	int num;						
	func_list* next;
};										//�����������ĺ�������ɵ����������ж��Ƿ�ֻ������δ���壩
typedef struct {
	char key[32];						//������
	int is_func;						//�Ƿ��Ǻ������β�
	char func_name[32];					//����������
	enum{Var,Struct,Func} type;			//������������
	union{
		var_symbol var_val;
		struct_symbol struct_val;
		func_symbol  func_val;
	}u;									//�����壨���ڲ�ͬ���ͱ�����ʹ�ò�ͬ���ͽṹ��
}symbol_node;							//���ű�洢����



struct Type_{
	enum{Int,Float,Array,Structure,Error}kind;//5�����ͣ�����Error���������Ѿ��������ʹ���󣬷�ֹ���������͵��¶δ������õ�
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
	char id[32];					//����
	int lineno;						//�к�
	Type* type;						//����
	FieldList* next;				
};									//�ṹ�����




typedef struct hash_node_{
	symbol_node *data;
	struct hash_node_ *prev,*next;
}hash_node;							//��ϣ��ĵ������

#define NR_SYMBOL_TABLE 0X4000
hash_node symbol_table[NR_SYMBOL_TABLE];	//�����ϣ��
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
