#include "core.h"

// ---------------- Installed Version ----------------
const char* get_installed_version() {
    static char version[64] = {0};
    const char *home = getenv("HOME");
    static char path[PATH_MAX];
    if (!home) return "unknown";
    snprintf(path, sizeof(path), "%s/.dynaserve_version", home);
    FILE *fp = fopen(path, "r");
    if (!fp) return "unknown";
    if (!fgets(version, sizeof(version), fp)) {
        fclose(fp);
        return "unknown";
    }
    fclose(fp);
    version[strcspn(version, "\n")] = 0;
    return version;
}

// ---------------- Commands ----------------
void print_help() {
    printf(COLOR_BLUE "Dynaserve CLI Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "  help" COLOR_RESET "          Show this help message\n");
    printf(COLOR_GREEN "  --v, --version" COLOR_RESET " Show Dynaserve CLI version\n");
}

void show_version() {
    printf(COLOR_GREEN "Dynaserve CLI %s\n" COLOR_RESET, get_installed_version());
}