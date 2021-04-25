#include <stdio.h>
#include <readline/readline.h>
#include <fcntl.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

struct buffer {
    char *data;
    size_t size;
    size_t used;
};

static void buffer_append_at_offset(struct buffer *buf, size_t offset, const char *data, size_t data_size)
{
    assert(buf->size >= buf->used + data_size);

    memmove(buf->data + offset + data_size, buf->data + offset, data_size);
    memcpy(buf->data + offset, data, data_size);

    buf->used += data_size;
}

static int parse_integer(char *buffer, int *value_out, char **end_out)
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
        *end_out = buffer + offset;

    return 0;
}

static int buffer_get_line_offset(struct buffer *buf, size_t lineno, size_t *offset_out)
{
    assert(lineno >= 1);
    --lineno;

    size_t offset = 0;
    while (lineno > 0) {
        if (offset >= buf->used)
            return -1;

        if (buf->data[offset] == '\n') {
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

static void buffer_delete_range(struct buffer *buf, size_t from_line, size_t to_line)
{
    assert(from_line <= to_line);

    int retval;

    size_t from_offset;
    retval = buffer_get_line_offset(buf, from_line, &from_offset);
    assert(retval == 0);

    size_t to_offset;
    retval = buffer_get_line_offset(buf, to_line + 1, &to_offset);
    assert(retval == 0);

    assert(to_offset - from_offset <= buf->used);

    memmove(buf->data + from_offset, buf->data + to_offset, to_offset - from_offset);
    buf->used -= to_offset - from_offset;
}

int main() {
    struct buffer buf = {
        .data = malloc(0x200),
        .size = 0x200,
        .used = 0,
    };

    for (;;) {
        char *raw_line = readline("% ");
        assert(raw_line != NULL);

        if (strchr(raw_line, '\n'))
            *strchr(raw_line, '\n') = 0;

        char *line = raw_line;

        int selection_start = -1;
        int selection_end = -1;

        if (parse_integer(line, &selection_start, &line) == 0) {
            if (*line == ',') {
                ++line;
                parse_integer(line, &selection_end, &line);
            }
        }

        if (*line == 'w') {
            ++line;

            assert(selection_start == -1);
            assert(selection_end == -1);

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
        } else if (*line == 'q') {
            ++line;

            assert(selection_start == -1);
            assert(selection_end == -1);
            assert(strlen(line) == 0);

            free(raw_line);
            break;
        } else if (*line == 'a') {
            ++line;

            assert(strlen(line) == 0);

            size_t offset = buf.used;

            if (selection_start != -1) {
                int retval = buffer_get_line_offset(&buf, (size_t)selection_start + 1, &offset);
                assert(retval == 0);

                if (selection_end != -1)
                    buffer_delete_range(&buf, selection_start, selection_end);
            }

            for(;;) {
                char *new_line = readline("");

                assert(strlen(new_line) >= 1);
                assert(new_line[strlen(new_line) - 1] == '\n');

                if (strcmp(new_line, ".\n") == 0) {
                    free(new_line);
                    break;
                }

                buffer_append_at_offset(&buf, offset, new_line, strlen(new_line));
                offset += strlen(new_line);

                free(new_line);
            }

            goto next_iteration;
        } else if (*line == 'p') {
            ssize_t retval;

            size_t start_offset = 0;
            size_t end_offset = buf.used;

            if (selection_start != -1) {
                retval = buffer_get_line_offset(&buf, selection_start, &start_offset);
                assert(retval == 0);

                retval = buffer_get_line_offset(&buf, selection_start + 1, &end_offset);
                assert(retval == 0);
            }
            if (selection_end != -1) {
                retval = buffer_get_line_offset(&buf, selection_end + 1, &end_offset);
                assert(retval == 0);
            }

            assert(end_offset >= start_offset);

            retval = write(STDOUT_FILENO, buf.data + start_offset, end_offset - start_offset);
            assert(retval == end_offset - start_offset);
        } else {
            printf("ed: Unknown command\n");
        }

    next_iteration:
        free(raw_line);
    }

    free(buf.data);
}
