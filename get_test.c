#include <stdio.h>
#include <stdlib.h>
#include "http_get.h"

// Struct to represent an HTTP header (key-value pair)
typedef struct {
    const char *key;
    const char *value;
} Header;

extern char *http_get(const char *url, const Header *headers, size_t header_count);

int main(void) {
    
    const char *url = URL;

    Header headers[] = {
        {"Accept", "application/json"},
        {"Content-Type", "application/json"},
        {"authorization", APIKEY }
    };

    char *response = http_get(url, headers, sizeof(headers)/sizeof(headers[0]));

    if (response) {
        printf("Response:\n%s\n", response);
        free(response);
    } else {
        fprintf(stderr, "Request failed.\n");
    }

    return 0;
}

