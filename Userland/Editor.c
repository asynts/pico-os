#include <stdio.h>
#include <readline/readline.h>
#include <fcntl.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

struct buffer {
    char *data;
    size_t size;
    size_t offset;
};

int main() {
    struct buffer buf = { .data = malloc(0x200), .size = 0x200, .offset = 0 };

    for (;;) {
        char *raw_line = readline("% ");
        assert(raw_line != NULL);

        if (strchr(raw_line, '\n'))
            *strchr(raw_line, '\n') = 0;

        char *line = raw_line;

        if (*line == 'w') {
            ++line;

            assert(*line == ' ');
            ++line;

            // FIXME: Add O_TRUNC
            int fd = open(line, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
            assert(fd >= 0);

            ssize_t retval = write(fd, buf.data, buf.offset);
            assert(retval >= 0);
            assert(retval == buf.offset);

            goto next_iteration;
        }
        if (*line == 'q') {
            ++line;

            assert(strlen(line) == 0);

            free(raw_line);
            break;
        }

    next_iteration:
        free(raw_line);
    }

    free(buf.data);
}
