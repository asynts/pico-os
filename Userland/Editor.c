#include <stdio.h>
#include <readline/readline.h>
#include <fcntl.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct buffer {
    char *data;
    size_t size;
    size_t used;
    size_t last_lineno;
};

void buffer_append_at_offset(struct buffer *buf, size_t offset, const char *data, size_t data_size)
{
    assert(buf->size >= buf->used + data_size);

    memmove(buf->data + offset + data_size, buf->data + offset, data_size);
    memcpy(buf->data + offset, data, data_size);

    buf->used += data_size;
    buf->last_lineno += 1;
}

int main() {
    struct buffer buf = {
        .data = malloc(0x200),
        .size = 0x200,
        .used = 0,
        .last_lineno = 1,
    };

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

            int fd = open(
                line,
                O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

            assert(fd >= 0);

            ssize_t retval = write(fd, buf.data, buf.used);
            assert(retval >= 0);
            assert(retval == buf.used);

            goto next_iteration;
        }
        if (*line == 'q') {
            ++line;

            assert(strlen(line) == 0);

            free(raw_line);
            break;
        }
        if (*line == 'a') {
            ++line;

            assert(strlen(line) == 0);

            char *new_line = readline("");

            assert(strlen(new_line) >= 1);
            assert(new_line[strlen(new_line) - 1] == '\n');

            buffer_append_at_offset(&buf, buf.used, new_line, strlen(new_line));

            free(new_line);

            goto next_iteration;
        }

    next_iteration:
        free(raw_line);
    }

    free(buf.data);
}
