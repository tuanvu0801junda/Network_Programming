#include <stdio.h>
#include "support.h"

int main(){
    char choice;
    char yY; 
    while (1){
        printf("\nLearning Management System\n");
        printf("-------------------------------------\n");
        printf("1. Add a new score board\n");
        printf("2. Add score\n");
        printf("3. Remove score\n");
        printf("4. Search score\n");
        printf("5. Display score board and score report\n");
        printf("Your choice (1-5, other to quit):");
        scanf("%c%*c",&choice);

        switch (choice){
        case '1':
            printf("\n1. Add a new score board\n");
            CreateNewScoreBoard();
            while(1){
                printf("\nContinue to use function 1 ? ('y' or 'Y'): ");
                scanf("%c%*c", &yY);
                if (yY == 'y' || yY == 'Y') {
                    printf("1. Add a new score board\n");
                    CreateNewScoreBoard();
                }
                else break;
            }
            break;
        
        case '2':
            printf("\n2. Add score\n");
            AddScoreOneStudent();
            while(1){
                printf("\nContinue to use function 2 ? ('y' or 'Y'): ");
                scanf("%c%*c", &yY);
                if (yY == 'y' || yY == 'Y') {
                    printf("2. Add score\n");
                    AddScoreOneStudent();
                }
                else break;
            }
            break;

        case '3':
            printf("\n3. Remove score\n");
            removeScore1Student();
            while(1){
                printf("\nContinue to use function 3 ? ('y' or 'Y'): ");
                scanf("%c%*c", &yY);
                if (yY == 'y' || yY == 'Y') {
                    printf("3. Remove score\n");
                    removeScore1Student();
                }
                else break;
            }
            break;
        
        case '4':
            printf("\n4. Search score\n");
            Search1StudentScore();
            while(1){
                printf("\nContinue to use function 4 ? ('y' or 'Y'): ");
                scanf("%c%*c", &yY);
                if (yY == 'y' || yY == 'Y') {
                    printf("4. Search score\n");
                    Search1StudentScore();
                }
                else break;
            }
            break;

        case '5':
            printf("\n5. Display score board and score report\n");
            showReport();
            while(1){
                printf("\nContinue to use function 5 ? ('y' or 'Y'): ");
                scanf("%c%*c", &yY);
                if (yY == 'y' || yY == 'Y') {
                    printf("5. Display score board and score report\n");
                    showReport();
                }
                else break;
            }
            break;

        default:
            printf("Exit\n");
            break;
        } //switch
        if (choice < '1' || choice > '5') break;
    }//while
    
}