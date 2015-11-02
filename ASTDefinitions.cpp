#include "ASTDefinitions.h"
#include <string>
#include <vector>
#include <iostream>
#include<fstream>
#define TAB giveTabs()

using namespace std;
using namespace llvm;
int tab_count = 0;
int tab_width = 4;
ofstream out("XML_visitor.txt");

Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value*> NamedValues;
Value *mainF;

void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(*(str+start), *(str+end));
        start++;
        end--;
    }
}
char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; 
    reverse(str, i);
    return str;
}

void giveTabs() {
	for(int i = 0; i<tab_count; i++) {
		for(int j = 0; j<tab_width; j++) {
			out << " ";
		}
	}
}
/*************************** CODE GENERATOR *************************************/
void CodeGenContext::generateCode(class ASTNode *start)
{
	
	FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
	mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
	
	mainF = mainFunction;	
	/* Push a new variable/block context */
	pushBlock(bblock);
	
	start->codegen(*this)->dump();
	ReturnInst::Create(getGlobalContext(), bblock);
	
	popBlock();
}
Type *typeOf() 
{
		return Type::getInt64Ty(getGlobalContext());
}
Value* ASTFieldDecls::codegen(CodeGenContext& context) {
	for(int i = 0; i < declList.size(); i++) {
		declList[i]->codegen(context);
	}
}
Value* ASTFieldDecl::codegen(CodeGenContext& context) {

	string varName;
	for(int i = 0; i<varList.size(); i++) {

		varName = varList[i]->getVarName();
		if( context.locals().find(varName) == context.locals().end() ) {
			AllocaInst *alloc = new AllocaInst(typeOf(), varName.c_str(), context.currentBlock());
			context.locals()[varName.c_str()] = alloc;
			alloc->setAlignment(8);
			//alloc->dump();
			return alloc;
		}
		else {
			std::cerr << "redeclared variable " << varName.c_str() << endl;

		}
	}
	return NULL;
}
Value* ASTProg::codegen(CodeGenContext &context) {
	
	/**** MAIN ******/
	// Function *function = context.module->getFunction("main");
	// ArrayRef<Value *>args;
	// CallInst *call = CallInst::Create(function, args, "", context.currentBlock());
	// call->dump();
	/****************/

	// FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
	// context.mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", context.module);
	// BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
	// context.mainFunction->dump();
	field_decls->codegen(context);
	stmt_decls->codegen(context);
	return mainF;
}
Value *ASTIntLiteral::codegen(CodeGenContext &context) {
  Value *V = ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
  //V->dump();
  return V;
}
Value *ASTBoolLiteral::codegen(CodeGenContext &context) {
  Value *V = ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value == "true", true);
  //V->dump();
  return V;
}
Value *ASTVar::codegen(CodeGenContext &context){
	// cout << "Var Stop\n";
	printf("Here due to %s\n", varName.c_str());	
	Value *v = new LoadInst(context.locals()[varName.c_str()], "", false, context.currentBlock());
	//v->dump();
	return v;
}
Value *ASTBinExpr::codegen(CodeGenContext &context) {
	Value *V = NULL;
	Instruction::BinaryOps instr;
	switch (opertor.c_str()[0]) {
		case '+': 	instr = Instruction::Add; break;
		case '-': 	instr = Instruction::Sub; break;
		case '*': 	instr = Instruction::Mul; break;
		case '/': 	instr = Instruction::SDiv; break;
		case '%':	instr = Instruction::SRem; break;
	}

	V = BinaryOperator::Create(instr, left->codegen(context), right->codegen(context), "", context.currentBlock());
	//V->dump();
	return V;
}
Value *ASTStmtDecls::codegen(CodeGenContext& context)
{
	for(int i = 0; i<declList.size(); i++) {
		//cout << "3rd Stops\n";
		declList[i]->codegen(context);
	}
}
// Value* ASTMethCall(CodeGenContext& context) {
// 	return NULL;
// }
Value* ASTCalloutarg::codegen(CodeGenContext &context) {
	expr->codegen(context);
}
Value* ASTCalloutargs::codegen(CodeGenContext &context) {
	// cout << "Here1\n";
	for(int i = 0; i<argList.size(); i++) {
		// cout << "Iter\n";
		argList[i]->codegen(context);
	}
	// cout << "Here2\n";
}
Value* ASTCallout::codegen(CodeGenContext& context) {
	// vector<const Type*> argTypes;
	int i;
	Type **argTypes = NULL;
	argTypes = (Type **)malloc(sizeof(Type*) * args->getCount());
	for (i = 0; i!=args->getCount(); i++) {
		// argTypes.push_back(typeOf());
		argTypes[i] = typeOf();
	}

	args->codegen(context);

	// vector <class ASTCalloutarg *> argList = args
	// CallInst *call = CallInst::Create(function, args->getArgsList().begin(), args->getArgsList().end(), "", context.currentBlock());
	// call->dump();
	return 0;
}
Value* ASTAssign::codegen(CodeGenContext& context) {
	// location->codegen(context);
	// expr->codegen(context);
	if (context.locals().find(location->getVarName()) == context.locals().end()) {
		std::cerr << "undeclared variable " << location->getVarName() << endl;
		return NULL;
	}
	Value *v = new StoreInst(expr->codegen(context), context.locals()[location->getVarName().c_str()], false, context.currentBlock());
	return v;
}
Value* ASTLocation::codegen(CodeGenContext& context) {
	if (context.locals().find(varName) == context.locals().end()) {
		std::cerr << "undeclared variable " << varName.c_str() << endl;
		return NULL;
	}
	// // cout << "Location Stop\n";
	// // cout << "VarName: " << varName.c_str() << endl;
	// cout << "Location due to " << varName.c_str() << endl;
	Value *V = new LoadInst(context.locals()[varName.c_str()], "", false, context.currentBlock());
	//V->dump();
	if(!loctype.compare("Array")){
		expr->codegen(context);
	}
	return V;
}
/********************************************************************************/
/*************************** CONSTRUCTORS ***************************************/

ASTVar::ASTVar(string declType, string varName, int length) {
	this->declType = declType;
	this->varName = varName;
	this->length = length;
}
ASTVar::ASTVar(string declType, string varName) {
	this->declType = declType;
	this->varName = varName;	
}
ASTFieldDecl::ASTFieldDecl(string dataType, class ASTVars *vars) {
	this->dataType = dataType;
	this->varList = vars->getVarList();
	for(int i = 0; i<varList.size(); i++) {
		varList[i]->setDataType(dataType);
	}
}
ASTAssign::ASTAssign(class ASTLocation *loc, class ASTExpr *expr) {
	this->location = loc;
	this->expr = expr;
}
ASTLocation::ASTLocation(string loctype, string varName, class ASTExpr *expr) {
	this->loctype = loctype;
	this->varName = varName;
	this->expr = expr;
	this->etype = exprType::location;
	//out << "1st Const. for Location called\n";

}
ASTLocation::ASTLocation(string loctype, string varName) {
	this->varName = varName;
	this->loctype = loctype;
	this->etype = exprType::location;
}

ASTIntLiteral::ASTIntLiteral(int value) {
	this->value = value;
	this->ltype = Int;
}
ASTCharLiteral::ASTCharLiteral(string value) {
	this->value = value[0];
	this->ltype = literalType::Char;
}
ASTBoolLiteral::ASTBoolLiteral(string value) {
	this->value = value;
	this->ltype = literalType::Bool;
}
ASTString::ASTString(string value) {
	this->value = value;
	this->ltype = literalType::String;
}
ASTBinExpr::ASTBinExpr(class ASTExpr *left, string opertor, class ASTExpr *right) {
	this->left = left;
	this->opertor = opertor;
	this->right = right;
	this->etype = exprType::binary;
}
ASTUnExpr::ASTUnExpr(string opertor, class ASTExpr *expr) {
	this->opertor = opertor;
	this->expr = expr;
	this->etype = exprType::unary;
}

ASTProg::ASTProg(class ASTFieldDecls *field_decls, class ASTStmtDecls *stmt_decls) {
	this->field_decls = field_decls;
	this->stmt_decls = stmt_decls;
}

ASTFieldDecls::ASTFieldDecls() {
	count = 0;
}
ASTStmtDecls::ASTStmtDecls() {
	count = 0;
}
ASTCalloutargs::ASTCalloutargs() {
	count = 0;
}
ASTParenExpr::ASTParenExpr(class ASTExpr *expr) {
	this->expr = expr;
	this->etype = exprType::parenExpr;
}
ASTExprs::ASTExprs() {
	count = 0;
}
ASTCalloutarg::ASTCalloutarg(class ASTExpr *expr) {
	this->expr = expr;
}
ASTCalloutarg::ASTCalloutarg(string literal) {
	class ASTLiteral *tmp1 = new ASTString(literal);
	this->expr = tmp1;
}
ASTCallout::ASTCallout(string methName, class ASTCalloutargs *args) {
	this->methName = methName;
	this->args = args;
}
ASTInBuilt::ASTInBuilt(string methName, class ASTExprs *exprs) {
	this->methName = methName;
	this->exprs = exprs;
}
/****************************** AUXILLARY FUNCTIONS *******************************/

void ASTVar::setDataType(string dataType) {
	this->dataType = dataType;
}

void ASTVars::push_back(class ASTVar *var) {
	varList.push_back(var);
}
void ASTVars::push_back_vars(class ASTVars *vars) {
	for(int i = 0; vars->varList.size(); i++) {
		varList.push_back(vars->varList[i]);
	}
}
vector <class ASTVar *> ASTVars::getVarList() {
	return varList;
}

void ASTFieldDecls::push_back(class ASTFieldDecl *fdecl) {
	declList.push_back(fdecl);
	count += fdecl->getVarList().size();
}
void ASTStmtDecls::push_back(class ASTStmtDecl *sdecl) {
	declList.push_back(sdecl);
	count++;
}
void ASTExprs::push_back(class ASTExpr *expr) {
	exprList.push_back(expr);
	count++;
}
void ASTCalloutargs::push_back(class ASTCalloutarg *arg) {
	argList.push_back(arg);
	count++;
}

vector <class ASTVar *> ASTFieldDecl::getVarList() {
	return varList;
}
int ASTIntLiteral::getValue() {
	return value;
}
string ASTIntLiteral::toString() {
	char num2str[1024];
	itoa(value, num2str, 10);
	return string(num2str); 
}
string ASTCharLiteral::toString() {
	char str[2];
	str[0] = value;
	str[1] = '\0';
	return string(str); 
}
string ASTBoolLiteral::toString() {
	return value; 
}
string ASTLocation::toString() {
	if(!loctype.compare("Number")) {
		return varName;
	}
	else {
		return varName + "[" + expr->toString() + "]";
	}
}
string ASTBinExpr::toString() {
	return left->toString() + opertor + right->toString();
}

void Traversal::Visit(ASTNode *node) {
	node->traverse();
}
/****************************** TRAVERSAL FUNCTIONS ********************************/
void ASTFieldDecl::traverse() {
	for(int i = 0; i<varList.size(); i++) {
		tab_count++;
		varList[i]->traverse();
		tab_count--;
	}
}
void ASTFieldDecl::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTVar::traverse() {
	TAB;
	out << "<declaration name=\"" << this->varName << "\"";
	if(!this->declType.compare("Array")) {
		out << " count=\"" << this->length << "\"";
	}
	out << " type=\"" << this->dataType << "\"/>" << endl;
}	
void ASTVar::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTFieldDecls::traverse() {
	TAB;
	out << "<field_declarations count=\"" << count << "\">" << endl;
	for(int i = 0; i < declList.size(); i++) {
		declList[i]->traverse();
	}
	TAB;
	out << "</field_declarations>" << endl;
}
void ASTFieldDecls::accept(Visitor *v)
{
	v->Visit(this);
}


void ASTVars::traverse() {
	for(int i = 0; i<varList.size(); i++) {
		tab_count++;
		varList[i]->traverse();
		tab_count--;
	}
}
void ASTVars::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTLocation::traverse() {
	TAB;
	out << "<location id=\"" << varName << "\">" << endl;
	if(!loctype.compare("Array")) {
		tab_count++;
		TAB;
		out << "<position>" << endl;
		
		tab_count++;
		expr->traverse();
		tab_count--;

		TAB;
		out << "</position>" << endl;
		tab_count--;
	}
	TAB;
	out << "</location>" << endl;
}
void ASTLocation::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTIntLiteral::traverse() {
	TAB;
	out << "<integer value=\"" << value << "\" />" << endl;
}
void ASTIntLiteral::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTCharLiteral::traverse() {
	TAB;
	out << "<character value=\"" << value << "\" />" << endl;

}
void ASTCharLiteral::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTBoolLiteral::traverse() {
	TAB;
	out << "<boolean value=\"" << value << "\" />" << endl;
}
void ASTBoolLiteral::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTString::traverse() {
	TAB;
	out << "<string value=\"" << value << "\" />" << endl;
}
void ASTString::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTAssign::traverse() {
	TAB;
	out << "<assignment>" << endl;
	tab_count++;
	location->traverse();
	expr->traverse();
	tab_count--;
	TAB;
	out << "</assignment>" << endl;
}
void ASTAssign::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTStmtDecls::traverse() {
	TAB;
	out << "<statement_declarations count=\"" << count << "\">" << endl;
	for(int i = 0; i<declList.size(); i++) {
		tab_count++;
		declList[i]->traverse();
		tab_count--;
	}
	TAB;
	out << "</statement_declarations>" << endl;
}
void ASTStmtDecls::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTCallout::traverse() {
	TAB;
	out << "<callout function=" << methName << ">" << endl;
	//tab_count++;
	args->traverse();
	//tab_count--;
	TAB;
	out << "</callout>" << endl;
}
void ASTCallout::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTInBuilt::traverse() {
	TAB;
	out << "<method_call function=\"" << methName << "\">" << endl;
	tab_count++;
	exprs->traverse();
	tab_count--;
	TAB;
	out << "</callout>" << endl;
}
void ASTInBuilt::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTBinExpr::traverse() {

	// if(left->getData)

	TAB;
	out << "<binary_expression type=\"";
	if (!opertor.compare("+"))
		out << "addition";
	else if (!opertor.compare("-"))
		out << "subtraction";
	else if (!opertor.compare("*"))
		out << "multiplication";
	else if (!opertor.compare("/"))
		out << "division";
	else if (!opertor.compare("%"))
		out << "remainder";
	else if (!opertor.compare("<"))
		out << "less_than";
	else if (!opertor.compare(">"))
		out << "greater_than";
	else if (!opertor.compare("<="))
		out << "less_equal";
	else if (!opertor.compare(">="))
		out << "greater_equal";
	else if (!opertor.compare("=="))
		out << "is_equal";
	else if (!opertor.compare("!="))
		out << "is_not_equal";
	else if (!opertor.compare("&&"))
		out << "and";
	else if (!opertor.compare("||"))
		out << "or";
	out <<"\">" << endl;
	tab_count++;
	left->traverse();
	right->traverse();
	tab_count--;
	TAB;
	out << "</binary_expression>" << endl;
}
void ASTBinExpr::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTUnExpr::traverse() {
	TAB;
	out << "<unary_expression type=\"";
	if (!opertor.compare("-"))
		out << "minus";
	else if (!opertor.compare("!"))
		out << "not";
	out <<"\">" << endl;
	tab_count++;
	expr->traverse();
	tab_count--;
	TAB;
	out << "</unary_expression>" << endl;
}
void ASTUnExpr::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTProg::traverse() {
	TAB;
	out << "<program>" << endl;
	tab_count++;
	field_decls->traverse();
	stmt_decls->traverse();
	tab_count--;
	TAB;
	out << "</program>" << endl;
}
void ASTProg::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTParenExpr::traverse() {
	TAB;
	out << "<paren_term>" << endl;
	tab_count++;
	expr->traverse();
	tab_count--;
	TAB;
	out << "</paren_term>" << endl;
}
void ASTParenExpr::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTExprs::traverse() {
	for(int i = 0; i<exprList.size(); i++) {
		tab_count++;
		exprList[i]->traverse();
		tab_count--;
	}
}
void ASTExprs::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTCalloutarg::traverse() {
	tab_count++;
	expr->traverse();
	tab_count--;
}
void ASTCalloutarg::accept(Visitor *v)
{
	v->Visit(this);
}

void ASTCalloutargs::traverse() {
	for(int i = 0; i<argList.size(); i++) {
		argList[i]->traverse();
	}
}
void ASTCalloutargs::accept(Visitor *v)
{
	v->Visit(this);
}
