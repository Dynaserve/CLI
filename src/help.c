#include "core.h"

void print_help() {
    printf(COLOR_BLUE "Dynaserve CLI Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "  help" COLOR_RESET "           Show this help message\n");
    printf(COLOR_GREEN "  --version" COLOR_RESET "      Show installed CLI version\n");
    printf(COLOR_GREEN "  --update" COLOR_RESET "       Update to latest version\n");
    printf(COLOR_GREEN "  --login" COLOR_RESET "        Login to Dynaserve account\n");
}