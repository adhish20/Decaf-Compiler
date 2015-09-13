%option yylineno
%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "parser.tab.h"
	char linebuf[500];
	extern yylex();
%}

/*=========================================================================
							TOKEN Definitions
=========================================================================*/

delim		[ \t\n]
ws			{delim}+
alpha		[A-Za-z]
digit		[0-9]
id			{alpha}({alpha}|{digit})*
integer		{digit}+(\.{digit}+)?(E[+-]?{digit}+)?
character   \'{alpha}\'
str         (\\n|\\t|\\'|\\\\|\\\"|[^\\"'])
	
/*=========================================================================
	REGULAR EXPRESSIONS defining the tokens for the Simple language
=========================================================================*/

%%
\n.*		     { strncpy(linebuf, yytext+1, sizeof(linebuf)); yyless(1);}
"class"			 { printf("CLASS\n"); return CLASS;}
"{"				 { return LCP;  }
"}"				 { return RCP;  }
"("				 { return LRP;  }
")"				 { return RRP;  }
"["				 { return LSP;  }
"]"				 { return RSP;  }
";"				 { return SC;   }
"!"				 { return NOT;  }

"+"				 { return ADD;       }
"-"				 { return SUBTRACT;  }
"*"				 { return MULTIPLY;  }
"/"				 { return DIVIDE;    }
"%"				 { return MOD;       }
"="				 { return EQ;        }
"+="			 { return ADDEQ;     }
"-="			 { return SUBTRACTEQ;}

"int"          	 {printf("INT_DECLARATION\n"); return TYPE;   }
"boolean"	     {printf("BOOLEAN_DECLARATION\n"); return TYPE;   }
"callout"		 { printf("CALLOUT\n"); return CALLOUT;}

">"  			 {return GT;}
"<"  			 {return LT;}
">=" 			 {return GE;}
"<=" 			 {return LE;}

"&&" 			 {return ANDAND;}
"||" 			 {return OROR;  }

"==" 			 {return EQEQ; }
"!=" 			 {return NOTEQ;}

true|false 		 {printf("BOOLEAN: %s\n", yytext); return BOOLEAN;}

{delim} 		 {;                }
","				 {return COMMA;    }
{id}			 {printf("ID: %s\n", yytext); return ID;       }
{integer}		 {printf("INT: %s\n", yytext); return INT;  }
{character}		 {printf("CHARACTER: %s\n", yytext); return CHARACTER;}

\"{str}*\" {printf("STRING: %s\n", yytext); return STRING;}

%%

/************************** End Scanner File *****************************/