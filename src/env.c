/*
** EPITECH PROJECT, 2025
** COS
** File description:
** env
*/

#include "env.h"
#include "../../include/kernel/lib/string.h"

static char previous_working_directory[256] = "/";

char* get_env_path()
{
    static char current_working_directory[256] = "/";
    return current_working_directory;
}

char* get_previous_env_path()
{
    return previous_working_directory;
}

void save_current_as_previous()
{
    char* current = get_env_path();
    cos_strcpy(previous_working_directory, current);
}