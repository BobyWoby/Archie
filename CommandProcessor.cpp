#include "CommandProcessor.h"
#include <cassert>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>


using namespace cmd;

void Command::searchFirefox(std::vector<std::string> args){
    std::string query = "https://www.google.com/search?q=";
    bool dontGoogle = false;
    {
        int i = 0, length = sizeof(args[0]) / sizeof(args[0][0]);
        for(auto c : args[0]){
            if(i < length - 1 && c == '.') {
                dontGoogle = true;
                break;
            }
            i++;
        }
    }
    if(dontGoogle){
        query = "";
    }
    for(auto arg : args){
        for(int i = 0; i < arg.length(); i++){
            //account for any special characters here
            switch(arg[i]){
                case '+':
                    arg = arg.substr(0, i) + "%2B" + arg.substr(i+1, std::string::npos);
                    break;
                case ' ':
                    arg = arg.substr(0, i) + "+" + arg.substr(i+1, std::string::npos);
                    break;
            }
        }
        query += arg;
        query += '+';
    }
    query = "firefox --new-window " + query;
    system(query.c_str());
}

double Command::calculate(std::vector<std::string> args){
    //need to split the args if it gets parsed as one thing
    int opStart = 0 , opLength;
    if(args.size() == 1){
        for(int i = 0; i < args[0].length(); i++){
            std::string first = args[0];
            if(std::isalpha(first[i])){
                //theres a stringified operation somewhere here
                opStart = i;
            }
            if(!std::isdigit(first[i])){
                if(opStart){
                    //reached end of the operation
                    opLength++;
                }
            }
        }
        std::string op = args[0].substr(opStart, opLength);
        std::cout << op << std::endl;
        double result = 0, operand1 = std::stod(args[0].substr(0, opStart)), operand2 = std::stod(args[0].substr(opStart + opLength));
        if(op == "+" || op == "plus"){
            result = operand1 + operand2;
        }
        if(op == "x" || op == "times" || op == "*"){
            result =  operand1 * operand2;
        }
        if(op == "/" || op == "divided"){
            if(args[2] == "by"){
                operand2 = std::stod(args[3]);
            }
            return operand1 / operand2;
        }
        if(op == "-" || op == "minus"){
            return operand1 + operand2;
        }
        return result;
    }
    else{
        //TODO: Maybe implement other operations sometime idk
        //the operation got split up into multiple strings
        double operand1 = std::stod(args[0]), operand2 = 0;
        try {
        operand2 = std::stod(args[2]);
        } catch (const std::invalid_argument& e) {
            operand2 = std::stod(args[3]);
        }
        std::string op = args[1];
        if(op == "+" || op == "plus"){
            return operand1 + operand2;
        }
        if(op == "x" || op == "times" || op == "*"){
            return operand1 * operand2;
        }
        if(op == "/" || op == "divided"){
            if(args[2] == "by"){
                operand2 = std::stod(args[3]);
            }
            return operand1 / operand2;
        }
        if(op == "-" || op == "minus"){
            return operand1 + operand2;
        }
    }
    return 0;
}

void Command::run(std::vector<std::string> args){
    std::string cmd = "";
    for(auto arg : args){
        if(arg == "and"){
            cmd += "&";
            continue;
        }
        cmd += arg;
    }
    system(cmd.c_str());
}

void Command::parseCommand(std::vector<std::string> tokens){
    // for(auto tok : tokens){
    //     printf("%s\n", tok.c_str());
    // }
    int cmdIndex, tokIndex;
    for(int j = 0; j < tokens.size(); j++){
        for(int i = 0; i < commandsLength; i++){
            if(tokens[j] == allowedCommands[i]){
                cmdIndex = i;
                tokIndex = j;
                break;
            }
        }
    }
    std::vector<std::string> args;
    if(tokIndex + 1 < tokens.size()){
        for(int i = tokIndex +1; i < tokens.size(); i++){
            args.push_back(tokens[i]);
        }
    }
    assert(cmdIndex < commandsLength);
    switch(cmdIndex){

        case 0: // search
        case 1: // search sometimes gets read as surge
            if(args.size() < 1){
                printf("You need to search something");
            }
            assert(args.size() >= 1);
            searchFirefox(args);
            break;
        case 2: // calculate
                //parse the rest of the string
            std::cout << "result: " << calculate(args) << std::endl;
            break;
        case 3: //run
        case 4: // open
            if(args.size() < 1){
                printf("You need to run something");
            }
            assert(args.size() >= 1);
            run(args);
            break;
        case 5:
        case 6:
            if(args.size() < 1){
                printf("You need to search something");
            }
            assert(args.size() >= 1);
            searchFirefox(args);
            break;
        default:
            break;
    }

}
