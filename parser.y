%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define YYDEBUG 1
int errors;
%}

/*=========================================================================
								TOKENS
=========================================================================*/
%start program
%token CLASS
%token ID
%token CALLOUT TYPE GE LE SC LRP RRP LCP RCP LSP RSP COMMA LT GT ADD SUBTRACT MULTIPLY DIVIDE MOD NOT ANDAND OROR EQEQ NOTEQ EQ ADDEQ SUBTRACTEQ
%token INT BOOLEAN STRING CHARACTER

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
	;

var:
	ID
	| ID LSP INT RSP
	;

statement_decl:
	location assign_op expr SC
	| CALLOUT LRP STRING COMMA callout_args RRP SC
	;

expr:
	location
	| method_call
	| literal
	| expr ADD expr
	| expr SUBTRACT expr
	| expr MULTIPLY expr
	| expr DIVIDE expr
	| expr MOD expr
	| expr LT expr
	| expr GT expr
	| expr LE expr
	| expr GE expr
	| expr EQEQ expr
	| expr NOTEQ expr
	| expr OROR expr
	| expr ANDAND expr
	| '-' expr
	| NOT expr
	| LRP expr RRP
	;

method_call:
	ID LRP exprs RRP SC
	| CALLOUT LRP STRING COMMA callout_args RRP SC
	;

exprs:
	expr
	| exprs COMMA expr
	;

literal:
	INT
	| CHARACTER
	| BOOLEAN
	;

location:
	ID
	| ID LSP expr RSP
	;

callout_args:
	callout_arg
	| callout_args COMMA callout_arg
	;

callout_arg:
	expr
	| STRING
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
	/*yydebug = 1;*/
	errors = 0;
	yyparse ();
}
/*=========================================================================
							YYERROR
=========================================================================*/
yyerror(char *s) /* Called by yyparse on error */
{
	errors++;
	printf ("%s\n", s);
}
/**************************** End Grammar File ***************************/