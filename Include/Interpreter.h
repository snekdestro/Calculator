#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <variant>
#include <stdexcept>

double add(double a, double b);
double sub(double a, double b);
double div(double a, double b);
double mult(double a, double b);
double mod(double a, double b);
double power(double a, double b);
double dummy(double a, double b);

enum FunctionChar{
    SIN = '\x01',
    COS = '\x02',
    TAN = '\x03',
    ASIN = '\x04',
    ACOS = '\x05',
    ATAN = '\x06',
    LOG10 = '\x07',
    LN = '\x08'
};

enum Priorities{
    LEFT,
    ADDITIVE,
    MULTIPLICATIVE,
    EXPONENTIAL,
    UNARYMATHFUNCTION,
    RIGHT
};

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
//TODO: evaluate with subs for variables!!






#endif




