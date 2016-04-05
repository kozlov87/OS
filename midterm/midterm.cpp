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

using namespace std;

const int buffer_size = 4096;
volatile int current_pid = -1;

string readString(int file_descriptor) {
    char data[buffer_size];
    string result = "";
    int read_characters_count;
    while ((read_characters_count = read(file_descriptor, data, buffer_size)) > 0) {
        for(int i = 0; i < read_characters_count; ++i) {
            result += data[i];
            if(data[i] == '\n') {
                return result;
            }
        }
    }
    return result;
}

vector<string> split(const string &text, char sep) {
    vector<string> tokens;
    size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

void execute(string name, bool last) {
    pid_t  pid;
    int    status;

    if ((pid = fork()) < 0) {
        printf("Forking child process failed\n");
    } else {
        if (pid == 0) {
            if (execlp("sh", "sh", "-c", name.c_str(), (char*) 0) < 0) {
               printf("Exec failed\n");
               exit(1);
           }
       } else {
           current_pid = pid;
           waitpid(pid, &status, 0);
           current_pid = -1;
       }
    }
}

void sig_handler(int signo, siginfo_t *siginfo, void *context) {
    if(current_pid != -1 && signo == SIGINT) {
        kill(current_pid, SIGINT);
    }
}

int main() {
    struct sigaction sigact;
    sigact.sa_sigaction = *sig_handler;
    sigset_t block_mask;
    sigaddset (&block_mask, SIGINT);
    sigact.sa_mask = block_mask;

    if (sigaction(SIGINT, &sigact, NULL) != 0) {
        printf("error with sigint\n");
        return errno;
    }
    string input_string;
    do {
        write(STDOUT_FILENO, "$", 1);
        input_string = readString(STDIN_FILENO);
        vector<string> comands = split(input_string, '|');

        for(auto comand = comands.begin(); comand != comands.end(); ++comand) {
            bool last = false;
            if(comand == --comands.end()) {
                last = true;
            }
            execute(*comand, last);
        }
    } while(input_string != "");
}
