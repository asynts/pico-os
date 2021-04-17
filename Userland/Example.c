#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

int main() {
    printf("This output is created by '/bin/Example.elf'\n");

    char *pwd = get_current_dir_name();
    printf("We are currently executing in '%s'\n", pwd);
    free(pwd);
}
