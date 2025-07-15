#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Struct to hold response content in memory
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Struct to represent an HTTP header (key-value pair)
typedef struct {
    const char *key;
    const char *value;
} Header;

// Write callback to store received data in memory
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + totalSize + 1);
    if (!ptr) {
        fprintf(stderr, "Error: Not enough memory to store response.\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, totalSize);
    mem->size += totalSize;
    mem->memory[mem->size] = '\0';

    return totalSize;
}

// Enhanced HTTP GET function with headers and detailed error checks
char *http_get(const char *url, const Header *headers, size_t header_count) {
    if (url == NULL) {
        fprintf(stderr, "Error: URL is NULL.\n");
        return NULL;
    }

    CURL *curl = NULL;
    CURLcode res;
    struct curl_slist *curl_headers = NULL;
    struct MemoryStruct chunk = {0};

    // Allocate memory for the response buffer
    chunk.memory = malloc(1);
    if (!chunk.memory) {
        fprintf(stderr, "Error: Failed to allocate initial memory for response.\n");
        return NULL;
    }
    chunk.size = 0;

    // Initialize global libcurl environment
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: curl_global_init() failed: %s\n", curl_easy_strerror(res));
        free(chunk.memory);
        return NULL;
    }

    // Create curl handle
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: curl_easy_init() failed — unable to create a CURL handle.\n");
        free(chunk.memory);
        curl_global_cleanup();
        return NULL;
    }

    // Construct header list if provided
    for (size_t i = 0; i < header_count; ++i) {
        if (!headers[i].key || !headers[i].value) {
            fprintf(stderr, "Warning: Skipping null header key or value.\n");
            continue;
        }

        char header_line[10024];
        if (snprintf(header_line, sizeof(header_line), "%s: %s", headers[i].key, headers[i].value) >= (int)sizeof(header_line)) {
            fprintf(stderr, "Warning: Header too long, skipping: %s\n", headers[i].key);
            continue;
        }

        curl_headers = curl_slist_append(curl_headers, header_line);
        if (!curl_headers) {
            fprintf(stderr, "Error: Failed to append header: %s\n", header_line);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            free(chunk.memory);
            return NULL;
        }
    }

    // Set URL
    res = curl_easy_setopt(curl, CURLOPT_URL, url);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: Failed to set CURLOPT_URL: %s\n", curl_easy_strerror(res));
        goto fail;
    }

    // Set response write callback
    res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: Failed to set CURLOPT_WRITEFUNCTION: %s\n", curl_easy_strerror(res));
        goto fail;
    }

    // Set response data destination
    res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: Failed to set CURLOPT_WRITEDATA: %s\n", curl_easy_strerror(res));
        goto fail;
    }

    // Set User-Agent
    res = curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: Failed to set CURLOPT_USERAGENT: %s\n", curl_easy_strerror(res));
        goto fail;
    }

    // Set headers if any
    if (curl_headers) {
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
        if (res != CURLE_OK) {
            fprintf(stderr, "Error: Failed to set CURLOPT_HTTPHEADER: %s\n", curl_easy_strerror(res));
            goto fail;
        }
    }

    // Perform the HTTP GET request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: HTTP request failed: %s\n", curl_easy_strerror(res));
        free(chunk.memory);
        chunk.memory = NULL;
    }

    // Cleanup
fail:
    if (curl_headers)
        curl_slist_free_all(curl_headers);
    if (curl)
        curl_easy_cleanup(curl);
    curl_global_cleanup();

    return chunk.memory;
}

