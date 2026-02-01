#include "core.h"

#ifndef VERSION
#define VERSION "unknown"
#endif

const char* get_installed_version() {
    static char version[128] = {0};
    
    if (version[0] == '\0') {
        snprintf(version, sizeof(version), "%s", VERSION);
    }
    
    if (strcmp(version, "unknown") == 0) {
        return COLOR_RED "version unknown" COLOR_RESET;
    }
    
    return version;
}

void show_version() {
    const char *installed = get_installed_version();
    printf(COLOR_GREEN "Dynaserve CLI %s\n" COLOR_RESET, installed);
    
    // Quick update check
    VersionInfo info = {0};
    if (fetch_latest_version(&info)) {
        int cmp = compare_versions(installed, info.version);
        if (cmp < 0) {
            printf(COLOR_YELLOW "\n⚠ Update available: %s → %s\n" COLOR_RESET, installed, info.version);
            printf(COLOR_YELLOW "Run " COLOR_GREEN "sudo dynaserve --update" COLOR_YELLOW " to upgrade\n" COLOR_RESET);
        }
    }
}