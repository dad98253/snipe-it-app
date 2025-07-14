#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    cJSON *fields;
    char * hostname = NULL;
    char * pchar = NULL;

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
//      printf("Response:\n%s\n\n", response);

	  // Parse the response using cJSON
	  cJSON *json = cJSON_Parse(response);
	  if (!json) {
		  fprintf(stderr, "Error: Failed to parse JSON response.\n");
		  free(response);
		  return 2;
	  }

	  // Find the "id" field
	  cJSON *number_item = cJSON_GetObjectItemCaseSensitive(json, "id");
	  if (cJSON_IsNumber(number_item)) {
		  printf("Value of 'id': %i\n", number_item->valueint);
	  } else {
		  fprintf(stderr, "Error: 'id' key not found or is not a number.\n");
	  }

	  // Find the "Asset Tag" field
	  number_item = cJSON_GetObjectItemCaseSensitive(json, "asset_tag");
	  if (cJSON_IsNumber(number_item)) {
		  printf("Value of 'Asset Tag': %lf\n", number_item->valuedouble);
	  } else {
		if (cJSON_IsString(number_item) && (number_item->valuestring != NULL)) {
		    printf("Name: %s\n", number_item->valuestring);
                    hostname = (char *)malloc(strlen(number_item->valuestring)+1);
                    strcpy(hostname,number_item->valuestring);
                    pchar = strchr(hostname, '.');
                    if (pchar != NULL) {
                        *pchar = '\000';		// terminate the fqhn at the first dot
                    } else {
                        printf("Character '%c' not found in the string.\n",'.');
                    }
                    printf("hostname: %s\n",hostname);
		} else {
		  fprintf(stderr, "Error: 'Asset Tag' key not found or is not a number or a string.\n");
		}
	  }

         // Find the "Custom Fields" field
//          number_item = cJSON_GetObjectItemCaseSensitive(json, "custom_fields");
         cJSON *address = cJSON_GetObjectItem(json, "custom_fields");
         if (address == NULL) {
           // Handle missing object
              printf("Custom Fields Object not found\n");
              cJSON_Delete(json);
              return 3;
         } else {
//              printf("found Custom Fields Object\n");
               if (cJSON_IsString(address) && (address->valuestring != NULL)) {
                    printf("Custom Fields: %s\n", address->valuestring);
                } else {
//                    printf("Custom Fields is not a string\n");
                    if (cJSON_IsObject(address)) {
//                            printf("Custom Fields is an object\n");
                            number_item = cJSON_GetObjectItem(address, "IP Address");
                            if (cJSON_IsString(number_item) && (number_item->valuestring != NULL)) {
                                 printf("IP Address: %s\n", number_item->valuestring);
                            } else {
//                                 fprintf(stderr, "Error: 'IP Address' key not found or is not a string.\n");
                                 if (cJSON_IsObject(number_item)) {
//                                     printf("IP Address is an object\n");
                                     fields = cJSON_GetObjectItem(number_item, "field");
                                     if (cJSON_IsString(fields) && (fields->valuestring != NULL)) {
//                                         printf("fields: %s\n", fields->valuestring);
                                     }
                                     if ( !strcmp ( "_snipeit_ip_address_2", fields->valuestring ) ) {
//                                        printf("found IP address\n");
                                     } else {
                                        printf("unexpectedly found %s\n",fields->valuestring);
                                     }
                                     fields = cJSON_GetObjectItem(number_item, "value");
                                     if (cJSON_IsString(fields) && (fields->valuestring != NULL)) {
                                         printf("IP Address: %s\n", fields->valuestring);
                                     }
                                 }
                            }
//"MAC Address"
                            number_item = cJSON_GetObjectItem(address, "MAC Address");
                            if (cJSON_IsString(number_item) && (number_item->valuestring != NULL)) {
                                 printf("MAC Address: %s\n", number_item->valuestring);
                            } else {
//                                 fprintf(stderr, "Error: 'MAC Address' key not found or is not a string.\n");
                                 if (cJSON_IsObject(number_item)) {
//                                     printf("MAC Address is an object\n");
                                     fields = cJSON_GetObjectItem(number_item, "field");
                                     if (cJSON_IsString(fields) && (fields->valuestring != NULL)) {
//                                         printf("fields: %s\n", fields->valuestring);
                                     }
                                     if ( !strcmp ( "_snipeit_mac_address_1", fields->valuestring ) ) {
//                                        printf("found MAC address\n");
                                     } else {
                                        printf("unexpectedly found %s\n",fields->valuestring);
                                     }
                                     fields = cJSON_GetObjectItem(number_item, "value");
                                     if (cJSON_IsString(fields) && (fields->valuestring != NULL)) {
                                         printf("MAC Address: %s\n", fields->valuestring);
                                     }
                                 }
                            }

                    } else {
                        printf("address is not an object\n");
                    }
                }
         }
//          if (cJSON_IsString(number_item) && (number_item->valuestring != NULL)) {
//              printf("Custom Fields: %s\n", number_item->valuestring);
//          } else {
//              fprintf(stderr, "Error: 'Custom Fields' key not found or is not a string.\n");
//          }




	  // Cleanup
	  cJSON_Delete(json);
//	  cJSON_Delete(fields);
//	  cJSON_Delete(number_item);
	  free(response);
	  free(hostname);

    return 0;
}

