#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

int main(int argc, char **argv, char **envp)
{
    printf("This output is created by '/bin/Example.elf'\n");

    char *pwd = get_current_dir_name();
    printf("We are currently executing in '%s'\n", pwd);
    free(pwd);

    printf("Got argc=%i argv=%p envp=%p\n", argc, argv, envp);

    printf("Arguments:\n");
    while (*argv != NULL)
        printf("  '%s'\n", *argv++);

    printf("Environment:\n");
    while (*envp != NULL)
        printf("  '%s'\n", *envp++);
}
