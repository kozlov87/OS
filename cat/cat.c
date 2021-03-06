#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

const int std_input = 0;
const int std_output = 1;
const int buff_size = 4096;

int cat(int f_in, int f_out) {
    int read_count;
    char buffer[buff_size];
    while ((read_count = read(f_in, buffer, buff_size)) > 0 || (errno == EINTR)) {
        int write_count = 0;
        while (write_count < read_count) {
            ssize_t cur_write = write(f_out, buffer + write_count, read_count - write_count);
            while (errno == EINTR) {
                ssize_t cur_write = write(f_out, buffer + write_count, read_count - write_count);
            }
            if (cur_write == -1) {
                return -1;
            }
            write_count += cur_write;
        }
    }
    if (read_count == 0) {
        return 0;
    }

    return -1;
}
int main(int argc, char ** argv) {
    if (argc > 1) {
    // file_descriptor
        int fd1 = open(argv[1], O_RDONLY);
        int fd2 = open(argv[2], O_CREAT | O_WRONLY, 0644);

        if (fd1 >= 0 && fd2 >= 0) {
            int cat_result = cat(fd1, fd2);
            if (cat_result != 0) {
                perror("Error occurs while reading data from file");
                return -1;
            }
            if (close(fd1) != 0) {
                perror("Error occurs while closing file");
                return -1;
            }

            if (close(fd2) != 0) {
                perror("Error occurs while closing file");
                return -1;
            }
        } else {
                perror("Error occurs while opening file");
                return -1;
        }
    } else {
        // если нет аргументов - берем данные из stdin
        if (cat(STDIN_FILENO, STDOUT_FILENO) != 0) {
            perror("Error occurs while reading data from stdin");
            return -1;
        }
    }
    return 0;
}
