/*
** EPITECH PROJECT, 2025
** COS
** File description:
** cd_command file
*/

#include "wrapper.h"
#include "../../include/kernel/lib/string.h"
#include "../include/ext4_functions.h"
#include "../include/env.h"

// cd command
void cos_cd_command(int32_t argc, char argv[32][20])
{
    static char old_pwd[256] = "";
    char* working_directory = get_env_path();
    char target_path[256] = {0};
    char normalized_path[256] = {0};
    
    if (argc < 2) {
        cos_strcpy(old_pwd, working_directory);
        cos_strcpy(target_path, "/");
    } else if (cos_strcmp(argv[1], "-") == 0) {
        if (cos_strlen(old_pwd) == 0) {
            cos_printf("cd: OLDPWD not set\n");
            return;
        }
        
        cos_strcpy(target_path, old_pwd);
        cos_strcpy(old_pwd, working_directory);
        
    } else {
        cos_strcpy(old_pwd, working_directory);
        cos_strcpy(target_path, argv[1]);
    }
    
    normalize_path(target_path, normalized_path);
    
    if (!is_directory_accessible(normalized_path)) {
        cos_printf("cd: cannot access '%s': No such file or directory\n", target_path);
        return;
    }
    
    cos_strcpy(working_directory, normalized_path);
}
