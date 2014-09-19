#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include "symbol_table.hpp"

using namespace std;

enum lintTypes{
  VARASSIGN = 1,
  OPECODEASSIGN,
  OPEADD
};

class analyzer{
 private:
  //
  char token[80];
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
  module_start.push_back(0);
}

analyzer::~analyzer() {}

void analyzer::parse(char* expr){
  pExpr = expr; //Why do a pExpr
  while(*pExpr){
    nextLine();
  }
}

void analyzer::nextLine(){
  // 1. judge the lineType
  // 2. get the module number
  // 3. parse the variable
  //
  lineType = 0;
  nextToken();
  if(tokenType != NUMBER){
    serror(0);
    return;
  }
  int buffer = atof(token);
  int n_token = 0;
  bool flag = false;
  char var_name[100];
  while(1){
    if( *pExpr == '\n'){ // next line
      pExpr++;
      break;
    }
    nextToken();
    n_token ++;
    if(!lineType){
      // judge
      if(strchr("AERI", *token)){
        lineType = OPEADD;
      } 
      else{
        switch(n_token){
          case 1:
            {
              char* bucket = var_name;
              char* t_bucket = token;
              while(*t_bucket){
                *bucket++ = *t_bucket++;
              }
              *bucket = '\0';
              flag = tokenType == VARIABLE;
              break;
            }
          case 2:
            flag = (tokenType == NUMBER) && flag;
        }
      }
    }
  }
  if(n_token == 1) flag = false; // in case there are only one token that line
  if(lineType != OPEADD){ // var assign
    switch(flag){
      case true:
        lineType = VARASSIGN;
        break;
      case false:
        lineType = OPECODEASSIGN;
        break;
    }
  }
  switch(lineType){
    case VARASSIGN:
      {
      char value[33];
      sprintf(value, "%d", atoi(token)+module_start.back());
      Symbol sym(var_name, VARIABLE, Integer, value);
      sym.PrintOutSym();
      break;
      }
    case OPEADD:
      module_start.push_back(module_start.back() + n_token / 2);
      break;
  }
}


void analyzer::nextToken(){
  char* bucket = token;
  *bucket = '\0';
  while(!isspace(*pExpr)){
    *bucket++ = *pExpr++;
  }
  if(!strchr("\n\r", *pExpr)){
  //if(*pExpr != '\n'){
    pExpr++; // skip the space except \n
  }
  *bucket = '\0';
  if(isalpha(*token)){
    tokenType = VARIABLE;
  }else if(isdigit(*token)){
    tokenType = NUMBER;
  }else{
    if(*pExpr == '\r'){
      std::cout << "Ends." << std::endl;
      exit(1);
    }
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
  fseek(fh, 0, SEEK_END);
  long s = ftell(fh);
  fseek(fh, 0, SEEK_SET);
  buffer = (char*)malloc(s);
  fread(buffer, s, 1, fh);
  fclose(fh);

  analyzer alz;
  alz.parse(buffer);

  free(buffer);
  return 0;
}
