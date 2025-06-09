#include "CommandProcessor.h"
#include <cassert>
#include <cctype>
#include <cstdio>


using namespace cmd;

void Command::searchFirefox(std::vector<std::string> args){
    std::string query = "https://www.google.com/search?q=";
    bool dontGoogle = false;
    for(auto c : args[0]){
        if(c == '.') {
            dontGoogle = true;
            break;
        }
    }
    if(dontGoogle){
        query = "";
    }
    for(auto arg : args){
        query += arg;
    }
    query = "firefox --new-window " + query;
    system(query.c_str());
}

void Command::calculate(std::vector<std::string> args){
    //need to split the args if it gets parsed as one thing
    for(int i = 0; i < args[0].length(); i++){
        std::string first = args[0];
        if(std::isdigit(first[i])){

        }
    }
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
        case 0: // calculate
            //parse the rest of the string

            break;
        case 1: // search
            if(args.size() < 1){
                printf("You need to search something");
            }
            assert(args.size() >= 1);
            searchFirefox(args);
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
    }

}
