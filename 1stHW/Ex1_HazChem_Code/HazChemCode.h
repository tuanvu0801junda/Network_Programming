#ifndef __HAZCHEMCODE_H_
#define __HAZCHEMCODE_H_

struct FirstChar{
    char num;
    char *message;
};

struct Reactivity{
    char reactChar;
    char *message;
};

enum ProtectionEnum{
    Full,
    BA,
    BA_for_fire
};

struct Protection{
    enum ProtectionEnum protectEnum;
    char *message;
};

enum ContainmentEnum{
    Dilute,
    Contain
};

struct Containment{
    enum ContainmentEnum containEnum;
    char *message;
};

enum Confirm{
    no,
    yes
};

struct SecondChar{
    char note;
    enum Confirm confirm;
    char reactChar;
    enum ProtectionEnum protectEnum;
    enum ContainmentEnum containEnum;
};

void showFirstChar(char compareChar);
void showSecondChar(char secondInput,int status);
void showThirdChar(char thirdChar);
char checkValid(char* HazCode);
#endif
