#include <Interpreter.h>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>



void runTimeTest(std::string& test){
    std::vector<Token> tokens;
    std::string fixed = fixup(test);
    getPostFix(fixed, tokens);
    double timeSum = 0;
    for(int i = 0; i < 1000; i++){
        auto start = std::chrono::steady_clock::now();
        evaluatePostFix(tokens);
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        timeSum += elapsed.count();
    }
    std::printf("Average Post fix evaluation time: %f micro seconds\n", timeSum / 1000);
    timeSum = 0;
    for(int i = 0; i < 1000; i++){
        auto start = std::chrono::steady_clock::now();
        evaluate(test);
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        timeSum += elapsed.count();

    }
    
    std::printf("Average Overall evaluation time: %f micro seconds\n", timeSum / 1000);

    std::printf("Calculated value was: %f\n", evaluate(test));
}

int main(int argc, char const *argv[])
{   


    std::ifstream eqnReader;
    eqnReader.open("test.txt");
    if(!eqnReader.is_open()){
        std::cout << "File not Found!" << std::endl;
        return 1;
    }
    std::string expression = "";
    std::string cur = "";
    std::ofstream outputFile("output.txt");
    while(std::getline(eqnReader,expression)){
        std::vector<Token> tokens;
        getPostFix(expression,tokens);
        std::string output = getPostFixString(tokens);
        outputFile << output << " ";
        outputFile << "Got: ";
        outputFile << evaluatePostFix(tokens);
        outputFile << std::endl;
        
        //runTimeTest(expression);
    }
    eqnReader.close();
    outputFile.close();
    return 0;  

}
