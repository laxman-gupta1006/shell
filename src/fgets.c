#include <stdio.h>

int main() {
    char arr[100];
    
    printf("Try typing something, then try Ctrl+D:\n");
    
    // Example 1: Normal input
    printf("Enter text: ");
    fgets(arr, sizeof(arr), stdin);
    printf("You typed: %s", arr);
    
    // Example 2: Ctrl+D handling
    printf("Enter text (or Ctrl+D): ");
    if (fgets(arr, sizeof(arr), stdin) == NULL) {
        printf("\nCtrl+D was pressed!\n");
    } else {
        printf("You typed: %s", arr);
    }
    
    return 0;
}