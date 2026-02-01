#include "core.h"

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