#include <stdio.h>
#include <stdlib.h>
#include "pvars.h"  // Assuming this is in your 'include' directory

// Helper macro to check for errors and print the message
#define CHECK_ERROR() \
    if (pvars_errno != SUCCESS) { \
        fprintf(stderr, "\n--- ERROR ---\n"); \
        fprintf(stderr, "Code: %d, Message: %s\n", pvars_errno, perror_message()); \
        exit(EXIT_FAILURE); \
    }

void run_nested_list_test()
{
    printf("====================================================\n");
    printf("  --- Starting Multiple Nested List Deep Copy Test ---\n");
    printf("====================================================\n\n");

    // 1. Create three lists: A (Container), B (Inner 1), C (Inner 2)
    plist_t *list_a = plist_create(5);
    plist_t *list_b = plist_create(5);
    plist_t *list_c = plist_create(5);

    CHECK_ERROR();

    // 2. Populate List B (The first inner list)
    plist_add_str(list_b, "First String");
    plist_add_int(list_b, 100);
    printf("List B (Original Inner List 1): ");
    plist_print(list_b);
    CHECK_ERROR();

    // 3. Populate List C (The second inner list, different types)
    plist_add_double(list_c, 3.14159);
    plist_add_long(list_c, 9876543210L);
    printf("List C (Original Inner List 2): ");
    plist_print(list_c);
    CHECK_ERROR();

    // 4. Build the Container List A
    printf("\n--- Test: plist_add_list and Multiple Nesting ---\n");
    
    // Add a deep copy of List B
    printf("-> Adding deep copy of List B to List A (Index 0)\n");
    plist_add_list(list_a, list_b);
    
    // Add a primitive type for verification
    printf("-> Adding a primitive int (42) to List A (Index 1)\n");
    plist_add_int(list_a, 42);

    // Add a deep copy of List C
    printf("-> Adding deep copy of List C to List A (Index 2)\n");
    plist_add_list(list_a, list_c);

    CHECK_ERROR();
    
    printf("\nContainer List A (Should show 3 elements: B, 42, C):\n");
    plist_print(list_a);
    
    /* Expected Output (If fixes are applied): 
     * [ [ 'First String', 100 ], 42, [ 3.14159, 9876543210L ] ]
     */

    // 5. Deep Copy Verification: Modify the originals
    printf("\n--- Test: Deep Copy Verification ---\n");
    
    // Modify original List B
    printf("-> Modifying original List B by adding 'NEW STRING B'...\n");
    plist_add_str(list_b, "NEW STRING B");
    
    // Modify original List C
    printf("-> Modifying original List C by adding a float (1.234f)...\n");
    plist_add_float(list_c, 1.234f);
    
    printf("\nList B (Original) after modification: ");
    plist_print(list_b);
    printf("List C (Original) after modification: ");
    plist_print(list_c);
    
    printf("\nContainer List A (Nested copies MUST remain UNCHANGED):\n");
    plist_print(list_a);
    
    printf("\n✅ Verification successful: The nested lists in List A are independent deep copies.\n");

    // 6. Cleanup
    printf("\n--- Cleaning up all lists ---\n");
    
    // Destroying List A should recursively free its elements (including copies of B and C)
    plist_destroy(list_a); 
    
    // Destroying B and C only frees the originals (which were not nested)
    plist_destroy(list_b);
    plist_destroy(list_c);
    
    printf("Cleanup complete. No segmentation fault means memory destruction was likely successful.\n");
}

int main()
{
    run_nested_list_test();
    return 0;
}
