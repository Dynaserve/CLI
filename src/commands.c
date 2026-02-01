#include "core.h"

// Fallback if VERSION not defined at compile time
#ifndef VERSION
#define VERSION "unknown"
#endif

// ---------------- Installed Version ----------------
const char* get_installed_version() {
    static char version[128] = {0};
    
    // First call: copy compile-time VERSION into static buffer
    if (version[0] == '\0') {
        snprintf(version, sizeof(version), "%s", VERSION);
    }
    
    // If VERSION was "unknown", return with color
    if (strcmp(version, "unknown") == 0) {
        return COLOR_RED "version unknown" COLOR_RESET;
    }
    
    return version;
}

// ---------------- Fetch Latest Version from API ----------------
int fetch_latest_version(char *version_out, size_t size) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "curl -s %s 2>/dev/null", API_URL);
    
    FILE *fp = popen(cmd, "r");
    if (!fp) return 0;
    
    char buf[2048] = {0};
    size_t total = 0;
    size_t n;
    while ((n = fread(buf + total, 1, sizeof(buf) - total - 1, fp)) > 0) {
        total += n;
        if (total >= sizeof(buf) - 1) break;
    }
    pclose(fp);
    buf[total] = '\0';
    
    // Parse "Release":{"Version":"x.x.x"
    const char *release_block = strstr(buf, "\"Release\":{");
    if (!release_block) return 0;
    
    const char *version_key = strstr(release_block, "\"Version\":\"");
    if (!version_key) return 0;
    
    version_key += strlen("\"Version\":\"");
    const char *version_end = strchr(version_key, '"');
    if (!version_end) return 0;
    
    size_t len = (size_t)(version_end - version_key);
    if (len >= size) len = size - 1;
    
    strncpy(version_out, version_key, len);
    version_out[len] = '\0';
    
    return 1;
}

// ---------------- Commands ----------------
void print_help() {
    printf(COLOR_BLUE "Dynaserve CLI Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "  help" COLOR_RESET "           Show this help message\n");
    printf(COLOR_GREEN "  --v, --version" COLOR_RESET "  Show installed CLI version\n");
    printf(COLOR_GREEN "  check" COLOR_RESET "          Check for updates\n");
}

void show_version() {
    printf(COLOR_GREEN "Dynaserve CLI %s\n" COLOR_RESET, get_installed_version());
}

void check_version() {
    const char *installed = get_installed_version();
    char latest[64] = {0};
    
    printf(COLOR_BLUE "Checking for updates...\n" COLOR_RESET);
    
    if (!fetch_latest_version(latest, sizeof(latest))) {
        printf(COLOR_RED "Failed to fetch latest version from server.\n" COLOR_RESET);
        return;
    }
    
    printf(COLOR_BLUE "Installed version: " COLOR_RESET "%s\n", installed);
    printf(COLOR_BLUE "Latest version:    " COLOR_RESET "%s\n", latest);
    
    if (strcmp(installed, latest) == 0) {
        printf(COLOR_GREEN "✓ You are running the latest version!\n" COLOR_RESET);
    } else {
        printf(COLOR_YELLOW "⚠ Update available: %s → %s\n" COLOR_RESET, installed, latest);
        printf(COLOR_YELLOW "Download from: https://github.com/YOUR_REPO/releases/latest\n" COLOR_RESET);
    }
}