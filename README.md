This library aims to simplify data storage in C, similar to Python lists, tuples, etc

Example use:

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pvars.h" // Library header
#include "perrno.h" // Error code header

int main()
{
    // 1. Create the dynamic list (starting with capacity 3)

  plist_t *my_list = plist_create(3);

  if (!my_list) {
    fprintf(stderr, "ERROR: List creation failed: %s\n", perror_message());
    return 1;
  }

  // 2. Add different data types (Python-style list appending)
  plist_add_str(my_list, "Hello, C!");
  plist_add_int(my_list, 42);
  plist_add_double(my_list, 3.14159);
  plist_add_long(my_list, 9876543210L);

  // Check list size
  printf("List size after adding elements: %zu\n", plist_get_size(my_list));

  // Optional: Print the entire list with plist_print
  // printf("List contents:\n");
  // plist_print(my_list);

  // 3. Retrieve values safely
  printf("\n--- Retrieving Values ---\n");
  char *retrieved_str = NULL;
  int retrieved_int = 0;
  double retrieved_double = 0.0;

  // Get string (Index 0)
  if (plist_get_str(my_list, 0, &retrieved_str) && retrieved_str != NULL) {
    printf("Index 0 (String): %s\n", retrieved_str);
  }

  // Get integer (Index 1)
  if (plist_get_int(my_list, 1, &retrieved_int)) {
    printf("Index 1 (Integer): %d\n", retrieved_int);
  }

  // Demonstrate type-safety failure: Trying to get a string as a double
  if (plist_get_double(my_list, 0, &retrieved_double)) {
    printf("Index 0 (Double): %.2f (Should NOT print this)\n", retrieved_double);
  } else {
  // This is the expected behavior, showing type safety in action!

  printf("Index 0 (Double): FAILED (Correctly detected wrong type: %s)\n", perror_message());
  }

  // 4. Cleanup and Free Memory
  plist_destroy(my_list);
  printf("\nList destroyed successfully. Program finished.\n");

  return 0;

}
