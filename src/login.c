#include "core.h"

void login_cli() {
    DeviceCodeResponse device = {0};
    
    printf(COLOR_BLUE "Dynaserve CLI Login\n" COLOR_RESET);
    printf("\n");
    
    // Step 1: Request device code
    printf(COLOR_BLUE "Requesting device code...\n" COLOR_RESET);
    
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), 
             "curl -sL --fail -X POST '%s/device/code' "
             "-H 'Content-Type: application/json' "
             "-d '{\"client_id\":\"dynaserve-cli\"}'",
             AUTH_URL);
    
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        printf(COLOR_RED "Failed to connect to authentication server\n" COLOR_RESET);
        return;
    }
    
    char response[2048] = {0};
    size_t total = 0;
    size_t n;
    while ((n = fread(response + total, 1, sizeof(response) - total - 1, fp)) > 0) {
        total += n;
        if (total >= sizeof(response) - 1) break;
    }
    pclose(fp);
    response[total] = '\0';
    
    // Parse device code response
    const char *device_code = strstr(response, "\"device_code\":\"");
    const char *user_code = strstr(response, "\"user_code\":\"");
    const char *verification_uri = strstr(response, "\"verification_uri\":\"");
    const char *expires_in = strstr(response, "\"expires_in\":");
    const char *interval = strstr(response, "\"interval\":");
    
    if (!device_code || !user_code || !verification_uri) {
        printf(COLOR_RED "Failed to get device code\n" COLOR_RESET);
        return;
    }
    
    // Extract device_code
    device_code += strlen("\"device_code\":\"");
    const char *dc_end = strchr(device_code, '"');
    size_t dc_len = (size_t)(dc_end - device_code);
    strncpy(device.device_code, device_code, dc_len < sizeof(device.device_code) ? dc_len : sizeof(device.device_code) - 1);
    
    // Extract user_code
    user_code += strlen("\"user_code\":\"");
    const char *uc_end = strchr(user_code, '"');
    size_t uc_len = (size_t)(uc_end - user_code);
    strncpy(device.user_code, user_code, uc_len < sizeof(device.user_code) ? uc_len : sizeof(device.user_code) - 1);
    
    // Extract verification_uri
    verification_uri += strlen("\"verification_uri\":\"");
    const char *vu_end = strchr(verification_uri, '"');
    size_t vu_len = (size_t)(vu_end - verification_uri);
    strncpy(device.verification_uri, verification_uri, vu_len < sizeof(device.verification_uri) ? vu_len : sizeof(device.verification_uri) - 1);
    
    // Extract expires_in and interval
    if (expires_in) {
        sscanf(expires_in + strlen("\"expires_in\":"), "%d", &device.expires_in);
    } else {
        device.expires_in = 900; // Default 15 minutes
    }
    
    if (interval) {
        sscanf(interval + strlen("\"interval\":"), "%d", &device.interval);
    } else {
        device.interval = 5; // Default 5 seconds
    }
    
    // Step 2: Display code to user
    printf("\n");
    printf(COLOR_BOLD COLOR_YELLOW "! " COLOR_RESET COLOR_BOLD "First copy your one-time code: " COLOR_CYAN "%s\n" COLOR_RESET, device.user_code);
    printf(COLOR_BOLD "! " COLOR_RESET "Press " COLOR_GREEN "Enter" COLOR_RESET " to open " COLOR_CYAN "%s" COLOR_RESET " in your browser...", device.verification_uri);
    fflush(stdout);
    
    // Wait for user to press Enter
    char enter[2];
    if (fgets(enter, sizeof(enter), stdin)) {
        // Open browser
        char open_cmd[512];
#if defined(__APPLE__)
        snprintf(open_cmd, sizeof(open_cmd), "open '%s'", device.verification_uri);
#elif defined(__linux__)
        snprintf(open_cmd, sizeof(open_cmd), "xdg-open '%s' 2>/dev/null || sensible-browser '%s' 2>/dev/null || echo 'Please open %s in your browser'", 
                 device.verification_uri, device.verification_uri, device.verification_uri);
#else
        snprintf(open_cmd, sizeof(open_cmd), "echo 'Please open %s in your browser'", device.verification_uri);
#endif
        int ret = system(open_cmd);
        (void)ret;
    }
    
    printf("\n");
    printf(COLOR_BLUE "Waiting for authentication...\n" COLOR_RESET);
    
    // Step 3: Poll for token
    time_t start_time = time(NULL);
    int attempts = 0;
    
    while (time(NULL) - start_time < device.expires_in) {
        attempts++;
        
        // Wait for interval before polling
        sleep(device.interval);
        
        // Poll token endpoint
        snprintf(cmd, sizeof(cmd),
                 "curl -sL -X POST '%s/device/token' "
                 "-H 'Content-Type: application/json' "
                 "-d '{\"client_id\":\"dynaserve-cli\",\"device_code\":\"%s\"}'",
                 AUTH_URL, device.device_code);
        
        fp = popen(cmd, "r");
        if (!fp) continue;
        
        total = 0;
        while ((n = fread(response + total, 1, sizeof(response) - total - 1, fp)) > 0) {
            total += n;
            if (total >= sizeof(response) - 1) break;
        }
        pclose(fp);
        response[total] = '\0';
        
        // Check for token
        const char *token = strstr(response, "\"access_token\":\"");
        if (token) {
            token += strlen("\"access_token\":\"");
            const char *token_end = strchr(token, '"');
            if (!token_end) continue;
            
            // Extract token
            char access_token[1024] = {0};
            size_t token_len = (size_t)(token_end - token);
            if (token_len >= sizeof(access_token)) token_len = sizeof(access_token) - 1;
            strncpy(access_token, token, token_len);
            
            // Save token
            const char *home = getenv("HOME");
            if (!home) {
                printf(COLOR_RED "Failed to get home directory\n" COLOR_RESET);
                return;
            }
            
            char token_path[PATH_MAX];
            snprintf(token_path, sizeof(token_path), "%s/.dynaserve_token", home);
            
            FILE *token_file = fopen(token_path, "w");
            if (!token_file) {
                printf(COLOR_RED "Failed to save authentication token\n" COLOR_RESET);
                return;
            }
            
            fprintf(token_file, "%s\n", access_token);
            fclose(token_file);
            chmod(token_path, 0600);
            
            printf("\n");
            printf(COLOR_GREEN "✓ Authentication complete!\n" COLOR_RESET);
            printf(COLOR_BLUE "Logged in as your Dynaserve account\n" COLOR_RESET);
            return;
        }
        
        // Check for pending authorization
        const char *error = strstr(response, "\"error\":\"");
        if (error) {
            error += strlen("\"error\":\"");
            const char *error_end = strchr(error, '"');
            if (error_end) {
                char error_msg[128] = {0};
                size_t err_len = (size_t)(error_end - error);
                if (err_len >= sizeof(error_msg)) err_len = sizeof(error_msg) - 1;
                strncpy(error_msg, error, err_len);
                
                if (strcmp(error_msg, "authorization_pending") != 0) {
                    printf(COLOR_RED "Authentication failed: %s\n" COLOR_RESET, error_msg);
                    return;
                }
            }
        }
        
        // Show progress every 10 attempts (50 seconds)
        if (attempts % 10 == 0) {
            printf(COLOR_YELLOW "Still waiting for authentication...\n" COLOR_RESET);
        }
    }
    
    printf("\n");
    printf(COLOR_RED "✗ Authentication timed out\n" COLOR_RESET);
    printf(COLOR_YELLOW "Please try again: dynaserve --login\n" COLOR_RESET);
}