#ifndef MIDTERMSH_UTILS_H
#define MIDTERMSH_UTILS_H
#include <vector>
#include <string>

struct Split {
public:
    static std::vector<std::string> split(const std::string &text, char sep) {
        std::vector<std::string> tokens;
        size_t start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos) {
            std::string temp = text.substr(start, end - start);
            if (temp != "")
                tokens.push_back(temp);
            start = end + 1;
        }
        std::string temp = text.substr(start);
        if (temp != "")
            tokens.push_back(temp);
        return tokens;
    }
};


#endif //MIDTERMSH_UTILS_H
