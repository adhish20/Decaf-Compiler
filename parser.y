/*=========================================================================
								BISON CODE
						ADHISH SINGLA 201403004
						AAYUSH MAINI  201301012
=========================================================================*/
%{

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <string.h>
#include<iostream>
#include <vector>
#include "ASTDefinitions.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#define YYDEBUG 1
using namespace std;
using namespace llvm;

extern "C" int yylex();
extern "C" int yylineno;
extern "C" char* yytext;
extern "C" char linebuf[500];
extern union NODE yylval;

int errors;
FILE *flex_out;
FILE *bison_out;
FILE *IR_out;
void yyerror(char *);
class ASTNode *start = NULL;
class Visitor *visitor = new Traversal();

/********************************** AUXILLARY VARIABLES ***********************************/
class ASTIntLiteral *ONE = new ASTIntLiteral(1);
/*******************************************************************************************/

%}


/*=========================================================================
								TOKENS
=========================================================================*/
%start program

%type <program> program
%type <var> var
%type <vars> vars
%type <location> location
%type <field_decl> field_decl
%type <field_decls> field_decls
%type <expr> expr
%type <method_call> method_call
%type <literal> literal
%type <stmt_decl> statement_decl
%type <stmt_decls> statement_decls
%type <exprs> exprs
%type <callout_arg> callout_arg
%type <callout_args> callout_args

%token CLASS
%token <string> ID TYPE GE LE SC LRP RRP LCP RCP LSP RSP COMMA LT GT ADD SUBTRACT MULTIPLY DIVIDE MOD NOT ANDAND OROR EQEQ NOTEQ EQ ADDEQ SUBTRACTEQ
%token <string> CALLOUT
%token <number> INT
%token <string> CHARACTER BOOLEAN STRING

%left EQEQ NOTEQ
%left ANDAND OROR
%left LT GE LE GT
%left ADD SUBTRACT
%left MULTIPLY DIVIDE MOD
%left NOT
%left UMINUS

/*=========================================================================
					GRAMMAR RULES for Decaf language
=========================================================================*/
%%

program:
	CLASS ID LCP field_decls statement_decls RCP
	{
		fprintf(bison_out, "PROGRAM ENCOUNTERED\n");
		$$ = new ASTProg($4, $5);
		start = $$;
	}
	;

field_decls:
	/* empty */ 
	{
		$$ = new ASTFieldDecls();
	}
	| field_decls field_decl 
	{
		$$->push_back($2);
	}
	;

statement_decls:
	/* empty */ 
	{
		$$ = new ASTStmtDecls();
	}
	| statement_decls statement_decl
	{
		$$->push_back($2);
	}
	;

field_decl:
	TYPE vars SC
	{
		fprintf(bison_out, "%s DECLARATION ENCOUNTERED.", $<string>1);
		$$ = new ASTFieldDecl(string($1), $2);
	}
	| error SC
	{
		yyerrok;
	}
	;
vars:
	var
	{
		$$ = new ASTVars();
		$$->push_back($1);
		
	}
	| vars COMMA var
	{
		$$->push_back($3);
	}
	;

var:
	ID
	{
		$$ = new ASTVar(string("Normal"), string($1));
		//start = $$;
	}
	| ID LSP INT RSP
	{
		$$ = new ASTVar(string("Array"), string($1), $3);
		//start = $$;
	}
	;

statement_decl:
	location assign_op expr SC
	{
		fprintf(bison_out, "ASSIGNMENT OPERATION ENCOUNTERED\n");
		$$ = new ASTAssign($1, $3);
	}
	| CALLOUT LRP STRING COMMA callout_args RRP SC
	{
		fprintf(bison_out, "CALLOUT TO %s ENCOUNTERED\n", $<string>3);
		$$ = new ASTCallout(string($3), $5);
	}
	| error SC
	{
		yyerrok;
	}
	;

expr:
	location 
	{ 
		$$ = $1;
	}				
	| method_call 
	{ 
		$$ = $1; 
	}
	| literal 
	{ 
		$$ = $1;
	}
	| expr ADD expr 
	{
		fprintf(bison_out, "ADDITION ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("+"), $3); 
	}
	| expr SUBTRACT expr
	{
		fprintf(bison_out, "SUBTRACTION ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("-"), $3); 
	}
	| expr MULTIPLY expr{
		fprintf(bison_out, "MULTIPLICATION ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("*"), $3); 
	}
	| expr DIVIDE expr
	{
		fprintf(bison_out, "DIVISION ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("/"), $3); 
	}
	| expr MOD expr
	{
		fprintf(bison_out, "MOD ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("%"), $3); 
	}
	| expr LT expr
	{
		fprintf(bison_out, "LESS THAN ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("<"), $3); 
	}
	| expr GT expr
	{
		fprintf(bison_out, "GREATER THAN ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string(">"), $3); 
	}
	| expr LE expr
	{
		fprintf(bison_out, "LESS THAN EQUAL TO ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("<="), $3); 
	}
	| expr GE expr
	{
		fprintf(bison_out, "GREATER THAN EQUAL TO ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string(">="), $3); 
	}
	| expr EQEQ expr
	{
		fprintf(bison_out, "EQUAL EQUAL TO ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("=="), $3); 
	}
	| expr NOTEQ expr
	{
		fprintf(bison_out, "NOT EQUAL TO ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("!="), $3); 
	}
	| expr OROR expr
	{
		fprintf(bison_out, "CONDITIONAL OR ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("||"), $3); 
	}
	| expr ANDAND expr
	{
		fprintf(bison_out, "CONDITIONAL AND ENCOUNTERED\n");
		$$ = new ASTBinExpr($1, string("&&"), $3); 
	}
	| SUBTRACT expr %prec UMINUS
	{
		fprintf(bison_out, "UNARY MINUS ENCOUNTERED\n");
		$$ = new ASTUnExpr("-",  $2);
	}
	| NOT expr
	{
		fprintf(bison_out, "NOT ENCOUNTERED\n");
		$$ = new ASTUnExpr(string("!"), $2);
	}
	| LRP expr RRP 	
	{
		$$ = new ASTParenExpr($2);
	}			
	;

method_call:
	ID LRP exprs RRP SC
	{
		$$ = new ASTInBuilt(string($1), $3);
	}
	| CALLOUT LRP STRING COMMA callout_args RRP SC
	{
		fprintf(bison_out, "CALLOUT TO %s ENCOUNTERED\n", $<string>3);
		$$ = new ASTCallout(string($3), $5);
	}
	;

exprs:
	expr
	{
		$$ = new ASTExprs();
		$$->push_back($1);
	}
	| exprs COMMA expr
	{
		$$->push_back($3);
	}
	;

literal:
	INT
	{
		fprintf(bison_out, "INT ENCOUNTERED=%d\n", $1);
		$$ = new ASTIntLiteral($1);
		//start = $$;
	}
	| CHARACTER
	{
		fprintf(bison_out, "CHAR ENCOUNTERED=%s\n", $1);
		$$ = new ASTCharLiteral($1);
		//start = $$;
	}
	| BOOLEAN
	{
		fprintf(bison_out, "BOOLEAN ENCOUNTERED=%s\n", $1);
		$$ = new ASTBoolLiteral($1);
		//start = $$;
	}
	;

location:
	ID
	{
		fprintf(bison_out, "LOCATION ENCOUNTERED=%s\n", $1);
		$$ = new ASTLocation(string("Normal"), $1);
		//start = $$;
	}
	| ID LSP expr RSP
	{
		fprintf(bison_out, "LOCATION ENCOUNTERED=%s\n", $1);
		$$ = new ASTLocation(string("Array"), $1, $3);
		//start = $$;
	}
	;

callout_args:
	callout_arg 
	{
		$$ = new ASTCalloutargs();
		$$->push_back($1);
	}
	| callout_args COMMA callout_arg
	{
		$$->push_back($3);
	}
	;

callout_arg:
	expr 
	{
		$$ = new ASTCalloutarg($1);
	}
	| STRING 
	{
		$$ = new ASTCalloutarg($1);
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
	extern Module *TheModule;
	
	extern FILE *yyin;
	++argv; --argc;
	yyin = fopen( argv[0], "r" );
	flex_out = fopen("flex_output.txt", "w");
	bison_out = fopen("bison_output.txt", "w");
	//IR_out = fopen("IR.txt", "w");
	CodeGenContext *ccontext = new CodeGenContext();
	errors = 0;
	yyparse ();
	fprintf(stdout, "201403004\n");
	fprintf(stdout, "201301012\n");
	ccontext->generateCode(start);
	if(errors == 0)
	{
		fprintf(stdout, "Success\n");
	}
	else
	{
		fprintf(stderr, "Total Errors: %d\n", errors);
		fprintf(stdout, "Syntax Error\n");
	}
	if(start && visitor)
		visitor->Visit(start);
}
/*=========================================================================
							YYERROR
=========================================================================*/
void yyerror(char *s)
{
	errors++;
	fprintf(stderr, "Line %d: \x1b[31m %s \x1b[0m at %s:\n \x1b[31m %s \x1b[0m \n", yylineno, s, yytext, linebuf);
}

/**************************** End Grammar File ***************************/