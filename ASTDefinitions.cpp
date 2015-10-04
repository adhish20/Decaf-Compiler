#include "ASTDefinitions.h"
#include <string>
#include <vector>
#include <iostream>
#include<fstream>
#define TAB giveTabs()

using namespace std;
int tab_count = 0;
int tab_width = 4;
ofstream out("XML_visitor.txt");
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
/****************************** TRAVERSAL FUNCTIONS ********************************/
void ASTFieldDecl::traverse() {
	for(int i = 0; i<varList.size(); i++) {
		tab_count++;
		varList[i]->traverse();
		tab_count--;
	}
}
void ASTVar::traverse() {
	TAB;
	out << this->declType << endl;
	TAB;
	out << "<declaration name=\"" << this->varName << "\"";
	if(!this->declType.compare("Array")) {
		out << " count=\"" << this->length << "\"";
	}
	out << " type=\"" << this->dataType << "\"/>" << endl;
}	
void ASTFieldDecls::traverse() {
	TAB;
	out << "<field_declarations count=" << count << ">" << endl;
	for(int i = 0; i < declList.size(); i++) {
		declList[i]->traverse();
	}
	TAB;
	out << "</field_declarations>" << endl;
}

void ASTVars::traverse() {
	for(int i = 0; i<varList.size(); i++) {
		tab_count++;
		varList[i]->traverse();
		tab_count--;
	}
}

void ASTLocation::traverse() {
	TAB;
	out << "<location id=\"" << varName;
	if(!loctype.compare("Array")) {
		out << " position=\"" << expr->toString();
	}
	out << "\" />" << endl;
}
void ASTIntLiteral::traverse() {
	TAB;
	out << "<integer value=\"" << value << "\" />" << endl;
}
void ASTCharLiteral::traverse() {
	TAB;
	out << "<character value=\"" << value << "\" />" << endl;

}
void ASTBoolLiteral::traverse() {
	TAB;
	out << "<boolean value=\"" << value << "\" />" << endl;
}
void ASTString::traverse() {
	TAB;
	out << "<string value=\"" << value << "\" />" << endl;
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
void ASTStmtDecls::traverse() {
	TAB;
	out << "<statement declarations>" << endl;
	for(int i = 0; i<declList.size(); i++) {
		tab_count++;
		declList[i]->traverse();
		tab_count--;
	}
	TAB;
	out << "</statement declarations>" << endl;
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
void ASTInBuilt::traverse() {
	TAB;
	out << "<method_call function=\"" << methName << "\">" << endl;
	tab_count++;
	exprs->traverse();
	tab_count--;
	TAB;
	out << "</callout>" << endl;
}
void ASTBinExpr::traverse() {
	TAB;
	out << "<binary_expression type=\"" << opertor <<"\">" << endl;
	tab_count++;
	left->traverse();
	right->traverse();
	tab_count--;
	TAB;
	out << "</binary_expression>" << endl;
}
void ASTUnExpr::traverse() {
	TAB;
	out << "<unary_expression type=\"" << opertor <<"\">" << endl;
	tab_count++;
	expr->traverse();
	tab_count--;
	TAB;
	out << "</unary_expression>" << endl;
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
void ASTParenExpr::traverse() {
	TAB;
	out << "<paren_term>" << endl;
	tab_count++;
	expr->traverse();
	tab_count--;
	TAB;
	out << "</paren_term>" << endl;
}
void ASTExprs::traverse() {
	for(int i = 0; i<exprList.size(); i++) {
		tab_count++;
		exprList[i]->traverse();
		tab_count--;
	}
}
void ASTCalloutarg::traverse() {
	tab_count++;
	expr->traverse();
	tab_count--;
}
void ASTCalloutargs::traverse() {
	for(int i = 0; i<argList.size(); i++) {
		argList[i]->traverse();
	}
}