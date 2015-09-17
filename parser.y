/*=========================================================================
								BISON CODE
						ADHISH SINGLA 201403004
						AAYUSH MAINI  201301012
=========================================================================*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define YYDEBUG 1
int errors;
extern int yylineno;
extern char* yytext;
extern char linebuf[500];
FILE *flex_out;
FILE *bison_out;
%}

%union
{
	int number;
	char *string;
}

/*=========================================================================
								TOKENS
=========================================================================*/
%start program
%token CLASS
%token <string> ID TYPE GE LE SC LRP RRP LCP RCP LSP RSP COMMA LT GT ADD SUBTRACT MULTIPLY DIVIDE MOD NOT ANDAND OROR EQEQ NOTEQ EQ ADDEQ SUBTRACTEQ
%token CALLOUT
%token <number >INT
%token <string> BOOLEAN STRING CHARACTER

%left EQEQ NOTEQ
%left ANDAND OROR
%left LT GE LE GT
%left ADD SUBTRACT
%left MULTIPLY DIVIDE MOD
%left NOT
%left '-'

/*=========================================================================
					GRAMMAR RULES for Decaf language
=========================================================================*/
%%

program:
	CLASS ID LCP field_decls statement_decls RCP
	{
		fprintf(bison_out, "PROGRAM ENCOUNTERED\n");
	}
	;

field_decls:
	/* empty */
	| field_decls field_decl
	;

statement_decls:
	/* empty */
	| statement_decls statement_decl
	;

field_decl:
	TYPE var SC
	{
		fprintf(bison_out, "%s DECLARATION ENCOUNTERED. ID=%s\n", $<string>1, $<string>2);
	}
	| error SC
	{
		yyerrok;
	}
	;

var:
	ID
	{
		$<string>$ = $1;
	}
	| ID LSP INT RSP
	{
		sprintf($<string>$, "%s SIZE=%d", $1, $3);
	}
	;

statement_decl:
	location assign_op expr SC
	{
		fprintf(bison_out, "ASSIGNMENT OPERATION ENCOUNTERED\n");
	}
	| CALLOUT LRP STRING COMMA callout_args RRP SC
	{
		fprintf(bison_out, "CALLOUT TO %s ENCOUNTERED\n", $<string>3);
	}
	| error SC
	{
		yyerrok;
	}
	;

expr:
	location 					
	| method_call
	| literal
	| expr ADD expr
	{
		fprintf(bison_out, "ADDITION ENCOUNTERED\n");
	}				
	| expr SUBTRACT expr
	{
		fprintf(bison_out, "SUBTRACTION ENCOUNTERED\n");
	}
	| expr MULTIPLY expr{
		fprintf(bison_out, "MULTIPLICATION ENCOUNTERED\n");
	}
	| expr DIVIDE expr
	{
		fprintf(bison_out, "DIVISION ENCOUNTERED\n");
	}
	| expr MOD expr
	{
		fprintf(bison_out, "MOD ENCOUNTERED\n");
	}
	| expr LT expr
	{
		fprintf(bison_out, "LESS THAN ENCOUNTERED\n");
	}
	| expr GT expr
	{
		fprintf(bison_out, "GREATER THAN ENCOUNTERED\n");
	}
	| expr LE expr
	{
		fprintf(bison_out, "LESS THAN EQUAL TO ENCOUNTERED\n");
	}
	| expr GE expr
	{
		fprintf(bison_out, "GREATER THAN EQUAL TO ENCOUNTERED\n");
	}
	| expr EQEQ expr
	{
		fprintf(bison_out, "EQUAL EQUAL TO ENCOUNTERED\n");
	}
	| expr NOTEQ expr
	{
		fprintf(bison_out, "NOT EQUAL TO ENCOUNTERED\n");
	}
	| expr OROR expr
	{
		fprintf(bison_out, "CONDITIONAL OR ENCOUNTERED\n");
	}
	| expr ANDAND expr
	{
		fprintf(bison_out, "CONDITIONAL AND ENCOUNTERED\n");
	}
	| '-' expr
	{
		fprintf(bison_out, "UNARY MINUS ENCOUNTERED\n");
	}
	| NOT expr
	{
		fprintf(bison_out, "NOT ENCOUNTERED\n");
	}
	| LRP expr RRP 				
	;

method_call:
	ID LRP exprs RRP SC
	| CALLOUT LRP STRING COMMA callout_args RRP SC
	{
		fprintf(bison_out, "CALLOUT TO %s ENCOUNTERED\n", $<string>3);
	}
	;

exprs:
	expr
	| exprs COMMA expr
	;

literal:
	INT
	{
		fprintf(bison_out, "INT ENCOUNTERED=%d\n", $1);
	}
	| CHARACTER
	{
		fprintf(bison_out, "CHAR ENCOUNTERED=%s\n", $1);
	}
	| BOOLEAN
	{
		fprintf(bison_out, "BOOLEAN ENCOUNTERED=%s\n", $1);
	}
	;

location:
	ID
	{
		fprintf(bison_out, "LOCATION ENCOUNTERED=%s\n", $1);
	}
	| ID LSP expr RSP
	{
		fprintf(bison_out, "LOCATION ENCOUNTERED=%s\n", $1);
	}
	;

callout_args:
	callout_arg
	| callout_args COMMA callout_arg
	;

callout_arg:
	expr
	| STRING
	{
		$<string>$ = $1;
	}
	;

assign_op:
	EQ
	| ADDEQ
	| SUBTRACTEQ
	;

%%
/*=========================================================================
								MAIN
=========================================================================*/
main( int argc, char *argv[] )
{
	extern FILE *yyin;
	++argv; --argc;
	yyin = fopen( argv[0], "r" );
	flex_out = fopen("flex_output.txt", "w");
	bison_out = fopen("bison_output.txt", "w");
	/*yydebug = 1;*/
	errors = 0;
	yyparse ();
	if(errors == 0)
	{
		fprintf(stdout, "Success\n");
	}
	else
	{
		fprintf(stderr, "Total Errors: %d\n", errors);
		fprintf(stdout, "Syntax Error\n");
	}
}
/*=========================================================================
							YYERROR
=========================================================================*/
yyerror(char *s)
{
	errors++;
	fprintf(stderr, "Line %d: \x1b[31m %s \x1b[0m at %s:\n \x1b[31m %s \x1b[0m \n", yylineno, s, yytext, linebuf);
}

/**************************** End Grammar File ***************************/