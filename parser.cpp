#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include "symbol_table.hpp"

using namespace std;

enum lintTypes{
  VARASSIGN = 0,
  OPECODEASSIGN,
  OPEADD
};

class analyzer{
 private:
  //
  char token[100];
  int tokenType;
  int lineType;
  char* pExpr;
  SymbolTable st;
  vector<int> module_start;
  char* pExpr_srt;

  // first pass
  void nextLine();
  void nextToken();
  void serror(int);

  // second pass


   
 public:
  analyzer();
  ~analyzer();
  void parse(char* expr); //entry of the parser
};

analyzer::analyzer(){
  std::cout << "analyzer built." << std::endl;
  pExpr = NULL;
}

analyzer::~analyzer() {}

void analyzer::parse(char* expr){
  pExpr = expr; //Why do a pExpr
  while(!*pExpr){
    nextLine();
  }
}

void analyzer::nextLine(){
  // Re-organize this part
  // 1. judge the lineType
  // 2. get the module number
  // 3. parse the variable
  //
  nextToken();
  if(tokenType != NUMBER){
    serror(0);
    return;
  }
  int buffer = atof(token);
  int n_token = 0;
  bool flag = true;
  char* var_name;
  while(*pExpr != 9){
    nextToken();
    n_token ++;
    if(*token == '0'){
      std::cout << "New module." << std::endl;
      break;
    }
    if(!lineType){
      // judge
      if(strchr("AERI", *token)){
        lineType = OPEADD;
      } 
      else{
        if(tokenType == VARIABLE && n_token == 1){
          var_name = token;
          flag = true;
        } 
        if(tokenType == NUMBER && n_token == 2) flag = flag && true;
      }
    }
  }
  if(!lineType){
    switch(flag){
      case true:
        lineType = VARASSIGN;
      case false:
        lineType = OPECODEASSIGN;
    }
  }
  switch(lineType){
    case VARASSIGN:
      {
      char value[33];
      sprintf(value, "%d", atof(token)+module_start.back());
      Symbol sym(var_name, VARIABLE, Integer, value);
      sym.PrintOutSym();
      }
    case OPEADD:
      module_start.push_back(module_start.back() + n_token / 2);
  }
}


void analyzer::nextToken(){
  char* bucket = token;
  *bucket = '\0';
  while(!isspace(*pExpr)){
    *bucket++ = *pExpr++;
  }
  if(isalpha(*bucket)){
    tokenType = VARIABLE;
  }else if(isdigit(*bucket)){
    tokenType = NUMBER;
  }else{
    serror(1);
  }
}


void analyzer::serror(int eno){
  switch(eno){
    case 0:
      std::cout << "Syntax error." << std::endl;
    case 1:
      std::cout << "Type error." << std::endl;
  }
}

int main(int argc, const char *argv[])
{
  char* buffer;
  FILE *fh = fopen("input", "r");
  fseek(fh, 0L, SEEK_END);
  long s = ftell(fh);
  buffer = (char*)malloc(s);
  fread(buffer, s, 1, fh);

  analyzer alz;
  alz.parse(buffer);

  free(buffer);
  fclose(fh);
  return 0;
}
