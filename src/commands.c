#include "core.h"

// ---------------- Installed Version ----------------
const char* get_installed_version() {
    static char version[128] = {0};  // persistent buffer for version
    const char *home = getenv("HOME");
    if (!home) {
        return COLOR_RED "version unknown" COLOR_RESET;
    }

    char path[PATH_MAX];
    int ret = snprintf(path, sizeof(path), "%s/.dynaserve_version", home);
    if (ret < 0 || ret >= (int)sizeof(path)) {
        return COLOR_RED "version path too long" COLOR_RESET;
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        return COLOR_RED "version unknown" COLOR_RESET;
    }

    // Read the version line safely
    if (!fgets(version, sizeof(version), fp)) {
        fclose(fp);
        return COLOR_RED "version unknown" COLOR_RESET;
    }
    fclose(fp);

    // Remove trailing newline or carriage return
    version[strcspn(version, "\r\n")] = '\0';

    // If empty, treat as unknown
    if (strlen(version) == 0) {
        return COLOR_RED "version unknown" COLOR_RESET;
    }

    return version;
}
// ---------------- Commands ----------------
void print_help() {
    printf(COLOR_BLUE "Dynaserve CLI Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN " help" COLOR_RESET "           Shows this help message.\n");
    printf(COLOR_GREEN " --version" COLOR_RESET "      Show the installed version of CLI.\n");
    printf(COLOR_GREEN " --update" COLOR_RESET "       Updates the CLI to the latest Version.\n");
    printf(COLOR_GREEN " --connect" COLOR_RESET "      Connects CLI to users account. Enabling account access.\n");
    printf(COLOR_GREEN " --disconnect" COLOR_RESET "   Disconnects users account from CLI, removing account access.\n");
}


void show_version() {
    printf(COLOR_GREEN "Dynaserve CLI %s\n" COLOR_RESET, get_installed_version());
}