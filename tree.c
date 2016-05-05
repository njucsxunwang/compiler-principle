#include "tree.h"
char name1[48][20]={"INT","FLOAT","ID","TYPE", "RELOP", "SEMI", "COMMA", "ASSIGNOP", "PLUS", "MINUS", "STAR", "DIV", "AND", "OR", "DOT", "NOT", "LP", "RP", "LB", "RB", "LC","RC", "STRUCT", "RETURN", "IF", "ELSE", "WHILE","Program","ExtDefList","ExtDef","Specifier","ExtDecList","FunDec","CompSt","VarDec","StructSpecifier","OptTag","DefList","Tag","VarList","ParamDec","StmtList","Stmt","Def","DecList","Dec","Exp","Args"};

struct TreeNode* createNode(int children_num,...);
void printNode(struct TreeNode* root);
//哈希函数
unsigned int hash_pjw(char* name){
	unsigned int val=0,i;
	for(;*name;++name){
		val = (val << 2)+*name;
		if(i=val&~0x3fff) val = (val ^(i >> 12)) & 0x3fff;
	}
	return val;
}
//查表操作，从表中获得对应id的变量或结构体或函数表项
symbol_node *get_symbol(char *name){
	int index=hash_pjw(name);
	hash_node *p=symbol_table[index].next;
	while(p){
		if(strcmp(name,p->data->key)==0)
			return p->data;
		else
			p=p->next;}
	return NULL;
}
//插入节点操作
void insert_symbol(symbol_node *p){
	int index=hash_pjw(p->key);
	hash_node *new_node=(hash_node *)malloc(sizeof(hash_node));
	new_node->data = p;
	new_node->next=symbol_table[index].next;
	new_node->prev=&(symbol_table[index]);
	if(symbol_table[index].next!=NULL){
		symbol_table[index].next->prev=new_node;
		symbol_table[index].next=new_node;
	}
	else{
		symbol_table[index].next=new_node;
	}
}
//函数形参比较函数，调用形参相同返回1，否则返回0
int cmp_args(arg_node* arg1,arg_node* arg2)
{
	if(arg1==NULL&&arg2==NULL)
	return 1;
	if(arg1==NULL)
	return  0;
	if(arg2==NULL)
	return  0;
	if(arg1->num!=arg2->num)
	return 0;
	else {
	while(arg1!=NULL)
	{
		if(cmp_types(arg1->type,arg2->type)==0)
		return 0;
		else
		{
			arg1=arg1->next;
			arg2=arg2->next;
		}
	}
	return 1;
}
}
//比较两种类型是否一致
int cmp_types(Type* t1,Type* t2)
{
	if(t1->kind != t2->kind)
	return 0;
	if(t1->kind==Int || t1->kind ==Float)
	return 1;
	if(t1->kind==Array)
	return cmp_types(t1->u.array.elem,t2->u.array.elem);
}
//比较结构体类型是否一致
int cmp_types_struct(Type* t1,Type* t2)
{
	
	FieldList* f1= t1->u.structure;
	FieldList* f2= t2->u.structure;
	while(f1!=NULL ||f2!=NULL)
	{
		
		if(f1==NULL &&f2!=NULL)
		{
			
			return 0;
			}
		if(f1!=NULL &&f2==NULL)
		{
			
			return 0;
			}
		if(cmp_types(f1->type,f2->type)==0)
		{
		
		return 0;
		}
		else 
		{
		
			f1=f1->next;
			f2=f2->next;
			
		}
	}
	return 1;
}
	
// 判断结构体内部域是否存在重名，如果重名，全部输出
void redefined_field(Type* type)
{
	FieldList* new_field=type->u.structure;
	if(new_field->next==NULL)
	return ;
	else{
		int count=2;
		new_field=new_field->next;
		while(new_field!=NULL)
		{ 
			FieldList* current_field=type->u.structure;
			int count1=1;
			while(current_field!=NULL)
			{
				if(strcmp(current_field->id,new_field->id)==0&&count1<count)
				{
					print_err(15,new_field->lineno,"Redefined field!");
					break;
				}
				else {
					count1++;
					current_field=current_field->next;
					if(count1>=count)
					break;
					
				}
			}
			new_field=new_field->next;
			count++;
		
		}
	}
}
//深度遍历语法树，进行类型检查和维护符号表
void DFS(struct TreeNode *root)
{
	
	if(root->type==27)
	{
	//当前节点是Program，构造一个包含所有进行过函数声明的函数名称的链表，在最后对其中每一个名称进行处理，判断函数是否定义过

		head=(func_list*)malloc(sizeof(func_list));
		head->next=NULL;
		DFS(root->children[0]);
		func_list *current=head->next;
		while(current!=NULL)
		{ 		
				symbol_node* func_node=get_symbol(current->id);
				if(func_node->type=Func&&func_node->u.func_val.is_defined==0)
					print_err(18,func_node->u.func_val.lineno,"Undefined function!");
				current=current->next;
		}
	}	
	else if(root->type==29)
	{
	// 当前节点是ExtDef
		if(root->children_number==3&&root->children[1]->type==31)
		{
		//ExtDef->Specifier ExtDecList SEMI，普通变量、数组、结构体定义
			DFS(root->children[0]);
			root->children[1]->inh_kind=root->children[0]->syn_kind;
			DFS(root->children[1]);
		}
		else if(root->children_number==3&&root->children[2]->type==33)//ExtDef->Specifier FunDec CompSt
		{
		//ExtDef->Specifier FunDec CompSt  函数定义
		    	DFS(root->children[0]);
		    	root->children[1]->inh_type=0;                  //用来标注是定义不是声明
			root->children[1]->inh_kind=root->children[0]->syn_kind;	
			DFS(root->children[1]);
			root->children[2]->inh_kind=root->children[0]->syn_kind;
			DFS(root->children[2]);
		}
		else if(root->children_number==3&&root->children[1]->type==32)
		{
		//ExtDef->Specifier FunDec SEMI 函数声明
			DFS(root->children[0]);
			root->children[1]->inh_type=1;			//用来标注函数声明
			root->children[1]->inh_kind=root->children[0]->syn_kind;	
			DFS(root->children[1]);
			DFS(root->children[2]);
		}
		else //ExtDef->Specifier SEMI 结构体定义
		DFS(root->children[0]);
	}
	else if(root->type==31) 
	{
	//ExtDecList-> VarDec | VarDec COMMA ExtDecList 在这一层实现插入符号表普通变量、数组、结构体定义的节点
		root->children[0]->inh_kind=root->inh_kind;
		if(root->children_number==3)
			root->children[2]->inh_kind=root->inh_kind;
		int i;
		root->children[0]->inh_type=0;
		for( i=0;i<root->children_number;i++)
			DFS(root->children[i]);
		
		if(root->inh_kind->kind==Int || root->inh_kind->kind==Float) //普通变量、数组节点构造
		{
		       	symbol_node *var_node=get_symbol(root->children[0]->string_value);
			if(var_node!=NULL)
				print_err(3,root->children[0]->lineno,"Redefined variable!");
			else
			{
			        var_node =(symbol_node *)malloc(sizeof(symbol_node));
			        strcpy(var_node->key,root->children[0]->string_value);
			        var_node->type=Var;
			        var_node->is_func=0;
			        var_node->u.var_val=*((var_symbol*)malloc(sizeof(var_symbol)));
			        var_node->u.var_val.type=root->children[0]->syn_kind;
			        insert_symbol(var_node);
			}
		}
		else if(root->inh_kind->kind==Structure)			//结构体节点构造
		{
			symbol_node *struct_node=get_symbol(root->children[0]->string_value);
			if(struct_node!=NULL)
			print_err(16,root->children[0]->lineno,"Duplicated name for struct!");
			else
			{
				struct_node=(symbol_node*)malloc(sizeof(symbol_node));
				strcpy(struct_node->key,root->children[0]->string_value);
				struct_node->type=Struct;
				struct_node->is_func=0;
				struct_node->u.struct_val=*((struct_symbol*)malloc(sizeof(struct_symbol)));
				struct_node->u.struct_val.structure=root->children[0]->syn_kind;
				insert_symbol(struct_node);
			}
		}
	}
	else if(root->type==33) 
	{
	// CompSt -> LC DefList StmtList RC 函数作用域
		DFS(root->children[0]);
		root->children[1]->inh_type=0;  //表示是函数作用域而不是结构体的成员
		DFS(root->children[1]);
		root->children[2]->inh_kind=root->inh_kind;
		DFS(root->children[2]);
		DFS(root->children[3]);
	}
	else if(root->type==41) 
	{
	// StmtList -> Stmt StmtList  语句
		if(root->children_number==2)
		{
			root->children[0]->inh_kind=root->inh_kind;
			DFS(root->children[0]);
			root->children[1]->inh_kind=root->inh_kind;
			DFS(root->children[1]);
		}
	}
	else if(root->type==42) // Stmt
	{	
	
		if(root->children_number==5)
		{
			if(root->children[0]->type==24)//Stmt ->IF LP Exp RP Stmt 
			{
				
				DFS(root->children[0]);
				DFS(root->children[1]);
				DFS(root->children[2]);
				DFS(root->children[3]);
				root->children[4]->inh_kind=root->inh_kind;
				DFS(root->children[4]);
			}
			else // Stmt -> WHILE LP Exp RP Stmt
			{ 
				DFS(root->children[0]);
				DFS(root->children[1]);
				
				DFS(root->children[2]);
				root->children[4]->inh_kind=root->inh_kind;
				DFS(root->children[4]);
			}	
		}
		else if(root->children_number==7) // Stmt ->IF LP Exp RP Stmt ELSE Stmt
		{
			
			DFS(root->children[2]);
			root->children[4]->inh_kind=root->inh_kind;
			DFS(root->children[4]);
			root->children[6]->inh_kind=root->inh_kind;
			DFS(root->children[6]);
		}
		else if(root->children_number==3) // Stmt -> RETURN Exp SEMI
		{
			DFS(root->children[1]);
			if((root->children[1]->syn_kind->kind!=root->inh_kind->kind)&&root->children[1]->syn_kind->kind!=Error)
			print_err(8,root->children[0]->lineno,"Type mismatched for return!");
			
			
		}
		else if(root->children_number==2)  // Stmt -> Exp SEMI
		{	
			DFS(root->children[0]);
		}
		else if(root->children_number==1) // Stmt -> Compst
		{
			root->children[0]->inh_kind=root->inh_kind;
			DFS(root->children[0]);
		}
	}
	else if(root->type==30) 
	{
	// Specifier 变量类型 
		DFS(root->children[0]);
		if(root->children[0]->type==3) 
		{
		// Specifier -> TYPE 普通类型
		    root->syn_kind=(Type *)malloc(sizeof(Type));
		    if(strcmp(root->children[0]->string_value,"int")==0)
		    root->syn_kind->kind=Int;
		    else root->syn_kind->kind=Float;
		}
		else
		// Specifier -> StructSpecifier 结构体类型
		root->syn_kind=root->children[0]->syn_kind; 
		    
	}
	else if(root->type==35) 
	{
	// StructSpecifier 结构体类型	
		if(root->children_number==5){
		// StructSpecifier-> struct opgtag lc deflist rc 结构体定义
			
			if(root->children[1]->children_number==1)//OptTag -> id 如果有名称，则插入符号表，否则，只作为类型返回
			{
				symbol_node *struct_node=get_symbol(root->children[1]->children[0]->string_value);
				if(struct_node!=NULL)
					print_err(16,root->children[0]->lineno,"Duplicated name!");
				else {
					root->children[3]->inh_type=1; //deflist 用于结构体域而不是函数作用域的标识
					DFS(root->children[3]);
					if(root->children[3]->syn_kind!=NULL)
						redefined_field(root->children[3]->syn_kind);
					struct_node=(symbol_node *)malloc(sizeof(symbol_node));
					strcpy(struct_node->key,root->children[1]->children[0]->string_value);
					struct_node->type=Struct;
					struct_node->u.struct_val=*((struct_symbol *)malloc(sizeof(struct_symbol)));
					if(root->children[3]->syn_kind==NULL)
					{
							struct_node->u.struct_val.structure=(Type*)malloc(sizeof(Type));
							struct_node->u.struct_val.structure->kind=Structure;
							struct_node->u.struct_val.structure->u.structure=NULL;
					}
					else	
						struct_node->u.struct_val.structure=root->children[3]->syn_kind;
					insert_symbol(struct_node);
					root->syn_kind=struct_node->u.struct_val.structure;
				}
			}
			
			else{   // OptTag -> none 只作为类型返回
				root->children[3]->inh_type=1;
				DFS(root->children[3]);
				redefined_field(root->children[3]->syn_kind);
				root->syn_kind=root->children[3]->syn_kind;
			}
		}
		else { // StructSpecifier -> STRUCT Tag 用已定义结构体的定义其他类型
			symbol_node *struct_node=get_symbol(root->children[1]->children[0]->string_value);
			if(struct_node==NULL)
			{
					print_err(17,root->children[0]->lineno,"Undefined structure!");
					root->syn_kind=(Type*)malloc(sizeof(Type));
					root->syn_kind->kind=Error; //出错返回Error类型，防止发生段错误
			}
			else
				root->syn_kind=struct_node->u.struct_val.structure;
		}
		
	}
	else if(root->type==37)  
	{
	// DefList 函数作用域或者结构体域
		if(root->children_number==2)  
		{
		// DefList -> Def DefList
			
			root->children[0]->inh_type=root->inh_type;
			root->children[1]->inh_type=root->inh_type;
			DFS(root->children[0]);
			DFS(root->children[1]);
			
			if(root->inh_type==1)//结构体域
			{
				
				FieldList* current_field=root->children[0]->syn_kind->u.structure;
				
				while(current_field->next!=NULL)
				{
					
					current_field=current_field->next;
				}
				
				if(root->children[1]->syn_kind!=NULL)
					current_field->next=root->children[1]->syn_kind->u.structure;
				root->syn_kind=root->children[0]->syn_kind;
				
			}
		}
		else if(root->children_number==0)
		{
			
			root->syn_kind==NULL;
		}
	}
		  				
	else if(root->type==38)  
	{
	// Tag -> id 已定义类型的结构体
		symbol_node *struct_node=get_symbol(root->children[0]->string_value);
		if(struct_node==NULL || (struct_node!=NULL &&struct_node->type!=Struct))
			print_err(1,root->children[0]->lineno,"Undefined variable!");
	}
	else if(root->type==34) 
	{
	// 当前结点为VarDec
		if(root->children_number==1){ // VarDec -> id 普通类型
			root->syn_kind=root->inh_kind;				
			strcpy(root->string_value,root->children[0]->string_value);
		}
		else if(root->children_number==4) // VarDec -> VarDec LB INT RB 数组类型
		{
		    root->children[0]->inh_kind=root->inh_kind;
		    DFS(root->children[0]);
		    strcpy(root->string_value,root->children[0]->string_value);
		    Type* new_kind=(Type*)malloc(sizeof(Type));
		    new_kind->kind=Array;//构造新的数组类型作为综合属性返回值传回上一层
		    new_kind->u.array.size=root->children[2]->int_value;
		    new_kind->u.array.elem=root->children[0]->syn_kind;
		    root->syn_kind=new_kind;
		}
	}	
	else if(root->type==32)  
	{
	// FunDec 函数实现	
		if(root->inh_type==0)//函数定义
		{	
			 
			symbol_node* func_node=get_symbol(root->children[0]->string_value);
			if(func_node!=NULL&&func_node->u.func_val.is_defined==1)//重定义
				print_err(4,root->children[0]->lineno,"Redefined function!");
			else if(func_node==NULL)//之前未定义也未声明
			{                                                        
				func_node=(symbol_node*)malloc(sizeof(symbol_node));
				func_node->type=Func;
				func_node->is_func=0;
				strcpy(func_node->key,root->children[0]->string_value);
				func_node->u.func_val=*((func_symbol*)malloc(sizeof(func_symbol)));
				if(root->children_number==4) // FunDec -> ID LP VarList RP
				{
					strcpy(root->children[2]->inh_func,root->children[0]->string_value);
					DFS(root->children[2]);
					func_node->u.func_val.args=root->children[2]->arg;
					func_node->u.func_val.ret_type=root->inh_kind;
					func_node->u.func_val.is_defined=1;
					func_node->u.func_val.num=root->children[2]->arg->num;
					func_node->u.func_val.lineno=root->children[0]->lineno;
					insert_symbol(func_node);
				}
				else{    // FunDec -> ID LP RP
					func_node->u.func_val.args=NULL;
					func_node->u.func_val.is_defined=1;
					func_node->u.func_val.num=0;
					func_node->u.func_val.ret_type=root->inh_kind;
					func_node->u.func_val.lineno=root->children[0]->lineno;
					insert_symbol(func_node);
				}	
			}
			else if(func_node!=NULL&&func_node->u.func_val.is_defined==0)//之前声明过
			{
				
				if(root->children_number==4) // FunDec -> ID LP VarList RP
				{
					strcpy(root->children[2]->inh_func,root->children[0]->string_value);
					DFS(root->children[2]);
					
					if(cmp_args(func_node->u.func_val.args,root->children[2]->arg)==1&&cmp_types(func_node->u.func_val.ret_type,root->inh_kind)==1)
					{
						func_node->u.func_val.lineno=root->children[0]->lineno;
						func_node->u.func_val.is_defined=1;
					}
					else 
						{print_err(19,root->children[0]->lineno,"Inconsistent function declaration!");
						  func_node->u.func_val.lineno=root->children[0]->lineno;
						func_node->u.func_val.is_defined=1;
						}	
				}
				else{    // FunDec -> ID LP RP
					if(func_node->u.func_val.args==NULL&&cmp_types(func_node->u.func_val.ret_type,root->inh_kind)==1)
					{
						func_node->u.func_val.lineno=root->children[0]->lineno;
						func_node->u.func_val.is_defined=1;
					}
					else 
						{print_err(19,root->children[0]->lineno,"Inconsistent function declaration!");
						func_node->u.func_val.lineno=root->children[0]->lineno;
						func_node->u.func_val.is_defined=1;
						}
				}
			}
		}
		else if(root->inh_type==1)//函数声明
		{
		 	
			symbol_node* func_node=get_symbol(root->children[0]->string_value);
			if(func_node==NULL)//之前未定义未声明
			{      
				                                                
				func_node=(symbol_node*)malloc(sizeof(symbol_node));
				func_node->type=Func;
				func_node->is_func=0;
				strcpy(func_node->key,root->children[0]->string_value);
				func_node->u.func_val=*((func_symbol*)malloc(sizeof(func_symbol)));
				func_list* current=head;
				while(current->next!=NULL)
				{
					current=current->next;
					
				}
				func_list* current1=(func_list*)malloc(sizeof(func_list));
				
				strcpy(current1->id,root->children[0]->string_value);
				
				current1->next=NULL;
				current->next=current1;
				if(root->children_number==4) // FunDec -> ID LP VarList RP
				{
					strcpy(root->children[2]->inh_func,root->children[0]->string_value);
					DFS(root->children[2]);
					func_node->u.func_val.args=root->children[2]->arg;
					func_node->u.func_val.ret_type=root->inh_kind;
					func_node->u.func_val.is_defined=0;
					func_node->u.func_val.num=root->children[2]->arg->num;
					func_node->u.func_val.lineno=root->children[0]->lineno;
					insert_symbol(func_node);
				}
				else{    // FunDec -> ID LP RP
					func_node->u.func_val.args=NULL;
					func_node->u.func_val.is_defined=0;
					func_node->u.func_val.num=0;
					func_node->u.func_val.ret_type=root->inh_kind;
					func_node->u.func_val.lineno=root->children[0]->lineno;
					insert_symbol(func_node);
				}	
			}
			else if(func_node!=NULL)//之前声明过或者定义过
			{
				
				if(root->children_number==4) // FunDec -> ID LP VarList RP
				{
					DFS(root->children[2]);
					if(cmp_args(func_node->u.func_val.args,root->children[2]->arg)==1&&cmp_types(func_node->u.func_val.ret_type,root->inh_kind)==1)
					;
					else 
						print_err(19,root->children[0]->lineno,"Inconsistent function declaration!");
						
				}
				else{    // FunDec -> ID LP RP
					if(func_node->u.func_val.args==NULL&&cmp_types(func_node->u.func_val.ret_type,root->inh_kind)==1)
					;
					else 
						print_err(19,root->children[0]->lineno,"Inconsistent function declaration!");
				}
			}
		}	
	}
	else if(root->type==39) 
	{
	//VarList 形参列表
		if(root->children_number==1) // VarList -> ParamDec 
		{
			strcpy(root->children[0]->inh_func,root->inh_func);
			DFS(root->children[0]);
			root->arg=root->children[0]->arg;
		}
		else{  // VarList -> ParamDec COMMA VarList
			strcpy(root->children[0]->inh_func,root->inh_func);
			DFS(root->children[0]);
			DFS(root->children[1]);
			strcpy(root->children[0]->inh_func,root->inh_func);
			DFS(root->children[2]);
			root->children[0]->arg->next=root->children[2]->arg;
			root->children[0]->arg->num=1+root->children[2]->arg->num;
			root->arg=root->children[0]->arg;
		}
	}
	else if(root->type==40)  
	{
	//ParamDec -> Specifier VarDec
		DFS(root->children[0]);
		root->children[1]->inh_kind=root->children[0]->syn_kind;
		DFS(root->children[1]);
		if(root->children[0]->syn_kind->kind==Int ||root->children[0]->syn_kind->kind==Float)//普通变量或数组类型
		{
			symbol_node *var_node=get_symbol(root->children[1]->string_value);
			if(var_node!=NULL&&(var_node->is_func==0||(var_node->is_func==1&&strcmp(var_node->func_name,root->inh_func)!=0)))
			{
			///之前有相同名称的变量但不是此函数的形参
					print_err(3,root->children[0]->lineno,"Redefined variable!");
					arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			        	new_arg->type=root->children[1]->syn_kind;
			               	new_arg->num=1;
			                new_arg->next=NULL;
			                root->arg=new_arg;
			}
			else if(var_node==NULL)
			{ 
			//之前没有此变量
					var_node =(symbol_node *)malloc(sizeof(symbol_node));
					strcpy(var_node->key,root->children[1]->string_value);
			                var_node->type=Var;
			                strcpy(var_node->func_name,root->inh_func);
			                var_node->is_func=1;
			                var_node->u.var_val=*((var_symbol*)malloc(sizeof(var_symbol)));
			                var_node->u.var_val.type=root->children[1]->syn_kind;
			                insert_symbol(var_node);
			                arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			                new_arg->type=root->children[1]->syn_kind;
			                new_arg->num=1;
			                new_arg->next=NULL;
			                root->arg=new_arg;
				
			}
			else if(var_node!=NULL&& var_node->is_func==1 && strcmp(var_node->func_name,root->inh_func)==0)
			{
			//此变量已经存在于符号表中
					arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			        	new_arg->type=root->children[1]->syn_kind;
			               	new_arg->num=1;
			                new_arg->next=NULL;
			                root->arg=new_arg;
			}
		}
		else if(root->children[0]->syn_kind->kind==Structure)//结构体类型形参
		{
			symbol_node *struct_node=get_symbol(root->children[1]->string_value);
			if(struct_node!=NULL&&(struct_node->is_func==0||(struct_node->is_func==1&&strcmp(struct_node->func_name,root->inh_func)!=0)))
			{
			///之前有相同名称的变量但不是此函数的形参
				print_err(16,root->children[0]->lineno,"Duplicated name!");
				arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			                new_arg->type=root->children[1]->syn_kind;
			                new_arg->num=1;
			                new_arg->next=NULL;
			                root->arg=new_arg;
			}
			else if(struct_node==NULL){
			//之前没有此变量
				struct_node =(symbol_node *)malloc(sizeof(symbol_node));
				strcpy(struct_node->key,root->children[1]->string_value);
			                struct_node->type=Struct;
			                struct_node->u.struct_val=*((struct_symbol*)malloc(sizeof(struct_symbol)));
			                struct_node->u.struct_val.structure=root->children[1]->syn_kind;
			                insert_symbol(struct_node);
			                arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			                new_arg->type=root->children[1]->syn_kind;
			                new_arg->num=1;
			                new_arg->next=NULL;
			                root->arg=new_arg;
			}
			else if(struct_node!=NULL&& struct_node->is_func==1 && strcmp(struct_node->func_name,root->inh_func)==0)
			{
			//此变量已经存在于符号表中
					arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			        	new_arg->type=root->children[1]->syn_kind;
			               	new_arg->num=1;
			                new_arg->next=NULL;
			                root->arg=new_arg;
			}
		}
		else if(root->children[0]->syn_kind->kind==Error)//设置Error类型，防止段错误
		{
			arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			           new_arg->type=root->children[1]->syn_kind;
			           new_arg->num=1;
			           new_arg->next=NULL;
			           root->arg=new_arg;
		}
	}
	else if(root->type==43) // Def -> Specifier DecList SEMI
	{
		DFS(root->children[0]);
		root->children[1]->inh_kind=root->children[0]->syn_kind;
		root->children[1]->inh_type=root->inh_type;
		DFS(root->children[1]);
		if(root->inh_type==1)//
			root->syn_kind=root->children[1]->syn_kind;
		
	}
	else if(root->type==44)// DecList-> Dec |Dec COMMA DecList
	{
		int i;
		for(i=0;i<root->children_number;i++)
		{
			root->children[i]->inh_kind=root->inh_kind;
			root->children[i]->inh_type=root->inh_type;
			DFS(root->children[i]);
			
		}
		if(root->inh_type==0)  //函数作用域
		{
			if(root->inh_kind->kind==Int || root->inh_kind->kind==Float)
			{
				symbol_node *var_node=get_symbol(root->children[0]->string_value);
				if(var_node!=NULL)
					print_err(3,root->children[0]->lineno,"Redefined variable!");
				else{
			 		var_node =(symbol_node *)malloc(sizeof(symbol_node));
			 		strcpy(var_node->key,root->children[0]->string_value);
			 
			                var_node->type=Var;
			                var_node->u.var_val=*((var_symbol*)malloc(sizeof(var_symbol)));
			                var_node->u.var_val.type=root->children[0]->syn_kind;
			                insert_symbol(var_node);
			 
				}
			}
			else if(root->inh_kind->kind==Structure)
			{
				
				symbol_node *struct_node=get_symbol(root->children[0]->string_value);
				if(struct_node!=NULL)
					print_err(16,root->children[0]->lineno,"Duplicated name for struct!");
				else{
			 		struct_node =(symbol_node *)malloc(sizeof(symbol_node));
			 		strcpy(struct_node->key,root->children[0]->string_value);
			                struct_node->type=Struct;
			                struct_node->u.struct_val=*((struct_symbol*)malloc(sizeof(struct_symbol)));
			                struct_node->u.struct_val.structure=root->children[0]->syn_kind;
			                insert_symbol(struct_node);
			 
				}
			}
		}
		else if(root->inh_type==1)//结构体域
		{
			Type* new_type=(Type*)malloc(sizeof(Type));
			new_type->kind=Structure;
			FieldList* new_field=(FieldList *)malloc(sizeof(FieldList));
			strcpy(new_field->id,root->children[0]->string_value);
			new_field->lineno=root->children[0]->lineno;
			new_field->type=root->children[0]->syn_kind;
			if(root->children_number==1)
				new_field->next=NULL;
			else 
				new_field->next=root->children[2]->syn_kind->u.structure;
			new_type->u.structure=new_field;
			root->syn_kind=new_type;
		}
	}
	else if(root->type==45) // Dec
	{
	
		root->children[0]->inh_type=0;
		int i;
		for(i=0;i<root->children_number;i++)
		{
			root->children[i]->inh_kind=root->inh_kind;
			DFS(root->children[i]);
		}
		if(root->children_number==3) // Dec -> VarDec ASSIGNOP Exp
		{
			if(root->inh_type==1)
			{
				print_err(15,root->children[0]->lineno,"Redefined field!");
			}
			else{
				if(root->children[0]->syn_kind->kind!=root->children[2]->syn_kind->kind)
					print_err(5,root->children[0]->lineno,"Type mismatched for assignment!");
			}
		}
		
		root->syn_kind=root->children[0]->syn_kind;
		strcpy(root->string_value,root->children[0]->string_value);
		
		
	}
	else if(root->type==46) // Exp 表达式
	{
		
		if(root->children[0]->type==0) {
					   //exp->int
					Type *new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Int;
					new_type->u.int_val=root->children[0]->int_value;
					root->syn_kind=new_type;
					;}
		if(root->children[0]->type==1)//exp->float
		 {
					    //exp->int
					Type *new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Float;
					new_type->u.int_val=root->children[0]->float_value;
					root->syn_kind=new_type;
		}
		
	if(root->children_number==3 && root->children[1]->type==7) //exp->exp assignop exp 
		{
			
			DFS(root->children[0]);
			
			DFS(root->children[2]);
			struct TreeNode *current=root->children[0];
				if((current->children_number==1&&current->children[0]->type==2)||(current->children_number==3 &&current->children[0]->type==46&&current->children[1]->type==14&&current->children[2]->type==2)||(current->children_number==4&&current->children[1]->type==18))
				{
					if(root->children[0]->syn_kind->kind!=root->children[2]->syn_kind->kind&&root->children[0]->syn_kind->kind!=Error&&root->children[2]->syn_kind->kind!=Error)
					{
						print_err(5,root->children[0]->lineno,"Type mismatched for assignment!");
					}
					else if(root->children[0]->syn_kind->kind==Structure && cmp_types_struct(root->children[0]->syn_kind,root->children[2]->syn_kind)==0&&root->children[0]->syn_kind->kind!=Error&&root->children[2]->syn_kind->kind!=Error)
					{
						print_err(5,root->children[0]->lineno,"Type mismatched for assignment!");
			
					}
					else if(root->children[0]->syn_kind->kind!=Structure && cmp_types(root->children[0]->syn_kind,root->children[2]->syn_kind)==0&&root->children[0]->syn_kind->kind!=Error&&root->children[2]->syn_kind->kind!=Error)
					{
						print_err(5,root->children[0]->lineno,"Type mismatched for assignment!");
					}
				}
				else
					print_err(6,root->children[0]->children[0]->lineno,"The left-hand side of an assignment must be a variable!");
			
			
			
		}


		else if(root->children_number==3&&root->children[0]->type==46 &&root->children[2]->type==46)//exp->exp xxx exp
		{

			
			if(root->children[1]->type==12 ||root->children[1]->type==13)//exp and/or
			{
			
				DFS(root->children[0]);
				DFS(root->children[1]);
				DFS(root->children[2]);
				Type *new_type=(Type*)malloc(sizeof(Type));
				new_type->kind=Int;
				root->syn_kind=new_type;
			}
			else if(root->children[1]->type==4)
			{
				DFS(root->children[0]);
				DFS(root->children[1]);
				DFS(root->children[2]);
				if(root->children[0]->syn_kind->kind==Error||root->children[2]->syn_kind->kind==Error)
				{
				
				Type *new_type=(Type*)malloc(sizeof(Type));
				new_type->kind=Error;
				root->syn_kind=new_type;
				}
				else if(root->children[0]->syn_kind->kind!=root->children[2]->syn_kind->kind){
					print_err(7,root->children[1]->lineno,"Type mismatched for operands!");
					Type *new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				else if(root->children[0]->syn_kind->kind!=Int && root->children[0]->syn_kind->kind!=Float)
				{
					print_err(7,root->children[1]->lineno,"Type mismatched for operands!");
					Type *new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				else {
				Type *new_type=(Type*)malloc(sizeof(Type));
				new_type->kind=Int;
				root->syn_kind=new_type;
				};
			}
			else if(root->children[1]->type==9||root->children[1]->type==10||root->children[1]->type==11||root->children[1]->type==8)    //exp plus/minus/star/div exp
			{

				DFS(root->children[0]);
				DFS(root->children[1]);
				DFS(root->children[2]);
				
				
				 if(root->children[0]->syn_kind->kind==Error||root->children[2]->syn_kind->kind==Error)
				{
				
				Type *new_type=(Type*)malloc(sizeof(Type));
				new_type->kind=Error;
				root->syn_kind=new_type;
				}
				else if(root->children[0]->syn_kind->kind!=root->children[2]->syn_kind->kind){
					print_err(7,root->children[1]->lineno,"Type mismatched for operands!");
					Type *new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				else if(root->children[0]->syn_kind->kind!=Int && root->children[0]->syn_kind->kind!=Float)
				{
					print_err(7,root->children[1]->lineno,"Type mismatched for operands!");
					Type *new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				else {
				root->syn_kind=root->children[0]->syn_kind;
				};
				
			}
			
		}

		 if(root->children[0]->type==16||root->children[0]->type==9||root->children[0]->type==15) //exp->LP exp rp /minus exp/not exp
		{

			Type *new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Int;
					root->syn_kind=new_type;
			int i;
			for(i=0;i<root->children_number;i++)
				DFS(root->children[i]);
		}
		
		if(root->children_number==1&&root->children[0]->type==2)//exp->id
		{
				
				
			symbol_node *node=get_symbol(root->children[0]->string_value);
				if(node==NULL)
				{
					print_err(1,root->children[0]->lineno,"Undefined variable!");			
					Type* new_type=(Type *)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				else {
					if(node->type==Var)
					root->syn_kind=node->u.var_val.type;
					else if(node->type==Struct)
					root->syn_kind=node->u.struct_val.structure;
				}


		}

		if((root->children_number==3||root->children_number==4)&&root->children[0]->type==2&&root->children[1]->type==16){//exp->id Lp args rp/id lp rp
			
			symbol_node *func_node=get_symbol(root->children[0]->string_value);
			if(func_node==NULL)
				{
					print_err(2,root->children[0]->lineno,"Undefined function!");
					Type* new_type=(Type *)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
					
				
			else {
				if(func_node->type!=Func)
					{
						print_err(11,root->children[0]->lineno,"This is not a function!");
						Type* new_type=(Type *)malloc(sizeof(Type));
						new_type->kind=Error;
						root->syn_kind=new_type;
					}
				else{
					root->syn_kind=func_node->u.func_val.ret_type;
					
					
					if(root->children_number==4){
						DFS(root->children[2]);
						
						if(root->children[2]->syn_kind!=NULL && root->children[2]->syn_kind->kind==Error)
							;
						else if(cmp_args(func_node->u.func_val.args,root->children[2]->arg)==0)
							print_err(9,root->children[0]->lineno,"Function is not applicable for arguments!");
							
							
					}
					else {
						
						if(func_node->u.func_val.num!=0)
							print_err(9,root->children[0]->lineno,"Function is not applicable for arguments!");
						
							
				}
				}
		}
				
		}

		if(root->children_number>=2 &&root->children[1]->type==18)//exp->exp lb exp rb
		{
				
				DFS(root->children[0]);
				if(root->children[0]->syn_kind->kind!=Array)
				{
					print_err(10,root->children[0]->lineno,"This is Not an array!");
					Type* new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				DFS(root->children[2]);
				if(root->children[2]->syn_kind->kind!=Int)
				{
					print_err(12,root->children[0]->lineno,"This is not an integer!");
					Type* new_type=(Type*)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				if(root->children[0]->syn_kind->kind==Array&&root->children[2]->syn_kind->kind==Int)
				root->syn_kind=root->children[0]->syn_kind->u.array.elem;
		}
		
		if(root->children_number>=2 &&root->children[1]->type==14)   //exp->exp dot id
		{
				DFS(root->children[0]);
				DFS(root->children[1]);
				DFS(root->children[2]);
				if(root->children[0]->syn_kind->kind!=Structure)
				{
					print_err(13,root->children[0]->lineno,"Illegal use of dot!");
					Type* new_type=(Type *)malloc(sizeof(Type));
					new_type->kind=Error;
					root->syn_kind=new_type;
				}
				else {
					FieldList* current_field=root->children[0]->syn_kind->u.structure;
					int count=0;
					while(current_field!=NULL)
					{
						if(strcmp(current_field->id,root->children[2]->string_value)==0)
						{
							root->syn_kind=current_field->type;
							count=1;
							
						}
						current_field=current_field->next;
					}
					if(count==0)
					{
						print_err(14,root->children[2]->lineno,"Non-existent field!");
						Type* new_type=(Type *)malloc(sizeof(Type));
						new_type->kind=Error;
						root->syn_kind=new_type;
					}
				}
		}
		
	}
	else if(root->type==47) // Args 调用函数的参数，形成一个args链表
	{
		if(root->children_number==1) // Args -> Exp 
		{
			
			DFS(root->children[0]);
			
			if(root->children[0]->syn_kind->kind==Error)
			root->syn_kind=root->children[0]->syn_kind;
			else root->syn_kind=NULL;
			arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			new_arg->type=root->children[0]->syn_kind;
			new_arg->num=1;
			new_arg->next=NULL;
			root->arg=new_arg;
			
		}
		else  // Args -> Exp COMMA Args
		{
			
			DFS(root->children[0]);
			DFS(root->children[2]);
			if((root->children[0]->syn_kind!=NULL &&root->children[0]->syn_kind->kind==Error) ||  (root->children[2]->syn_kind!=NULL&&root->children[2]->syn_kind->kind==Error))
			{ 
				Type * new_type=(Type*)malloc(sizeof(Type));
				new_type->kind=Error;
				root->syn_kind=new_type;
			}
			else root->syn_kind=NULL;
			arg_node* new_arg=(arg_node*)malloc(sizeof(arg_node));
			new_arg->type=root->children[0]->syn_kind;
			
			new_arg->next=root->children[2]->arg;
			new_arg->num=1+root->children[2]->arg->num;
			root->arg=new_arg;
			
		}
	}
	else {  //others
		if(root->children_number==0) return ;
		else {
			int i;
			for (i=0;i<root->children_number;i++)
			DFS(root->children[i]);
	}
	}
}




void print_err(int num,int lineno,char* message)
{
	printf("Error Type %d at Line %d: %s\n",num,lineno,message);
}
			
			
