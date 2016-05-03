%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntax.tab.h"
#include "tree.h"
	int ERROR=0;
	int yycolumn = 1;
	#define YY_USER_ACTION       yylloc.first_line = yylloc.last_line = yylineno; yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng -1 ; yycolumn += yyleng ;
%}

%option yylineno
delim [ \t]
ws  {delim}+
letter [a-zA-Z_]
int_digit 0|[1-9][0-9]*
float_digit {int_digit}\.[0-9]+
float_e_digit ([0-9]*)\.[0-9]*(e|E)([+-]?)[0-9]+
id {letter}({letter}|[0-9])*
int_8 0([0-7])+
int_16 0(x|X)((([0-9a-f])+)|(([0-9A-F])+))
zhushi \/\*([^*]|(\*[^"/"]))*\*\/ 
%%
{ws} {;}
\/\/   {char c=input();while (c!='\n') c=input();}
{zhushi} {;}
int    {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type = 3;
	yylval.node->lineno = yylineno;
	strcpy(yylval.node->string_value,yytext);
	return TYPE;}
float  {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=3;
	yylval.node->lineno=yylineno;
	strcpy(yylval.node->string_value,yytext);
	return TYPE;}
struct {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=22;
	yylval.node->lineno=yylineno;
	return STRUCT;}
return {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=23;
	yylval.node->lineno=yylineno;
	return RETURN;}
if   {  	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=24;
	yylval.node->lineno=yylineno;
	return IF;}
else {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=25;
	yylval.node->lineno=yylineno;
	return ELSE;}
while {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=26;
	yylval.node->lineno=yylineno;
	return WHILE;}
";" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=5;
	yylval.node->lineno=yylineno;
	return SEMI;}
"," {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=6;
	yylval.node->lineno=yylineno;
	return COMMA;}
">"|"<"|">="|"<="|"=="|"!=" {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=4;
	yylval.node->lineno=yylineno;
	strcpy(yylval.node->string_value,yytext);
	return RELOP;}
"+" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=8;
	yylval.node->lineno=yylineno;
	return PLUS;}
"=" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=7;
	yylval.node->lineno=yylineno;
	return ASSIGNOP;}
"-" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=9;
	yylval.node->lineno=yylineno;
	return  MINUS;}
"*" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=10;
	yylval.node->lineno=yylineno;
	return STAR;}
"/" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=11;
	yylval.node->lineno=yylineno;
	return DIV;}
"&&" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=12;
	yylval.node->lineno=yylineno;
	return AND;}
"||" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=13;
	yylval.node->lineno=yylineno;
	return OR;}
"." {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=14;
	yylval.node->lineno=yylineno;
	return DOT;}
"!" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=15;
	yylval.node->lineno=yylineno;
	return NOT;}
"(" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=16;
	yylval.node->lineno=yylineno;
	return LP;}
")" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=17;
	yylval.node->lineno=yylineno;
	return RP;}
"[" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=18;
	yylval.node->lineno=yylineno;
	return LB;}
"]" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=19;
	yylval.node->lineno=yylineno;
	return RB;}
"{" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=20;
	yylval.node->lineno=yylineno;
	return LC;}
"}" {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=21;
	yylval.node->lineno=yylineno;
	return RC;}
{id} {	yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=2;
	yylval.node->lineno=yylineno;
	strcpy(yylval.node->string_value,yytext);
	return ID;}
{int_digit} {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=0;
	yylval.node->lineno=yylineno;
	yylval.node->int_value=atoi(yytext);
	return INT;}
{float_digit} {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=1;
	yylval.node->lineno=yylineno;
	yylval.node->float_value=atof(yytext);
	return FLOAT;}
{int_8}	 {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=0;
	yylval.node->lineno=yylineno;
	yylval.node->int_value=strtol(yytext,NULL,8);
	return INT;}
{int_16}  {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=0;
	yylval.node->lineno=yylineno;
	yylval.node->int_value=strtol(yytext,NULL,16);
	return INT;}
{float_e_digit} {yylval.node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	yylval.node->type=1;
	yylval.node->lineno=yylineno;
	yylval.node->float_value=strtof(yytext,NULL);
	return FLOAT;}
\n  {yycolumn = 1;}
.   {ERROR=1;	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);}
%%




