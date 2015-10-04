#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
using namespace std;

enum exprType { binary = 1, unary = 2, location = 3, literal = 4, methodCall = 5, parenExpr = 6};
enum literalType { Int = 1, Char = 2, Bool = 3, String = 4};

/********************************** CLASS PROTOTYPES ***********************************/
// class ASTVar;
// class ASTFieldDecl;
// class ASTFieldDecls;
// class ASTVars;
// class ASTLocation;
// class ASTExpr *expr;
// class ASTLiteral;
// class ASTIntLiteral;
// class ASTCharLiteral;
// class ASTBoolLiteral;
// class ASTMethodCall;
// class ASTCallout;
/*******************************************************************************************/

union NODE
{
	int number;
	char *string;
	class ASTVar *var;
	class ASTFieldDecl *field_decl;
	class ASTFieldDecls *field_decls;
	class ASTVars *vars;
	class ASTLocation *location;
	class ASTExpr *expr;
	class ASTLiteral *literal;
	class ASTIntLiteral *intliteral;
	class ASTCharLiteral *charliteral;
	class ASTBoolLiteral *boolliteral;
	class ASTMethCall *method_call;
	class ASTCallout *callout;
	class ASTStmtDecl *stmt_decl;
	class ASTStmtDecls *stmt_decls;
	class ASTProg *program;
	class ASTExprs *exprs;
	class ASTCalloutarg *callout_arg;
	class ASTCalloutargs *callout_args;

	NODE() {
		number = 0;
		string = NULL;
		var = NULL;
		field_decl = NULL;
		field_decls = NULL;
		vars = NULL;
	};
	~NODE(){};
};
typedef union NODE YYSTYPE;
#define YYSTYPE_IS_DECLARED 1

class ASTStmtDecl {
	public:
		virtual void traverse(){}	
};
class ASTVar {
	
	private:
		string declType;  //NORMAL OR ARRAY
		string varName;   //Name of the Variable
		string dataType;   // DataType of the Variable
		unsigned int length; // Length of the array if variable is an array
	
	public:
		ASTVar(string, string, int length);
		ASTVar(string, string);
		void setDataType(string);
		void traverse();
};
class ASTVars {
	private:
		vector <class ASTVar *> varList;
	public:
		void push_back(class ASTVar *);
		void push_back_vars(class ASTVars *);
		void traverse();
		vector <class ASTVar *> getVarList();
		void write2bison(FILE *);
};
class ASTFieldDecl {
	
	private:
		string dataType;
		vector <class ASTVar *> varList;
	
	public:
		ASTFieldDecl(string, class ASTVars *);
		vector <class ASTVar *> getVarList();
		void traverse();

};

class ASTFieldDecls {
	private:
		vector <class ASTFieldDecl *> declList;
		int count;
	public:
		ASTFieldDecls();
		void push_back(class ASTFieldDecl *);
		//void push_back_decls(class ASTFieldDecls *);
		void traverse();
};
class ASTExprs {
	private:
		vector <class ASTExpr *> exprList;
		int count;
	public:
		ASTExprs();
		virtual string toString(){}
		void push_back(class ASTExpr *);
		void traverse();
};
class ASTExpr {
	protected:
		exprType etype;
	public:
		void setEtype(exprType x) { etype = x; }
		exprType getEtype() { return etype; }
		virtual void traverse(){}
		virtual string toString(){}
};
class ASTParenExpr: public ASTExpr {
	private:
		class ASTExpr *expr;
	public:
		ASTParenExpr(class ASTExpr *);
		void traverse();
};
class ASTMethCall: public ASTStmtDecl, public ASTExpr {
	protected:
		string methName;
	public: 
		virtual void traverse(){} 
};
class ASTCallout: public ASTMethCall{
	private:
		class ASTCalloutargs *args;
	public:
		ASTCallout(string, class ASTCalloutargs *);
		void traverse();	
};
class ASTInBuilt: public ASTMethCall {
	private:
		class ASTExprs *exprs;
	public:
		ASTInBuilt(string, class ASTExprs *exprs);
		void traverse();
};
class ASTLocation: public ASTExpr {	
	private:
		string varName;
		string loctype;
		class ASTExpr *expr;
	public:
		ASTLocation(string, string, class ASTExpr *expr);
		ASTLocation(string, string);
		void traverse();
		string toString();
};	
class ASTLiteral: public ASTExpr{
	protected:
		literalType ltype; 
	public:
		virtual void traverse(){}
		virtual int getValue() {}
		virtual string toString() {}
};
class ASTIntLiteral: public ASTLiteral {
	private:
		int value;
	public:
		ASTIntLiteral(int);
		void traverse();
		int getValue();
		string toString();

};
class ASTCharLiteral: public ASTLiteral{
	private:
		char value;
	public:
		ASTCharLiteral(string);
		void traverse();
		string toString();
		//int getValue();

};
class ASTBoolLiteral: public ASTLiteral {
	private:
		string value;
	public:
		ASTBoolLiteral(string);
		void traverse();
		string toString();
		//int getValue();

};
class ASTString: public ASTLiteral {
	private:
		string value;
	public:
		ASTString(string);
		//string toString();
		void traverse();
};
class ASTBinExpr: public ASTExpr{
	private:
		class ASTExpr *left;
		class ASTExpr *right;
		string opertor;
	public:
		ASTBinExpr(class ASTExpr *left, string opertor, class ASTExpr *right);
		void traverse();
		string toString();
};
class ASTUnExpr: public ASTExpr {
	private:
		string opertor;
		class ASTExpr *expr;
	public:
		ASTUnExpr(string opertor, class ASTExpr *);
		void traverse();
		// string toString();
};
class ASTStmtDecls {
	private:
		vector <class ASTStmtDecl *> declList;
		int count;
	public: 
		ASTStmtDecls();
		void push_back(class ASTStmtDecl *);
		void traverse();
};
class ASTAssign: public ASTStmtDecl{
	private:
		class ASTLocation *location;
		class ASTExpr *expr;
	public:
		ASTAssign(class ASTLocation *, class ASTExpr *);
		void traverse();
};
class ASTCalloutarg {
	private:
		class ASTExpr *expr;
	public:
		ASTCalloutarg(class ASTExpr *);
		ASTCalloutarg(string literal);
		void traverse();
};
class ASTCalloutargs {
	private:
		vector <class ASTCalloutarg *> argList;
		int count = 0;
	public:
		ASTCalloutargs();
		void push_back(class ASTCalloutarg *);
		void traverse();
};
class ASTProg {
	private:
		class ASTFieldDecls *field_decls;
		class ASTStmtDecls *stmt_decls;
	public:
		ASTProg(class ASTFieldDecls *, class ASTStmtDecls *);
		void traverse();
};	