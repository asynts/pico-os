#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

int main() {
    int fd = open("/dev/ttyACM0", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Obtain current TTY configuration.
    struct termios tty;
    if (tcgetattr(fd, &tty) < 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Configure baudrate.
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // Apply modified TTY configuration.
    if (tcsetattr(fd, TCSANOW, &tty) < 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    char buffer[0x1000];
    for(;;) {
        ssize_t nread = read(fd, buffer, sizeof(buffer));
        if (nread < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        ssize_t nwritten = write(STDOUT_FILENO, buffer, nread);
        if (nwritten < 0) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
