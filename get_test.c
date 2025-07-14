#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
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

    if (!response) {
          fprintf(stderr, "Request failed or no response.\n");
          return 1;
      }

	  // Parse the response using cJSON
	  cJSON *json = cJSON_Parse(response);
	  if (!json) {
		  fprintf(stderr, "Error: Failed to parse JSON response.\n");
		  free(response);
		  return 1;
	  }

	  // Find the "number" field
	  cJSON *number_item = cJSON_GetObjectItemCaseSensitive(json, "number");
	  if (cJSON_IsNumber(number_item)) {
		  printf("Value of 'number': %lf\n", number_item->valuedouble);
	  } else {
		  fprintf(stderr, "Error: 'number' key not found or is not a number.\n");
	  }

	  // Cleanup
	  cJSON_Delete(json);
	  free(response);

    return 0;
}

