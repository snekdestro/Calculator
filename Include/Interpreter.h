#pragma once

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


enum Priorities{
    LEFT,
    ADDITIVE,
    MULTIPLICATIVE,
    EXPONENTIAL,
    RIGHT
};

struct Operation{
    char symbol;
    double (*func)(double,double);
    int priority;
};
struct Variable{
    char name;
};
using Token = std::variant<double, Operation, Variable>;


double evaluate(std::string& expression);
std::string fixup(std::string expression);
void getPostFix(std::string& expression, std::vector<Token>& tokens);
double evaluatePostFix(const std::vector<Token>& tokens);
//TODO: evaluate with subs for variables!!











