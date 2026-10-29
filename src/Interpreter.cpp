#include "Interpreter.h"
#include <algorithm> 
#include <cctype>
#include <string>
#include <iostream>
#include <stdexcept>
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

std::string fixup(std::string expression){
    expression.erase(std::remove(expression.begin(), expression.end(), ' '), expression.end()); //cull spaces for formatting
    if (expression.length() <= 1) {
        return expression;
    }
    std::string ret = "";
    ret += expression.at(0);
    //lets pretend functions like sin(x) don't exist yet :)
    for(int i = 1; i < expression.length(); i++){
        char working = expression.at(i);

        if(working == '('){
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
            //functions will be a bitch to handle, lets pretend they don't exist yet
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
    std::cout << ret << std::endl;
    return ret;
}

bool isOp(char c){
    return (c == '+' || c == '%' || c == '-' || c == '/' || c == '*' || c == '^');
}
void checkStack(std::vector<Token>& tokens, std::stack<Operation>& stack, Operation o){
    if(stack.empty()){
        stack.push(o);
        return;
    }
    if(o.priority == LEFT){
        stack.push(o);
        return;
    }
    if(o.priority == RIGHT){
        while(!stack.empty()){
            if(stack.top().priority == LEFT){

                stack.pop();
                break;
            }
            tokens.push_back(stack.top());
            stack.pop();
        }
        return;
    }

    if(o.priority < stack.top().priority){
        while(!stack.empty()){
            if(o.priority > stack.top().priority){
                break;
            }
            tokens.push_back(stack.top());
            stack.pop();
        }
    }
    stack.push(o);
    //std::cout << stack.top().symbol << std::endl;



}
void getPostFix(std::string& expression, std::vector<Token>& tokens){
    std::stack<Operation> stack;
    expression = fixup(expression);
    
    int offset = 1;
    if(expression.at(0) == '-'){
        for(; offset < expression.length(); offset++){
            //std::cout << offset << std::endl;
            if(!(std::isdigit(expression.at(offset) | (expression.at(offset) != '.')))){
                break;
            }
        }
        //std::cout << expression.substr(0,offset);
        tokens.push_back(std::stod(expression.substr(0,offset)));
   
    }else if(expression.at(0) == '('){
        checkStack(tokens,stack,Operation{'(', dummy, LEFT});
    }
    else{
        offset = 0;
    }
    int i = offset;

    while(i < expression.length()){
        char working = expression.at(i);

        if(working == '-'){
            if(!std::isalnum(expression.at(i -1))){
                int j = i + 1;
                for(;j < expression.length(); j++){
                    if(!std::isdigit(expression.at(j))){
                        break;
                    }
                }
                //construct double
                tokens.push_back(std::stod(expression.substr(i,j-i)));
                i = (j); //don't laugh;
                continue;
                
            }else{
                //subtract!
                checkStack(tokens,stack, Operation{'-', sub,ADDITIVE});
                
            }
        }
        else if(working == '+'){
            checkStack(tokens, stack, Operation{'+',add,ADDITIVE});
        }
        else if(working == '*'){
            checkStack(tokens,stack, Operation{'*',mult, MULTIPLICATIVE});
        }
        else if(working == '/'){
            checkStack(tokens,stack, Operation{'/', div,MULTIPLICATIVE});
        }
        else if(working == '%'){
            checkStack(tokens,stack, Operation{'%',mod, MULTIPLICATIVE});
        }
        else if(working == '^'){
            checkStack(tokens,stack, Operation{'^', power, EXPONENTIAL});
        }else if(working == '('){
            checkStack(tokens,stack,Operation{'(', dummy, LEFT});
        }
        else if(working == ')'){
            checkStack(tokens,stack,Operation{')', dummy, RIGHT});
        }
        else if(std::isalpha(working)){ //assumbed variable! (1 CHAR)
            tokens.push_back(Variable{working});
        }else if(std::isdigit(working)){
            int j = i+1;
            for(; j < expression.length(); j++){
                

                    if(!std::isdigit(expression.at(j))){
                        break;
                    }
           
                
            }
            tokens.push_back(std::stod(expression.substr(i,j-i)));
            i = (j); //don't laugh;
            continue;

        }
        i++;

    }
    while(!stack.empty()){
        tokens.push_back(stack.top());
        stack.pop();
    }



}



double evaluatePostFix(const std::vector<Token>& tokens){
    std::stack<double> nums;
    for(int i = 0; i < tokens.size(); i++){
        if(std::holds_alternative<double>(tokens.at(i))){
            nums.push(std::get<double>(tokens.at(i)));
        }else if(std::holds_alternative<Operation>(tokens.at(i))){
            double p1 = nums.top(); nums.pop();
            double p2 = nums.top(); nums.pop();
            Operation o = std::get<Operation>(tokens.at(i));
            std::printf("%f %c %f\n", p2,o.symbol,p1);
            nums.push(o.func(p2,p1));
        }else{
            throw std::invalid_argument("Variable expression where const is expected");
        }
    }
    return nums.top();



}

double evaluate(std::string& expression){
    std::vector<Token> tokens;
    getPostFix(expression, tokens);
    return evaluatePostFix(tokens);
}