#include <vector>
#include <stack>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/IR/IRBuilder.h>
//#include <llvm/ModuleProvider.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <stdlib.h>
#include <string>
using namespace std;
using namespace llvm;

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

class Visitor {
	public: 
		virtual void Visit(class ASTNode *) {}
		virtual void accept(class ASTNode *){}
};
class Traversal : public Visitor {
	public:
		void Visit(ASTNode *);
};
/****** CODE GEN CLASSES ********/
class CodeGenBlock {
public:
    BasicBlock *block;
    std::map<std::string, Value*> locals;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    Function *mainFunction;

public:
    Module *module;
    CodeGenContext() { module = new Module("main", getGlobalContext()); }
    
    void generateCode(class ASTNode *);
    //GenericValue runCode();
    std::map<std::string, Value*>& locals() { return blocks.top()->locals; }
    BasicBlock *currentBlock() { return blocks.top()->block; }
    void pushBlock(BasicBlock *block) { blocks.push(new CodeGenBlock()); blocks.top()->block = block; }
    void popBlock() { CodeGenBlock *top = blocks.top(); blocks.pop(); delete top; }
};
/********************************/
class ASTNode { 
	public:
		virtual void traverse() {}
		virtual void accept(Visitor *v) {}
		virtual Value *codegen(CodeGenContext&){}
};
class ASTStmtDecl : virtual public ASTNode {
	public:
		virtual void traverse(){}
		virtual void accept(Visitor *){}
		virtual Value* codegen(CodeGenContext &){}
};
class ASTVar : public ASTNode{
	
	private:
		string declType;  //NORMAL OR ARRAY
		string varName;   //Name of the Variable
		string dataType;   // DataType of the Variable
		unsigned int length; // Length of the array if variable is an array
		Value *codegen(CodeGenContext& );
	
	public:
		ASTVar(string, string, int length);
		ASTVar(string, string);
		void setDataType(string);
		void traverse();
		void accept(Visitor *);
		string getVarName() { return varName; }
};
class ASTVars : public ASTNode {
	private:
		vector <class ASTVar *> varList;
	public:
		void push_back(class ASTVar *);
		void push_back_vars(class ASTVars *);
		void traverse();
		vector <class ASTVar *> getVarList();
		void write2bison(FILE *);
		void accept(Visitor *);
};
class ASTFieldDecl : public ASTNode {
	
	private:
		string dataType;
		vector <class ASTVar *> varList;
	
	public:
		ASTFieldDecl(string, class ASTVars *);
		vector <class ASTVar *> getVarList();
		void traverse();
		void accept(Visitor *);
		Value* codegen(CodeGenContext&);
};

class ASTFieldDecls : public ASTNode {
	private:
		vector <class ASTFieldDecl *> declList;
		int count;
	public:
		ASTFieldDecls();
		void push_back(class ASTFieldDecl *);
		//void push_back_decls(class ASTFieldDecls *);
		void traverse();
		void accept(Visitor *);
		Value* codegen(CodeGenContext&);
};
class ASTExprs : public ASTNode {
	private:
		vector <class ASTExpr *> exprList;
		int count;
	public:
		ASTExprs();
		virtual string toString(){}
		void push_back(class ASTExpr *);
		void traverse();
		void accept(Visitor *);
};
class ASTExpr : virtual public ASTNode {
	protected:
		exprType etype;
	public:
		void setEtype(exprType x) { etype = x; }
		exprType getEtype() { return etype; }
		virtual void traverse(){}
		virtual string toString(){}
		virtual void accept(Visitor *){}
		virtual Value *codegen(CodeGenContext& ){}
};
class ASTParenExpr: public ASTExpr {
	private:
		class ASTExpr *expr;
	public:
		ASTParenExpr(class ASTExpr *);
		void traverse();
		void accept(Visitor *);
};
class ASTMethCall: public ASTStmtDecl, public ASTExpr {
	protected:
		string methName;
	public: 
		virtual void traverse(){}
		virtual void accept(Visitor *) {}
		virtual Value* codegen(CodeGenContext &){}
};
class ASTCallout: virtual public ASTMethCall{
	private:
		class ASTCalloutargs *args;
	public:
		ASTCallout(string, class ASTCalloutargs *);
		void traverse();	
		void accept(Visitor *);
		Value* codegen(CodeGenContext& );
};
class ASTInBuilt: public ASTMethCall {
	private:
		class ASTExprs *exprs;
	public:
		ASTInBuilt(string, class ASTExprs *exprs);
		void traverse();
		void accept(Visitor *);
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
		void accept(Visitor *);
		Value* codegen(CodeGenContext& );
};	
class ASTLiteral: public ASTExpr{
	protected:
		literalType ltype; 
	public:
		virtual void traverse(){}
		virtual int getValue() {}
		virtual string toString() {}
		void accept(Visitor *){}
};
class ASTIntLiteral: public ASTLiteral {
	private:
		int value;
	public:
		ASTIntLiteral(int);
		void traverse();
		int getValue();
		string toString();
		void accept(Visitor *);
		virtual Value *codegen(CodeGenContext& );
};
class ASTCharLiteral: public ASTLiteral{
	private:
		char value;
	public:
		ASTCharLiteral(string);
		void traverse();
		string toString();
		//int getValue();
		void accept(Visitor *);
};
class ASTBoolLiteral: public ASTLiteral {
	private:
		string value;
	public:
		ASTBoolLiteral(string);
		void traverse();
		string toString();
		//int getValue();
		void accept(Visitor *);
		Value* codegen(CodeGenContext&);
};
class ASTString: public ASTLiteral {
	private:
		string value;
	public:
		ASTString(string);
		//string toString();
		void traverse();
		void accept(Visitor *);
};
class ASTBinExpr: public ASTExpr{
	private:
		class ASTExpr *left;
		class ASTExpr *right;
		string opertor;
		Value *codegen(CodeGenContext& );
	public:
		ASTBinExpr(class ASTExpr *left, string opertor, class ASTExpr *right);
		void traverse();
		string toString();
		void accept(Visitor *);
};
class ASTUnExpr: public ASTExpr {
	private:
		string opertor;
		class ASTExpr *expr;
	public:
		ASTUnExpr(string opertor, class ASTExpr *);
		void traverse();
		// string toString();
		void accept(Visitor *);
};
class ASTStmtDecls : public ASTNode{
	private:
		vector <class ASTStmtDecl *> declList;
		int count;
	public: 
		ASTStmtDecls();
		void push_back(class ASTStmtDecl *);
		void traverse();
		void accept(Visitor *);
		Value* codegen(CodeGenContext&);
};
class ASTAssign: public ASTStmtDecl{
	private:
		class ASTLocation *location;
		class ASTExpr *expr;
	public:
		ASTAssign(class ASTLocation *, class ASTExpr *);
		void traverse();
		void accept(Visitor *);
		Value* codegen(CodeGenContext&);
};
class ASTCalloutarg : public ASTNode {
	private:
		class ASTExpr *expr;
	public:
		ASTCalloutarg(class ASTExpr *);
		ASTCalloutarg(string literal);
		void traverse();
		void accept(Visitor *);
		Value* codegen(CodeGenContext &context);
};
class ASTCalloutargs : public ASTNode {
	private:
		vector <class ASTCalloutarg *> argList;
		int count = 0;
	public:
		ASTCalloutargs();
		void push_back(class ASTCalloutarg *);
		void traverse();
		void accept(Visitor *);
		int getCount() { return count; }
		vector <class ASTCalloutarg *> getArgsList(){return argList;}
		Value* codegen(CodeGenContext &context);
};
class ASTProg : public ASTNode {
	private:
		class ASTFieldDecls *field_decls;
		class ASTStmtDecls *stmt_decls;
	public:
		ASTProg(class ASTFieldDecls *, class ASTStmtDecls *);
		void traverse();
		void accept(Visitor *);
		Value* codegen(CodeGenContext& );
};	
