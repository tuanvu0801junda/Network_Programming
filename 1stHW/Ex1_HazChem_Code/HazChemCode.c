#include <stdio.h>
#include <string.h>
#include "HazChemCode.h"

#define FIRST_CHAR_LENGTH 4
#define REACT_LENGTH 2
#define PROTECT_LENGTH 3
#define CONTAIN_LENGTH 2
#define SECOND_CHAR_LENGTH 12
#define THIRD_CHAR_LENGTH 1

struct FirstChar firstChar[FIRST_CHAR_LENGTH] = {
    {'1', "jets"},
    {'2', "fog"},
    {'3', "foam"},
    {'4', "dry Agent"}
};

struct Reactivity reactivity[REACT_LENGTH] = {
    {' ', "no reaction"},
    {'V', "can be violently reactive."}
};

struct Protection protection[PROTECT_LENGTH] = {
    {Full, "full protective clothing must be worn"},
    {BA, "breathing apparatus"},
    {BA_for_fire, "breathing apparatus, protective gloves for fire only"}
};

struct Containment containment[CONTAIN_LENGTH] = {
    {Dilute, "the dangerous goods may be washed down to a drain with a large quantity of water"},
    {Contain, "a need to avoid spillages from entering drains or water courses."}
};

struct SecondChar secondChar[SECOND_CHAR_LENGTH] = {
    {'P',no, 'V', Full,        Dilute  },
    {'R',no, ' ', Full,        Dilute  },
    {'S',no, 'V', BA,          Dilute  },
    {'S',yes,'V', BA_for_fire, Dilute  },
    {'T',no, ' ', BA,          Dilute  },
    {'T',yes,' ', BA_for_fire, Dilute  },
    {'W',no, 'V', Full,        Contain },
    {'X',no, ' ', Full,        Contain },
    {'Y',no, 'V', BA,          Contain },
    {'Y',yes,'V', BA_for_fire, Contain },
    {'Z',no, 'V', BA,          Contain },
    {'Z',yes,'V', BA_for_fire, Contain }
};

struct FirstChar thirdChar[THIRD_CHAR_LENGTH] = {
    {'E', "consider evacuation"}
};

void showFirstChar(char compareChar){
    int i;
    for (i=0;i<FIRST_CHAR_LENGTH;i++){
        if (compareChar == firstChar[i].num){
            printf("\n***Emergency action advice***\n\n");
            printf("%-11s: %s\n", "Material", firstChar[i].message);
        }
    }
}

void showSecondChar(char secondInput, int status){
    int i,j;
    for (i=0;i<SECOND_CHAR_LENGTH;i++){
        if (secondInput == secondChar[i].note && status == secondChar[i].confirm){
            for (j=0;j<REACT_LENGTH;j++){
                if (secondChar[i].reactChar == reactivity[j].reactChar)
                    printf("%-11s: %s\n", "Reactivity" ,reactivity[j].message);
            }

            for (j=0;j<PROTECT_LENGTH;j++){
                if (secondChar[i].protectEnum == protection[j].protectEnum)
                    printf("%-11s: %s\n", "Production", protection[j].message);
            }

            for (j=0;j<CONTAIN_LENGTH;j++){
                if (secondChar[i].containEnum == containment[j].containEnum)
                    printf("%-11s: %s\n", "Containment", containment[j].message);
            }
        }
    }
}

void showThirdChar(char thirdInput){
    int i;
    for (i=0;i<THIRD_CHAR_LENGTH;i++){
        if (thirdInput == thirdChar[i].num){
            printf("%-11s: %s\n", "Evacuation", thirdChar[i].message);
        }
    }
}

char checkValid(char* HazCode){
    int i,valid = 0;
    for (i=0;i<FIRST_CHAR_LENGTH;i++){
        if (HazCode[0] == firstChar[i].num) valid = 1;
    }
    if (valid != 1) return '1';

    valid = 0;
    for (i=0;i<SECOND_CHAR_LENGTH;i++){
        if (HazCode[1] == secondChar[i].note) valid = 1;
    }
    if (valid != 1) return '2';

    valid = 0;
    if (strlen(HazCode) == 3){
        for (i=0;i<THIRD_CHAR_LENGTH;i++){
            if (HazCode[2] == thirdChar[i].num) valid = 1;
        }
        if (valid != 1) return '3';
    }
    if (valid == 1) return '0';
}