%{
#include <stdarg.h>
#include "lex.yy.c"
#include "tree.h"
int tab_number=0;

//0-26 终结符 27-47非终结符
char name[48][20]={"INT","FLOAT","ID","TYPE", "RELOP", "SEMI", "COMMA", "ASSIGNOP", "PLUS", "MINUS", "STAR", "DIV", "AND", "OR", "DOT", "NOT", "LP", "RP", "LB", "RB", "LC","RC", "STRUCT", "RETURN", "IF", "ELSE", "WHILE","Program","ExtDefList","ExtDef","Specifier","ExtDecList","FunDec","CompSt","VarDec","StructSpecifier","OptTag","DefList","Tag","VarList","ParamDec","StmtList","Stmt","Def","DecList","Dec","Exp","Args"};

struct TreeNode* createNode(int children_num,...);
void printNode(struct TreeNode* root);
%}
%union {
struct TreeNode * node;	
}
%token INT FLOAT ID TYPE RELOP SEMI COMMA ASSIGNOP PLUS MINUS STAR DIV AND OR DOT NOT LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE error
%type <node> Program ExtDefList ExtDef Specifier ExtDecList FunDec CompSt VarDec StructSpecifier OptTag DefList Tag VarList ParamDec StmtList Stmt Def DecList Dec Exp Args 

%type <node> INT FLOAT ID TYPE RELOP SEMI COMMA ASSIGNOP PLUS MINUS STAR DIV AND OR DOT NOT LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE error
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  PLUS MINUS
%left  STAR DIV
%right negetive NOT
%left  DOT LP RP LB RB
%error-verbose
%%
Program : ExtDefList {$$=createNode(1,$1);$$->type=27;$$->lineno=@$.first_line;treeroot=$$;}
;
ExtDefList :  ExtDef ExtDefList{$$=createNode(2,$1,$2);$$->type=28;$$->lineno=@$.first_line;}
|{$$=createNode(0);$$->type=28;$$->lineno=@$.first_line;}
;
ExtDef : Specifier ExtDecList SEMI{$$=createNode(3,$1,$2,$3);$$->type=29;$$->lineno=@$.first_line;}
| Specifier SEMI{$$=createNode(2,$1,$2);$$->type=29;$$->lineno=@$.first_line;}
| Specifier FunDec CompSt{$$=createNode(3,$1,$2,$3);$$->type=29;$$->lineno=@$.first_line;}
| Specifier FunDec SEMI{$$=createNode(3,$1,$2,$3);$$->type=29;$$->lineno=@$.first_line;}
| Specifier error SEMI
;
ExtDecList : VarDec{$$=createNode(1,$1);$$->type=31;$$->lineno=@$.first_line;}
| VarDec COMMA ExtDecList{$$=createNode(3,$1,$2,$3);$$->type=31;$$->lineno=@$.first_line;}
;
Specifier : TYPE{$$=createNode(1,$1);$$->type=30;$$->lineno=@$.first_line;}
| StructSpecifier{$$=createNode(1,$1);$$->type=30;$$->lineno=@$.first_line;}
;
StructSpecifier : STRUCT OptTag LC DefList RC{$$=createNode(5,$1,$2,$3,$4,$5);$$->type=35;$$->lineno=@$.first_line;}
| STRUCT Tag{$$=createNode(2,$1,$2);$$->type=35;$$->lineno=@$.first_line;}
| STRUCT error RC 
;
OptTag : {$$=createNode(0);$$->type=36;$$->lineno=@$.first_line;}
| ID{$$=createNode(1,$1);$$->type=36;$$->lineno=@$.first_line;}
;
Tag : ID{$$=createNode(1,$1);$$->type=38;$$->lineno=@$.first_line;}
;
VarDec : ID{$$=createNode(1,$1);$$->type=34;$$->lineno=@$.first_line;}
| VarDec LB INT RB{$$=createNode(4,$1,$2,$3,$4);$$->type=34;$$->lineno=@$.first_line;}
| error RB
;
FunDec : ID LP VarList RP{$$=createNode(4,$1,$2,$3,$4);$$->type=32;$$->lineno=@$.first_line;}
| ID LP RP{$$=createNode(3,$1,$2,$3);$$->type=32;$$->lineno=@$.first_line;}
| error RP
;
VarList : ParamDec COMMA VarList {$$=createNode(3,$1,$2,$3);$$->type=39;$$->lineno=@$.first_line;}
| ParamDec{$$=createNode(1,$1);$$->type=39;$$->lineno=@$.first_line;}
;
ParamDec : Specifier VarDec{$$=createNode(2,$1,$2);$$->type=40;$$->lineno=@$.first_line;}
;
CompSt : LC DefList StmtList RC{$$=createNode(4,$1,$2,$3,$4);$$->type=33;$$->lineno=@$.first_line;}
| error RC
;
StmtList : {$$=createNode(0);$$->type=41;$$->lineno=@$.first_line;}
| Stmt StmtList{$$=createNode(2,$1,$2);$$->type=41;$$->lineno=@$.first_line;}
;
Stmt : Exp SEMI{$$=createNode(2,$1,$2);$$->type=42;$$->lineno=@$.first_line;}
| CompSt{$$=createNode(1,$1);$$->type=42;$$->lineno=@$.first_line;}
| RETURN Exp SEMI{$$=createNode(3,$1,$2,$3);$$->type=42;$$->lineno=@$.first_line;}
| IF LP Exp RP Stmt   %prec LOWER_THAN_ELSE{$$=createNode(5,$1,$2,$3,$4,$5);$$->type=42;$$->lineno=@$.first_line;}
| IF LP Exp RP Stmt ELSE Stmt{$$=createNode(7,$1,$2,$3,$4,$5,$6,$7);$$->type=42;$$->lineno=@$.first_line;}
| WHILE LP Exp RP Stmt{$$=createNode(5,$1,$2,$3,$4,$5);$$->type=42;$$->lineno=@$.first_line;}
| error SEMI 
| Exp error
;
DefList : {$$=createNode(0);$$->type=37;$$->lineno=@$.first_line;}
| Def DefList{$$=createNode(2,$1,$2);$$->type=37;$$->lineno=@$.first_line;}
;
Def : Specifier DecList SEMI{$$=createNode(3,$1,$2,$3);$$->type=43;$$->lineno=@$.first_line;}
| Specifier error  SEMI
;
DecList : Dec{$$=createNode(1,$1);$$->type=44;$$->lineno=@$.first_line;}
| Dec COMMA DecList{$$=createNode(3,$1,$2,$3);$$->type=44;$$->lineno=@$.first_line;}
;
Dec : VarDec{$$=createNode(1,$1);$$->type=45;$$->lineno=@$.first_line;}
| VarDec ASSIGNOP Exp{$$=createNode(3,$1,$2,$3);$$->type=45;$$->lineno=@$.first_line;}
;


Exp : Exp ASSIGNOP Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp AND Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp OR Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp RELOP Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp PLUS Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp MINUS Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp STAR Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp DIV Exp{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| LP Exp RP{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| MINUS Exp  %prec negetive{$$=createNode(2,$1,$2);$$->type=46;$$->lineno=@$.first_line;}
| NOT Exp{$$=createNode(2,$1,$2);$$->type=46;$$->lineno=@$.first_line;}
| ID LP Args RP{$$=createNode(4,$1,$2,$3,$4);$$->type=46;$$->lineno=@$.first_line;}
| ID LP RP{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| Exp LB Exp RB{$$=createNode(4,$1,$2,$3,$4);$$->type=46;$$->lineno=@$.first_line;}
| Exp DOT ID{$$=createNode(3,$1,$2,$3);$$->type=46;$$->lineno=@$.first_line;}
| ID  {$$=createNode(1,$1);$$->type=46;$$->lineno=@$.first_line;}
| INT  {$$=createNode(1,$1);$$->type=46;$$->lineno=@$.first_line;}
| FLOAT{$$=createNode(1,$1);$$->type=46;$$->lineno=@$.first_line;}
| error RP
| Exp LB error RB
;
Args : Exp COMMA Args{$$=createNode(3,$1,$2,$3);$$->type=47;$$->lineno=@$.first_line;}
| Exp{$$=createNode(1,$1);$$->type=47;$$->lineno=@$.first_line;}
;
%%
#include <stdlib.h>
int yyerror(char* message){
	ERROR=1;
	char str[20];
	sprintf(str,"%d",yylineno);
	fputs("Error type B at line ",stderr);
	fputs(str,stderr);
	fputs(": ",stderr);
	fputs(message,stderr);
	fputc('\n',stderr);
	return 0;
}

struct TreeNode* createNode(int children_num, ...)
{
	struct TreeNode* parent_node=(struct TreeNode* )malloc(sizeof(struct TreeNode));;
	va_list ap;
	parent_node->children_number=children_num;
	if(children_num==0);
	else
	{
		va_start(ap,children_num);
		int i;
		for( i=0;i<children_num;i++)
		{
			parent_node->children[i]=va_arg(ap,struct TreeNode *);	
		}
 		va_end(ap);
	}
	return parent_node;
}


void printNode(struct TreeNode* root)
{
	if(root->type >=27 && root->children_number==0)
	return;
	else
	{
		if(tab_number>0)
		{
			int i;
			for(i=1;i<=tab_number;i++)
				printf("  ");
		}
		int i=root->type;
		printf("%s",name[i]);
		if(root->type>=27) 
			printf(" (%d)",root->lineno);
		else if(root->type>=2&&root->type<=4)
			printf(": %s",root->string_value);
		else if(root->type==0)
			printf(": %d",root->int_value);
		else if(root->type==1)
			printf(": %f",root->float_value);
		printf("\n");
		tab_number++;
		if(root->children_number>0)
		{
			int i;
			for(i=0;i<root->children_number;i++)
				printNode(root->children[i]);
		}
		tab_number--;
	}
}
