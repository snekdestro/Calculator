#include <Interpreter.h>
#include <iostream>
#include <string>


int main(int argc, char const *argv[])
{
    std::string test = "log(sin((-(1 * -2) * (3 + 4))^2) * 1.5)";
    std::printf("%f\n", evaluate(test));
    //std::string test = "";
    //std::printf("Enter an equation\n");
    //std::cin >> test;
    //std::printf("%f\n", evaluate(test));
    return 0;  

}
