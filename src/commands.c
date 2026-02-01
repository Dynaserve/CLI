#include "core.h"

// Fallback if VERSION not defined at compile time
#ifndef VERSION
#define VERSION "unknown"
#endif

// ---------------- Installed Version ----------------
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

// ---------------- Platform Detection ----------------
const char* get_platform_string() {
#if defined(__APPLE__) && defined(__arm64__)
    return "darwin-arm64";
#elif defined(__APPLE__) && defined(__x86_64__)
    return "darwin-x86_64";
#elif defined(__linux__) && defined(__aarch64__)
    return "linux-aarch64";
#elif defined(__linux__) && defined(__x86_64__)
    return "linux-x86_64";
#else
    return "unknown";
#endif
}

// ---------------- Version Comparison ----------------
// Returns: -1 if v1 < v2, 0 if equal, 1 if v1 > v2
int compare_versions(const char *v1, const char *v2) {
    int major1 = 0, minor1 = 0, patch1 = 0;
    int major2 = 0, minor2 = 0, patch2 = 0;
    
    sscanf(v1, "%d.%d.%d", &major1, &minor1, &patch1);
    sscanf(v2, "%d.%d.%d", &major2, &minor2, &patch2);
    
    if (major1 != major2) return (major1 > major2) ? 1 : -1;
    if (minor1 != minor2) return (minor1 > minor2) ? 1 : -1;
    if (patch1 != patch2) return (patch1 > patch2) ? 1 : -1;
    
    return 0;
}

// ---------------- Fetch Latest Version from API ----------------
int fetch_latest_version(VersionInfo *info) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "curl -s --connect-timeout 3 %s 2>/dev/null", API_URL);
    
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
    
    if (total == 0) return 0;
    
    // Parse: "release":{"version":"x.x.x"
    const char *release_block = strstr(buf, "\"release\"");
    if (!release_block) {
        release_block = strstr(buf, "\"Release\"");
        if (!release_block) return 0;
    }
    
    // Get version
    const char *version_key = strstr(release_block, "\"version\":\"");
    if (!version_key) {
        version_key = strstr(release_block, "\"Version\":\"");
        if (!version_key) return 0;
        version_key += strlen("\"Version\":\"");
    } else {
        version_key += strlen("\"version\":\"");
    }
    
    const char *version_end = strchr(version_key, '"');
    if (!version_end) return 0;
    
    size_t len = (size_t)(version_end - version_key);
    if (len >= sizeof(info->version)) len = sizeof(info->version) - 1;
    strncpy(info->version, version_key, len);
    info->version[len] = '\0';
    
    // Get platform-specific URL from "repo" object
    const char *platform = get_platform_string();
    char search_key[128];
    snprintf(search_key, sizeof(search_key), "\"%s\":\"", platform);
    
    const char *repo_block = strstr(release_block, "\"repo\"");
    if (!repo_block) return 0;
    
    const char *url_key = strstr(repo_block, search_key);
    if (!url_key) return 0;
    
    url_key += strlen(search_key);
    const char *url_end = strchr(url_key, '"');
    if (!url_end) return 0;
    
    len = (size_t)(url_end - url_key);
    if (len >= sizeof(info->url)) len = sizeof(info->url) - 1;
    strncpy(info->url, url_key, len);
    info->url[len] = '\0';
    
    return 1;
}

// ---------------- Commands ----------------
void print_help() {
    printf(COLOR_BLUE "Dynaserve CLI Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "  help" COLOR_RESET "           Show this help message\n");
    printf(COLOR_GREEN "  --v, --version" COLOR_RESET "  Show installed CLI version\n");
    printf(COLOR_GREEN "  --update" COLOR_RESET "       Update to latest version\n");
}

void show_version() {
    printf(COLOR_GREEN "Dynaserve CLI %s\n" COLOR_RESET, get_installed_version());
}

void update_cli() {
    const char *installed = get_installed_version();
    VersionInfo info = {0};
    
    printf(COLOR_BLUE "Checking for updates...\n" COLOR_RESET);
    
    if (!fetch_latest_version(&info)) {
        printf(COLOR_RED "Failed to fetch latest version from server.\n" COLOR_RESET);
        return;
    }
    
    printf(COLOR_BLUE "Installed version: " COLOR_RESET "%s\n", installed);
    printf(COLOR_BLUE "Latest version:    " COLOR_RESET "%s\n", info.version);
    
    // Compare versions properly
    int cmp = compare_versions(installed, info.version);
    
    if (cmp == 0) {
        printf(COLOR_GREEN "✓ You are already running the latest version!\n" COLOR_RESET);
        return;
    } else if (cmp > 0) {
        printf(COLOR_YELLOW "⚠ You are running a newer version than what's available.\n" COLOR_RESET);
        printf(COLOR_BLUE "Installed: %s, Latest: %s\n" COLOR_RESET, installed, info.version);
        return;
    }
    
    printf(COLOR_YELLOW "⚠ Update available: %s → %s\n" COLOR_RESET, installed, info.version);
    
    // Get current executable path
    char exe_path[PATH_MAX] = {0};
#if defined(__APPLE__)
    uint32_t size = sizeof(exe_path);
    extern int _NSGetExecutablePath(char *buf, uint32_t *bufsize);
    if (_NSGetExecutablePath(exe_path, &size) != 0) {
        printf(COLOR_RED "Failed to get executable path.\n" COLOR_RESET);
        return;
    }
#elif defined(__linux__)
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        printf(COLOR_RED "Failed to get executable path.\n" COLOR_RESET);
        return;
    }
    exe_path[len] = '\0';
#else
    printf(COLOR_RED "Unsupported platform.\n" COLOR_RESET);
    return;
#endif
    
    printf(COLOR_BLUE "Binary location:   " COLOR_RESET "%s\n", exe_path);
    
    // Check if we can write to the directory (better check than just the file)
    char dir_path[PATH_MAX];
    snprintf(dir_path, sizeof(dir_path), "%s", exe_path);
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) *last_slash = '\0';
    
    if (access(dir_path, W_OK) != 0) {
        printf(COLOR_RED "Cannot write to %s/\n" COLOR_RESET, dir_path);
        printf(COLOR_YELLOW "Please run: sudo dynaserve --update\n" COLOR_RESET);
        return;
    }
    
    // Download new binary
    char download_cmd[1024];
    snprintf(download_cmd, sizeof(download_cmd), 
             "curl -L --fail -o \"%s.new\" \"%s\"", exe_path, info.url);
    
    printf(COLOR_YELLOW "Downloading %s...\n" COLOR_RESET, info.version);
    
    int ret = system(download_cmd);
    if (ret != 0) {
        printf(COLOR_RED "Failed to download update (curl exit code: %d).\n" COLOR_RESET, ret);
        printf(COLOR_YELLOW "Try manually downloading from: %s\n" COLOR_RESET, info.url);
        return;
    }
    
    // Make new binary executable
    char chmod_cmd[1024];
    snprintf(chmod_cmd, sizeof(chmod_cmd), "chmod +x \"%s.new\"", exe_path);
    ret = system(chmod_cmd);
    (void)ret;
    
    // Backup current binary
    char backup_cmd[1024];
    snprintf(backup_cmd, sizeof(backup_cmd), "mv \"%s\" \"%s.backup\"", exe_path, exe_path);
    ret = system(backup_cmd);
    (void)ret;
    
    // Replace with new binary
    char replace_cmd[1024];
    snprintf(replace_cmd, sizeof(replace_cmd), "mv \"%s.new\" \"%s\"", exe_path, exe_path);
    
    if (system(replace_cmd) != 0) {
        printf(COLOR_RED "Failed to replace binary. Restoring backup...\n" COLOR_RESET);
        snprintf(backup_cmd, sizeof(backup_cmd), "mv \"%s.backup\" \"%s\"", exe_path, exe_path);
        ret = system(backup_cmd);
        (void)ret;
        return;
    }
    
    // Clean up backup
    snprintf(backup_cmd, sizeof(backup_cmd), "rm -f \"%s.backup\"", exe_path);
    ret = system(backup_cmd);
    (void)ret;
    
    printf(COLOR_GREEN "✓ Successfully updated to v%s!\n" COLOR_RESET, info.version);
    printf(COLOR_BLUE "Run 'dynaserve --version' to verify.\n" COLOR_RESET);
}