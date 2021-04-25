#include <stdio.h>
#include <readline/readline.h>
#include <fcntl.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct buffer {
    char *data;
    size_t size;
    size_t used;
    size_t last_lineno;
};

static void buffer_append_at_offset(struct buffer *buf, size_t offset, const char *data, size_t data_size)
{
    assert(buf->size >= buf->used + data_size);

    memmove(buf->data + offset + data_size, buf->data + offset, data_size);
    memcpy(buf->data + offset, data, data_size);

    buf->used += data_size;
    buf->last_lineno += 1;
}

static int parse_integer(char *buffer, int *value_out, size_t *end_out)
{
    size_t offset = 0;
    int value = 0;

    while (isdigit(buffer[offset])) {
        value = value * 10 + (buffer[offset] - '0');
        ++offset;
    }

    if (offset == 0)
        return -1;

    if (value_out)
        *value_out = value;
    if (end_out)
        *end_out = offset;

    return 0;
}

static int buffer_get_line_offset(struct buffer *buf, size_t lineno, size_t *offset_out)
{
    assert(lineno >= 1);
    --lineno;

    printf("Looking for line (index) %zu\n", lineno);

    size_t offset = 0;
    while (lineno > 0) {
        if (offset >= buf->used)
            return -1;

        if (buf->data[offset] == '\n') {
            printf("Found a line-feed lineno=%zu\n", lineno);

            --lineno;
            ++offset;
        } else {
            ++offset;
        }
    }

    if (offset_out)
        *offset_out = offset;

    return 0;
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
