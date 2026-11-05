#include "Interpreter.h"
#include <cctype>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <stack>
#include <cmath>



enum FunctionChar{
    SIN = '\x01',
    COS = '\x02',
    TAN = '\x03',
    ASIN = '\x04',
    ACOS = '\x05',
    ATAN = '\x06',
    LOG10 = '\x07',
    LN = '\x08',
    ABS = '\x09',
    SQRT = '\x0A',
    ENDUNARY
};

enum Priorities{
    LEFT,
    ADDITIVE,
    MULTIPLICATIVE,
    EXPONENTIAL,
    UNARYMATHFUNCTION,
    RIGHT
};


int getPrecedence(const Operation& op) {
    // std::visit automatically figures out whether it's looking at 
    // a BinaryOperation or a UnaryOperation and accesses .priority
    return std::visit([](const auto& o) { return o.priority; }, op);
}
struct UnaryMapping {
    std::string name;
    char code;
};

const std::vector<UnaryMapping> UnaryFunctions = {
    {"asin", ASIN},
    {"acos", ACOS},
    {"atan", ATAN},
    {"sin", SIN},
    {"cos",  COS},
    {"tan",  TAN},
    {"log", LOG10},
    {"ln", LN},
    {"abs", ABS},
    {"sqrt", SQRT}
};
const std::unordered_map<char, std::string> getFuncBack ={
    {SIN,"sin"},
    {COS,"cos"},
    {TAN,"tan"},
    {ASIN,"asin"},
    {ACOS,"acos"},
    {ATAN,"atan"},
    {LOG10,"log"},
    {LN,"ln"},
    {ABS,"abs"},
    {SQRT, "sqrt"},
    {'+', "+"},
    {'-', "-"},
    {'*', "*"},
    {'/', "/"},
    {'%', "%"},
    {'^', "^"},
    {'(', "("},
    {')', ")"}

};
const std::unordered_map<std::string, double> constants ={
    {"e",  2.718281828459045},
    {"pi", 3.141592653589793}
};
const std::unordered_map<char, Operation> operationMap = {
    {'+', BinaryOperation{'+', add,   ADDITIVE}},
    {'-', BinaryOperation{'-', sub, ADDITIVE}},
    {'*', BinaryOperation{'*', mult,  MULTIPLICATIVE}},
    {'/', BinaryOperation{'/', div, MULTIPLICATIVE}}, 
    {'%', BinaryOperation{'%', mod,   MULTIPLICATIVE}},
    {'^', BinaryOperation{'^', power, EXPONENTIAL}},
    {'(', BinaryOperation{'(', dummy, LEFT}},
    {')', BinaryOperation{')', dummy, RIGHT}},
    {SIN, UnaryOperation{SIN, std::sin, UNARYMATHFUNCTION}},
    {COS, UnaryOperation{COS, std::cos, UNARYMATHFUNCTION}},
    {TAN, UnaryOperation{TAN, std::tan, UNARYMATHFUNCTION}},
    {ASIN, UnaryOperation{ASIN, std::asin, UNARYMATHFUNCTION}},
    {ACOS, UnaryOperation{ACOS, std::acos, UNARYMATHFUNCTION}},
    {ATAN, UnaryOperation{ATAN, std::atan, UNARYMATHFUNCTION}},
    {LOG10, UnaryOperation{LOG10,std::log10, UNARYMATHFUNCTION}},
    {LN, UnaryOperation{LN, std::log, UNARYMATHFUNCTION}},
    {ABS, UnaryOperation{ABS,std::abs, UNARYMATHFUNCTION}},
    {SQRT, UnaryOperation{SQRT,std::sqrt, UNARYMATHFUNCTION}}
};
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        // Advance past the newly replaced chunk to avoid infinite loops
        start_pos += to.length(); 
    }
}

double add(double a, double b){
    return a + b;
}
double sub(double a, double b){
    return a -b;
}
double div(double a, double b){
    return a /b;
}
double mult(double a, double b){
    return a * b;
}
double mod(double a, double b){
    return std::fmod(a,b);
}
double power(double a, double b){
    return std::pow(a,b);
}
double dummy(double a, double b){
    return -1; //shouldn't be here dumbass
}
//lexers are for fucking nerds; lets just fucking walk through the string till we find the shit we need. 
//With good maps and shit, this stuff is easy to extend!
std::string fixup(std::string& expression){
    expression.erase(std::remove(expression.begin(), expression.end(), ' '), expression.end()); //cull spaces for formatting
    for (const auto& Unary : UnaryFunctions) {
        replaceAll(expression, Unary.name, std::string(1, Unary.code));
    }
    for(const auto&[constant, value] : constants){
        replaceAll(expression,constant,std::to_string(value));
    }
    if (expression.length() <= 1) {
        return expression;
    }
    std::string ret = "";
    ret += expression.at(0);
    for(int i = 1; i < expression.length(); i++){
        char working = expression.at(i);

        if(working == '(' || working < ENDUNARY){
            if(std::isalnum(expression.at(i -1))){
                ret += '*';
                
            }
            if(expression.at(i-1) == '-'){
                if(i - 2 < 0){
                    ret += "1*";
                }else if(!std::isalnum(expression.at(i - 2))){
                    ret += "1*";
                }
                
            }
            /*I don't care if a / -(some shit) becomes a / (-1) * (some shit) = (-a) / (some shit)
            the basic division symbol and, by proxy, "/" are kind of ambigous and stupid. 
            If this causes confusion then blame division.  
            Oh and I guess this affects modulo (%). Womp womp.
            */
            ret += working;
        }else if(working == ')'){
            //gotta walk forwards
            ret += working;
            if(i + 1 < expression.length()){
                if(expression.at(i+1) == '('){
                    ret += '*';
                }
                if(std::isalnum(expression.at(i+1))){
                    ret += '*';
                }
            }
        }else if(std::isalpha(working)){
            if(std::isalnum(expression.at(i - 1))){
                ret += '*';
            }
            ret += working;
            if(i + 1 < expression.length()){
                if(std::isalnum(expression.at(i + 1))){
                    ret += '*';
                }
            }
        }else{
            ret += working;
        }
    }
    return ret;
}

void checkStack(std::vector<Token>& tokens, std::stack<Operation>& stack, Operation o){
    
    if(stack.empty()){
        stack.push(o);
        return;
    }
    
    if(getPrecedence(o) == LEFT){
        stack.push(o);
        return;
    }
    if(getPrecedence(o) == RIGHT){
        while(!stack.empty()){
            if(getPrecedence(stack.top()) == LEFT){

                stack.pop();
                break;
            }
            tokens.push_back(stack.top());
            stack.pop();
        }
        return;
    }

    if(getPrecedence(o) <= getPrecedence(stack.top())){
        while(!stack.empty()){
            if(getPrecedence(o) > getPrecedence(stack.top())){
                break;
            }
            tokens.push_back(stack.top());
            stack.pop();
        }
    }
    stack.push(o);



}
void getPostFix(std::string& expression, std::vector<Token>& tokens){
    std::stack<Operation> stack;
    expression = fixup(expression);
    
    int offset = 1;
    if(expression.at(0) == '-'){
        for(; offset < expression.length(); offset++){
            if(!(std::isdigit(expression.at(offset) | (expression.at(offset) != '.')))){
                break;
            }
            
        }
        tokens.push_back(std::stod(expression.substr(0,offset)));
   
    }else if(expression.at(0) == '('){
        checkStack(tokens,stack,BinaryOperation{'(', dummy, LEFT});
    }
    else{
        offset = 0;
    }
    int i = offset;

    while(i < expression.length()){
        char working = expression.at(i);
        //std::cout << working;
        if(working == '-'){
            if((!std::isalnum(expression.at(i -1))) & (expression.at(i-1) != ')')){
                
                int j = i + 1;
                for(;j < expression.length(); j++){
                    if(!std::isdigit(expression.at(j)) & (expression.at(j) != '.')){
                        break;
                    }
                }
                //construct double
                tokens.push_back(std::stod(expression.substr(i,j-i)));
                i = (j); //don't laugh;
                continue;
                
            }else{
                //subtract!
                checkStack(tokens,stack, BinaryOperation{'-', sub,ADDITIVE});
                
                
            }
        }else if(std::isalpha(working)){ //assumbed variable! (1 CHAR)
            tokens.push_back(Variable{working});
        }else if(std::isdigit(working)){
            int j = i+1;
            for(; j < expression.length(); j++){
                

                    if(!std::isdigit(expression.at(j)) & (expression.at(j) != '.')){
                        break;
                    }
           
                
            }
            tokens.push_back(std::stod(expression.substr(i,j-i)));
            i = (j); //don't laugh;
            continue;

        }else{
            auto it = operationMap.find(working);
            if (it != operationMap.end()) {
                checkStack(tokens, stack, it->second);
            }
            
        }

        i++;

    }
    while(!stack.empty()){
        tokens.push_back(stack.top());
        stack.pop();
    }



}



double evaluatePostFix(const std::vector<Token>& tokens){
    std::stack<double,std::vector<double>> nums;
    for(int i = 0; i < tokens.size(); i++){
        if(std::holds_alternative<double>(tokens[i])){
            nums.push(std::get<double>(tokens[i]));
        }else if(std::holds_alternative<Operation>(tokens[i])){
            Operation o = std::get<Operation>(tokens[i]);
            if(std::holds_alternative<BinaryOperation>(o)){
                BinaryOperation binary = std::get<BinaryOperation>(o);
                double p1 = nums.top(); nums.pop();
                double p2 = nums.top(); nums.pop();
                
                nums.push(binary.func(p2,p1));

            }else if (std::holds_alternative<UnaryOperation>(o)){
                UnaryOperation unary = std::get<UnaryOperation>(o);
                double p1 = nums.top(); nums.pop();
                nums.push(unary.func(p1));


            }
  
        }else{
            throw std::invalid_argument("Variable expression where constants are expected");
        }
    }
    return nums.top();
}
double evaluatePostFix(const std::vector<Token>& tokens, const std::unordered_map<char, double>& variables){
    std::stack<double,std::vector<double>> nums;
    for(int i = 0; i < tokens.size(); i++){
        if(std::holds_alternative<double>(tokens[i])){
            nums.push(std::get<double>(tokens[i]));
        }else if(std::holds_alternative<Operation>(tokens[i])){
            Operation o = std::get<Operation>(tokens[i]);
            if(std::holds_alternative<BinaryOperation>(o)){
                BinaryOperation binary = std::get<BinaryOperation>(o);
                double p1 = nums.top(); nums.pop();
                double p2 = nums.top(); nums.pop();
                
                nums.push(binary.func(p2,p1));

            }else if (std::holds_alternative<UnaryOperation>(o)){
                UnaryOperation unary = std::get<UnaryOperation>(o);
                double p1 = nums.top(); nums.pop();
                nums.push(unary.func(p1));


            }
  
        }else if(std::holds_alternative<Variable>(tokens[i])){
            auto boundValue = variables.find(std::get<Variable>(tokens[i]).name);
            if(boundValue == variables.end()){
                throw std::invalid_argument("Unknown variable in expression");
            }
            nums.push(boundValue->second);
        
        }
    }
    return nums.top();
}

double evaluate(std::string& expression){
    std::vector<Token> tokens;
    getPostFix(expression, tokens);
    return evaluatePostFix(tokens);
}
std::string getPostFixString(const std::vector<Token>& tokens){
    std::string output ="";
    for(int i = 0;  i < tokens.size(); i++){
        if(std::holds_alternative<double>(tokens[i])){
            
            output += std::to_string(std::get<double>(tokens[i]));
        }else if(std::holds_alternative<Operation>(tokens[i])){
            Operation o = std::get<Operation>(tokens[i]);
            if(std::holds_alternative<BinaryOperation>(o)){
                
                BinaryOperation binary = std::get<BinaryOperation>(o);
                output += getFuncBack.find(binary.symbol)->second;
            }else if (std::holds_alternative<UnaryOperation>(o)){
                UnaryOperation unary = std::get<UnaryOperation>(o);
                output += getFuncBack.find(unary.symbol)->second;


            }
  
        }else if(std::holds_alternative<Variable>(tokens[i])){
            Variable var = std::get<Variable>(tokens[i]);
            output += var.name;
        
        }
        output += " ";




    }
    return output;


}