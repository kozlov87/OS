#include "Task.h"
#include "Split.h"
#include <sys/wait.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <signal.h>
#include <iostream>
#include <algorithm>

const int BUFFER_SIZE = 4096;

std::vector<pid_t> pids;

std::string readComand() {
    std::vector<char> buffer(BUFFER_SIZE, 0);

    std::string result = "";
    ssize_t char_count;
    while ((char_count = read(STDIN_FILENO, buffer.data(), BUFFER_SIZE)) > 0) {
        result.append(buffer.data(), (unsigned long) char_count);
        if(buffer[char_count - 1] == '\n') {
            result.pop_back();
            return result;
        }
    }
    if(char_count < 0) {
        perror("Error while reading comand");
    }

    return result;
}

std::vector<Task> getComands() {
    auto parts = Split::split(readComand(), '|');
    std::vector<Task> comands;
    std::transform(parts.begin(), parts.end(), std::back_inserter(comands),
                   [](std::string s) {return Task(s);}
    );
    return comands;
}


std::vector<std::string> split(const std::string &input_str, char target_char) {
        std::vector<std::string> symbols;
        size_t start = 0, end = 0;
        while ((end = input_str.find(target_char, start)) != std::string::npos) {
            std::string cur_str = input_str.substr(start, end - start);
            if (cur_str != "")
                symbols.push_back(cur_str);
            start = end + 1;
        }
        std::string cur_str = input_str.substr(start);
        if (cur_str != "")
            symbols.push_back(cur_str);
        return symbols;
}

void start_tasks(std::vector<Task> tasks, std::vector<pid_t> &pids) {
    int pipe1[2], pipe2[2];
    for(auto cur_task = tasks.begin(); cur_task != tasks.end(); ++cur_task) {
        pipe(pipe2);
        pid_t pid = fork();
        if (pid) {
            if (cur_task != tasks.begin()) {
                close(pipe1[0]);
                close(pipe1[1]);
            }
            if(cur_task == (--tasks.end())) {
                close(pipe2[0]);
                close(pipe2[1]);
            }

            pids.push_back(pid);
            int status;
            waitpid(pid, &status, 0);
        } else {
            if (cur_task != tasks.begin()) {
                dup2(pipe1[0], STDIN_FILENO);
                close(pipe1[0]);
                close(pipe1[1]);
            }
            if(cur_task != (--tasks.end())) {
                dup2(pipe2[1], STDOUT_FILENO);
            }
            close(pipe2[0]);
            close(pipe2[1]);

            std::vector<char *> ops {strdup(cur_task->name.c_str())};
            for(auto op: cur_task->operands) {
                ops.push_back(strdup(op.c_str()));
            }
            ops.push_back(NULL);
            execvp(cur_task->name.c_str(), ops.data());
        }
        pipe1[0] = pipe2[0];
        pipe1[1] = pipe2[1];
    }
}

void sig_handler(int sig) {
    std::cerr << "SIGINT!\n";
    for (pid_t pid : pids) {
        kill(pid, sig);
    }
}

int main() {
    struct sigaction act;
    act.sa_handler = &sig_handler;
    sigaction(SIGINT, &act, NULL);

    while(true) {
        write(STDOUT_FILENO, "$", 1);

        auto tasks = getComands();
        if (tasks.empty()) {
            break;
        }
        start_tasks(tasks, pids);
        pids.clear();
    }
}
