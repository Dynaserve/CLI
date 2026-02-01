#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>

#define COLOR_RESET  "\033[0m"
#define COLOR_RED    "\033[31m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE   "\033[34m"

#define API_URL "https://cli.dynaserve.io/version"

typedef struct {
    char version[64];
    char url[512];
} VersionInfo;

// help.c
void print_help();

// version.c
void show_version();
const char* get_installed_version();

// update.c
void update_cli();
int fetch_latest_version(VersionInfo *info);
int compare_versions(const char *v1, const char *v2);

// utils.c
const char* get_platform_string();

#endif /* CORE_H */