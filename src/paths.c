#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wordexp.h>
#include <linux/limits.h>

#include "paths.h"

char* get_xdg_user_dir(const char* dir_name);

void determine_save_path(char* out) {
    char* new_path = malloc(PATH_MAX);
    char* pictures_path = get_xdg_user_dir("PICTURES");

    if (pictures_path) {
        strcpy(new_path, pictures_path);
        free(pictures_path);
    } else {
        pictures_path = getenv("HOME");
        if (!pictures_path) {
            // What are you even running this tool on bruhh :sob::wilted_flower:
            fprintf(stderr, "Can't determine the path to save to. Pass the path to save to via an argument.\n");
            exit(0);
        }
        strcpy(new_path, pictures_path);
    }

    time_t time_epoch = time(nullptr);
    size_t path_len = strlen(new_path);
    strftime(new_path + path_len, sizeof(new_path) - path_len - 1, "/%Y%m%d_%H%M%S_" PROJECT_NAME ".png", localtime(&time_epoch));

    strcpy(out, new_path);
}

char* get_xdg_user_dir(const char* dir_name) {
    char config_path[512];
    char* home = getenv("HOME");

    if (!home) return NULL;

    snprintf(config_path, sizeof(config_path),
             "%s/.config/user-dirs.dirs", home);

    FILE* f = fopen(config_path, "r");
    if (!f) return NULL;

    char line[512];
    char search_key[64];
    snprintf(search_key, sizeof(search_key), "XDG_%s_DIR=", dir_name);

    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, search_key)) {
            char* start = strchr(line, '"');
            if (start) {
                start++;
                char* end = strchr(start, '"');
                if (end) {
                    *end = '\0';
                    // Handle $HOME variable
                    if (strncmp(start, "$HOME/", 6) == 0) {
                        char* result = malloc(strlen(home) + strlen(start) - 5);
                        sprintf(result, "%s/%s", home, start + 6);
                        fclose(f);
                        return result;
                    }
                    fclose(f);
                    return strdup(start);
                }
            }
        }
    }

    fclose(f);
    return NULL;
}
