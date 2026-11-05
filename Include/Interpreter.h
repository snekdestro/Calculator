#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <variant>
#include <stdexcept>
#include <unordered_map>


double add(double a, double b);
double sub(double a, double b);
double div(double a, double b);
double mult(double a, double b);
double mod(double a, double b);
double power(double a, double b);
double dummy(double a, double b);



struct BinaryOperation{
    char symbol;
    double (*func)(double,double);
    int priority;
};
struct UnaryOperation{
    char symbol;
    double (*func)(double);
    int priority;
};


struct Variable{
    char name;
};
using Operation = std::variant<BinaryOperation, UnaryOperation>;
using Token = std::variant<double, Operation, Variable>;


double evaluate(std::string& expression);
std::string fixup(std::string& expression);
void getPostFix(std::string& expression, std::vector<Token>& tokens);
double evaluatePostFix(const std::vector<Token>& tokens);
double evaluatePostFix(const std::vector<Token>& tokens, const std::unordered_map<char, double>& variables);
std::string getPostFixString(const std::vector<Token>& tokens);





#endif




