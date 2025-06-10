#pragma once
#include <string>
#include <vector>

namespace cmd {
    inline std::string allowedCommands[]={
        "search",
        "surge",
        "calculate",
        "run",
        "open",
        "whats", // this is for calculating, or if no operator words are found in between a number, then just google it
        "what", // this is for calculating, or if no operator words are found in between a number, then just google it
    };
    inline int commandsLength = sizeof(allowedCommands) / sizeof(allowedCommands[0]);
    class Command{
        public:

        static void searchFirefox(std::vector<std::string> args);
        static double calculate(std::vector<std::string> args);
        static void run(std::vector<std::string> args);
        static void parseCommand(std::vector<std::string> tokens);
    };
}


