#include <stdio.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define BOLD    "\e[1m"
#define BOLDOFF "\e[0m"

int main(){
    // printf(RED     "This text is RED!"     RESET "\n");
    // printf(GREEN   "This text is GREEN!"   RESET "\n");
    // printf(YELLOW  "This text is YELLOW!"  RESET "\n");
    // printf(BLUE    "This text is BLUE!"    RESET "\n");
    // printf(MAGENTA "This text is MAGENTA!" RESET "\n");
    // printf(CYAN    "This text is CYAN!"    RESET "\n");
    
    printf("🐴" GREEN BOLD " 1 " BOLDOFF RESET "kicked 🐴" RED BOLD " 3 " BOLDOFF RESET "\n\n");
    printf("🐴" YELLOW BOLD " 2 " BOLDOFF RESET "cannot kick 🐴" CYAN BOLD " 3 " BOLDOFF RESET "\n\n"); 
    printf(YELLOW BOLD "Horse 2 " BOLDOFF RESET "cannot kick " BLUE BOLD "Horse 3" BOLDOFF RESET "\n\n"); 
    printf(YELLOW BOLD "Horse 4 " BOLDOFF RESET "kicked " RED BOLD "Horse 1" BOLDOFF RESET "\n\n"); 
}