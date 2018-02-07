%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern FILE *yyin;
extern FILE *yyout;
void yyerror (char *s);
extern lineNum;
%}

%token  RW_PROGRAM 
%token  RW_VAR 
%token  RW_PROCEDURE 
%token  RW_FUNCTION
%token  RW_IF 
%token  RW_THEN 
%token	RW_ELSE
%token  RW_WHILE 
%token  RW_DO 
%token  RW_BEGIN 
%token  RW_END
%token  ID
%token  DATATYPE
%token	ASSIGN
%token	INT
%token	UNKNOWN_TOKEN
%left   OR
%left   AND
%left   NOT
%left   LT LE GT GE NE EQ
%left   PLUS MINUS
%left   TIMES MOD DIV 
%start program
%%

program: 
	RW_PROGRAM ID ';' block '.'
	;

block: 
	many_locals compound_statement
	;

many_locals:

	|local_definition many_locals
	;

local_definition:
	variable_definition    
	| procedure_definition
	| function_definition
	;

variable_definition: 
	RW_VAR def_some_variables ';' more_vars
	;

more_vars: 

	| def_some_variables ';' more_vars 
	; 

def_some_variables: 
	ID more_IDs ':' DATATYPE
	;

more_IDs:

	| ',' ID more_IDs
	;

procedure_definition:
	procedure_header block ';'
	;

procedure_header:
	RW_PROCEDURE ID formal_parameters ';'
	;

function_definition:
	function_header block ';'
	;

function_header:
	RW_FUNCTION ID formal_parameters ':' DATATYPE ';'
	; 

formal_parameters:

	|'(' def_some_variables more_parameters ')'
	;

more_parameters:

	| ';' def_some_variables more_parameters
	;

statement:
	
	| ID assignment_or_proc_func_call
	| if_statement
	| while_statement
	| compound_statement
	;

assignment_or_proc_func_call:
	ASSIGN expression
	| actual_parameters_or_not
	;		

actual_parameters_or_not:
	|'(' actual_parameters ')'
	;

if_statement:
	RW_IF expression RW_THEN statement else_statement
	;

else_statement:

	| RW_ELSE statement
	;

while_statement:
	RW_WHILE expression RW_DO statement
	;

actual_parameters:
	expression ',' actual_parameters
	| expression
	;

compound_statement:
	RW_BEGIN statement more_statements RW_END
	;

more_statements:

	| ';' statement more_statements
	;	

expression:
	expression binary_op expression
	| unary_op expression
	| '(' expression ')'
	| ID actual_parameters_or_not
	| INT
	;

binary_op:
	PLUS 
	| MINUS
	| TIMES
	| DIV
	| AND
	| OR
	| LE
	| LT
	| GE
	| GT
	| EQ
	| NE
	| MOD
	;

unary_op:
	NOT
	| PLUS
	| MINUS 
	;

%%

void yyerror(char *s){
	printf("parser: Wrong Buzen program \n");
	printf("parser: Error in line %d : %s \n", lineNum, s);
}

int main (int argc, char **argv){
	++argv; 
	--argc;
	int result;
	char* extension = 0;
	if (argc > 0 ){
		yyin = fopen(argv[0], "r");
		if(yyin == 0){
			printf("parser: %s: No such file \n", argv[0]);
			fclose(yyin);
			exit(1);
		}
		extension = strrchr(argv[0], '.');
		if( !extension || strcmp(extension,".buz") ){
			printf("parser: %s: File format not recognized \n", argv[0]);
			printf("parser: Target file should be .buz \n");
			fclose(yyin);
			exit(1);
		}
		yyout = fopen("output","w");
		result = yyparse();
		if(result == 0){
			printf("parser: Corrent Buzen program \n");
		}
		fclose(yyin);
		fclose(yyout);
		return 0;
	}else{
		printf("parser: No input file \n");
		printf("Usage: parser.exe targetfile.buz \n");
		exit(1);
	}
}
