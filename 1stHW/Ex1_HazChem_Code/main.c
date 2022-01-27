#include <stdio.h>
#include <string.h>
#include "HazChemCode.h"

int main(){
    char HazChem[5];
    char color[4];
    int yesno;
    printf("Enter HazChem code (uppercase): ");
    gets(HazChem);

    if (strlen(HazChem) > 3 || strlen(HazChem) == 0){
        printf("0 < HazChem Code's length <= 3\n");
    }
    else {
        char validCode = checkValid(HazChem);
        if (validCode == '1'){
            printf("Invalid at 1st position character (only in {1,2,3,4})!\n");
            return 0;
        }
        else if (validCode == '2'){
            printf("Invalid at 2nd position character (only in {P,R,S,T,W,X,Y,Z})!\n");
            return 0;
        }
        else if (validCode == '3'){
            printf("Invalid at 3rd position character (only in {E})!\n");
            return 0;
        }
        else {
            if (HazChem[1] == 'S' || HazChem[1] == 'T' || HazChem[1] == 'Y' || HazChem[1] == 'Z'){
                printf("Is the %c reverse coloured ?: ",HazChem[1]);
                gets(color);
                if (strcmp(color,"yes") == 0) yesno = 1;
                else if (strcmp(color,"no") == 0) yesno = 0;
                else printf("Please re-enter \"yes\" or \"no\"\n");
            } else yesno = 0;

            showFirstChar(HazChem[0]);
            showSecondChar(HazChem[1],yesno);
            if (strlen(HazChem) == 3){
                showThirdChar(HazChem[2]);
            }
            printf("\n*****************************\n");
        }
    }
}