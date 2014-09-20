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

enum AddType{
  RELATIVE,
  ABSOLUTE,
  IMEDIATE,
  EXTERNAL
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
  vector<int>::iterator it;
  char* pExpr_srt;

  // second pass
  vector<int> lineTypePass;
  vector<int>::iterator itlnType;
  vector<string> ope_buffer;
  int n_add;
  int addtype;

  // first pass
  void nextLineFirst();
  void nextToken();
  void serror(int);

  // second pass
  void nextLineSecond();


   
 public:
  analyzer();
  ~analyzer();
  void parse(char* expr); //entry of the parser
  void addmap(char* expr);
};

analyzer::analyzer(){
  std::cout << "analyzer built." << std::endl;
  pExpr = NULL;
  module_start.push_back(0);
  n_add = 0;
}

analyzer::~analyzer() {}

void analyzer::parse(char* expr){
  pExpr = expr; //Why do a pExpr
  while(*pExpr != '\r'){ // TODO three times
    nextLineFirst();
  }
}

void analyzer::addmap(char* expr){
  pExpr = expr;
  it = module_start.begin();
  while(itlnType++ != lineTypePass.end()){
    nextLineSecond();
  }
}


void analyzer::nextLineFirst(){
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
    if( *pExpr == '\r'){// TODO three times
      return;
    }
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
;    case true:
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
  lineTypePass.push_back(lineType);
}


void analyzer::nextLineSecond(){
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
  switch(*itlnType){
    case VARASSIGN:
      while(*pExpr++!='\n') {}
      return;
    case OPECODEASSIGN:
      while(*pExpr != '\n'){
        nextToken();
        n_token++;
        ope_buffer.push_back(string(token));
      }
      pExpr++;
    case OPEADD:
      vector<string>::iterator it_ext = ope_buffer.begin();
      while(*pExpr != '\n'){
        nextToken();
        n_token++;
        switch(*token){
          case('R'):
            // relative
            addtype = RELATIVE;
            std::cout << n_add++ << ": ";
            break;
          case('A'):
            addtype = ABSOLUTE;
            std::cout << n_add++ << ": ";
            break;
          case('E'):
            addtype = EXTERNAL;
            std::cout << n_add++ << ": ";
            break;
          case('I'):
            addtype = IMEDIATE;
            std::cout << n_add++ << ": ";
            break;
        }
        nextToken();
        switch(addtype){
          case('R'):
            std::cout << *it+atoi(token) << std::endl;
            break;
          case('A'):
            std::cout << atoi(token) << std::endl;
            break;
          case('E'):
            for (int i = 0; i < st.symbols.size(); i++) { // scan
              if(st.symbols[i].GetName() == *it_ext){
                st.symbols[i].GetValue(); // TODO substitute *token function?
                break;
              }
            }

            std::cout << "TODO"  << std::endl;
            break;
          case('I'):
            std::cout << atoi(token) << std::endl;
            break;
        }
      }
      pExpr++;
      it++;
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
    if(*pExpr == '\r'){  // TODO three times
      std::cout << "Ends." << std::endl;
      return;
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
  alz.parse(buffer); // Not safe! const!
  alz.addmap(buffer); // Not safe! const!

  free(buffer);
  return 0;
}
