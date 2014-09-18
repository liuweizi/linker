#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

enum Types{
  VARIABLE = 0,
  NUMBER,
  DELIMITER
};

enum ValueType{
  Boolean = 0,
  String,
  Integer,
  Nil
};

class Symbol{
 private:
   string name;
   Types token_type;
   ValueType value_type;
   string value;
 public:
   Symbol(string, Types, ValueType, string);
   ~Symbol();
   string GetName();
   ValueType GetValueType();
   Types GetType();
   string GetValue();
   void PrintOutSym();
};

Symbol::Symbol(string n, Types t, ValueType vt, string v):
  name(n),
  token_type(t),
  value_type(vt),
  value(v) {}

Symbol::~Symbol(){}

string Symbol::GetName(){
  return name;
}

ValueType Symbol::GetValueType(){
  return value_type;
}

Types Symbol::GetType(){
  return token_type;
}

string Symbol::GetValue(){
  return value;
}

void Symbol::PrintOutSym(){
  std::cout << name << " ";
  std::cout << value << std::endl;
}

struct SymbolTable{
  vector<Symbol> symbols;
};
  

