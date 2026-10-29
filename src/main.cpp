

#include <Interpreter.h>
#include <iostream>
#include <string>
int main(int argc, char const *argv[])
{
    std::string test = "-(1 * -2) * (3 + 4)";
    std::printf("%f\n", evaluate(test));
    return 0;  

}
