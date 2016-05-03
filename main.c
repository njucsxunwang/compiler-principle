#include <stdio.h>
#include "tree.h"
int main(int argc, char** argv)
{
	if  (argc <=1) return 1;
	FILE* f = fopen(argv[1],"r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	if(ERROR==0)
		DFS(treeroot);
	return 0;
}
