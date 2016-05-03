#ifndef MIDTERMSH_COMAND_H
#define MIDTERMSH_COMAND_H

#include <string>
#include <vector>
#include "Split.h"

#include <iostream>
class Task {
private:
    static const char separator = ' ';
public:
    std::string name;
    std::vector<std::string> operands;

    Task(std::string s) {
        std::vector<std::string> tokens = Split::split(s, Task::separator);
        name = tokens.front();
        operands.insert(operands.end(), tokens.begin() + 1, tokens.end());
    }
};


#endif //MIDTERMSH_COMAND_H

