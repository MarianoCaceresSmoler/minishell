#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

ssize_t read_line(char **lineptr, size_t *n, FILE *stream)
{
    return getline(lineptr, n, stream);
}

ssize_t parse_line(char *line, char **args)
{
    char *token = strtok(line, " \n");
    int i = 0;

    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    return i;
}

void built_in_exit(void)
{
    exit(0);
}

void built_in_cd(char **args)
{
    const char *path;

    if (args[1] == NULL)
    {
        path = getenv("HOME");
        if (!path)
        {
            fprintf(stderr, "cd: HOME not defined\n");
            return;
        }
    }
    else
    {
        path = args[1];
    }

    if (chdir(path) != 0)
    {
        perror("cd");
    }
}

void built_in_pwd(void)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("getcwd");
    }
}

int is_builtin(char *cmd)
{
    return strcmp(cmd, "cd") == 0 || strcmp(cmd, "exit") == 0 || strcmp(cmd, "pwd") == 0;
}

void run_builtin(char **argv)
{
    if (strcmp(argv[0], "cd") == 0)
    {
        built_in_cd(argv);
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
        built_in_exit();
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        built_in_pwd();
    }
}

void exec_external(char **argv)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return;
    }

    if (pid == 0)
    {
        // Child process
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char const *argv[])
{
    char *line = NULL;
    size_t len = 0;

    while (1)
    {
        printf("mini-shell> ");

        // Read a line of input from the user
        if (read_line(&line, &len, stdin) == -1)
        {
            perror("getline failed");
            break;
        }

        // Tokenize the input line and save tokens in array
        char *args[100];
        int args_count = parse_line(line, args);

        if (args_count == 0)
        {
            continue; // Empty command
        }

        if (is_builtin(args[0]))
        {
            // Execute built-in command
            run_builtin(args);
            continue;
        }
        else
        {
            // Execute external command
            exec_external(args);
        }
    }

    free(line);
    return 0;
}
