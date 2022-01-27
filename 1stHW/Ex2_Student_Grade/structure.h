#ifndef __STRUCTURE_H_
#define __STRUCTURE_H_

struct Student{
    char mssv[9];
    char familyName[40];
    char realName[25];
    float midterm;
    float finalTerm;
    char grade[5];
    struct Student *next;
};

typedef struct Student *student;

struct Subject{
    char subjectID[9];
    char subjectName[100];
    int midRate,final;
    char semester[6];
    int studentCount;
};

typedef struct Subject *subject;

student create_node(char*mssv, char*familyName, char*realName, float midterm, float finalTerm, char* grade);
student look_for_node(student root, char* mssv);
student insert_node(student root, char*mssv, char*familyName, char*realName, float midterm, float finalTerm, char* grade);
student delete_node(student root, char*mssv);
student freeAllList(student root);
int countStudent(student root);

void findHighestPoint(student root, int midRate, int finalRate, FILE* file);
void findLowestPoint(student root, int midRate, int finalRate, FILE* file);
void calculateAverage(student root, int midRate, int finalRate, FILE* file);
void printStars(student root, char* grade, FILE* file);
#endif