#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

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

const char* get_installed_version();
const char* get_platform_string();
int fetch_latest_version(VersionInfo *info);

void print_help();
void show_version();
void update_cli();

#endif /* CORE_H */