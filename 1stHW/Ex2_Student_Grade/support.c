#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "support.h"
#include "structure.h"

void CreateNewScoreBoard(){
    char id[10], name[100], semester[10], fileName[25];
    int rate, count, i=0;
    char mssv[9], familyName[40], realName[25], grade[5];
    float mid, final;

    printf("\tSubjectID: ");
    gets(id);
    printf("\tSubject name: ");
    gets(name);

    printf("\tMidterm counts for (in %%): ");
    scanf("%d%*c",&rate);
    printf("\tSemester: ");
    gets(semester);
    printf("\tStudent count: ");
    scanf("%d%*c",&count);

    strcpy(fileName,"");  // if not --> weird characters mixed in
    strcat(fileName,id);
    strcat(fileName,"_");
    strcat(fileName,semester);
    strcat(fileName,".txt");

    FILE* f = fopen(fileName,"w");
    if (f == NULL) printf("Cannot open. File doesn't exit\n");
    else{
        fprintf(f, "SubjectID|%s\n",id);
        fprintf(f, "Subject|%s\n",name);
        fprintf(f, "F|%d|%d\n",rate,100-rate);
        fprintf(f, "Semester|%s\n",semester);
        fprintf(f, "StudentCount|%d\n", count);

        printf("Please enter %d row(s) of student information!\n",count);
        for(i=0;i<count;i++){
            printf("\nEnter MSSV: ");
            gets(mssv);
            printf("Enter family name: ");
            gets(familyName);
            printf("Enter real name: ");
            gets(realName);
            printf("Enter midterm: ");
            scanf("%f%*c",&mid);
            printf("Enter final: ");
            scanf("%f%*c",&final);
            char CharScore = convertScore(rate*mid/100 + (100-rate)*final/100);
            grade[0] = ' '; grade[1] = CharScore; grade[2] = ' '; grade[3] = '\0';
            fprintf(f,"S|%s|%-35s|%-20s|%-6.1f|%-6.1f|%s|\n",mssv,familyName,realName,mid,final,grade);
        }
        fclose(f);
        printf("New subject file created!");
    }
    printf("\n");
}

char convertScore(float inputScore){
    if (inputScore > 10 || inputScore <0){
        printf("Invalid Score!\n");
        return ' ';
    } else if (inputScore >= 8.5) return 'A';
    else if (inputScore >= 7.0) return 'B';
    else if (inputScore >= 5.5) return 'C';
    else if (inputScore >= 4.0) return 'D';
    else return 'F';
}

void AddScoreOneStudent(){
    char id[10], semester[10], fileName[25], contain[100], temp, *token;
    int i=0;

    const char cchar[2] = "|";
    student root = NULL;
    subject new = (subject)malloc(sizeof(struct Subject));
    char mssv[9], familyName[40], name[25], grade[5];
    float mid, final;

    printf("\tSubjectID: ");
    gets(id);
    printf("\tSemester: ");
    gets(semester);

    strcpy(fileName,"");
    strcat(fileName,id);
    strcat(fileName,"_");
    strcat(fileName,semester);
    strcat(fileName,".txt");

    FILE* f = fopen(fileName,"r");
    if (f == NULL) printf("Cannot open. File doesn't exit\n");
    else{
        i=0;
        while(1){
            switch(i){
                case 0:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->subjectID,token);
                    break;
                case 1:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->subjectName,token);
                    break;
                case 2:
                    fscanf(f, "%c|%d|%d\n", &temp, &(new->midRate), &(new->final));
                    break;
                case 3:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->semester,token);
                    break;
                case 4:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    new->studentCount = atoi(token);
                    break;
                default:
                    printf("No suitable!\n");
                    break;
            } 
            i++;
            if (i>4) break;
        }

        while(1){ 
            i = 0;
            fscanf(f, "%[^\n]%*c", contain);
            if(feof(f)) break;
            token = strtok(contain,cchar); //token(here) = 'S'
            token = strtok(NULL, cchar); //token(here) = MSSV

            while(token!=NULL){  
                if (i==0){
                    strcpy(mssv,token);  
                } else if (i==1){
                    strcpy(familyName,token);  
                } else if (i==2){
                    strcpy(name,token);
                } else if (i==3){
                    mid = atof(token);
                } else if (i==4){
                    final = atof(token);
                } else if (i==5){
                    strcpy(grade,token);
                } else printf("No suitable!\n");
                i++; token = strtok(NULL, cchar);
            }
            root = insert_node(root,mssv,familyName,name,mid,final,grade);
        }
        fclose(f);
        printf("\nEnter student personal information!\n");
        printf("\tEnter new MSSV: ");
        gets(mssv);
        printf("\tEnter new familyName(family+middle): ");
        gets(familyName);
        printf("\tEnter real name: ");
        gets(name);
        printf("\tEnter midterm: ");
        scanf("%f%*c",&mid);
        printf("\tEnter final: ");
        scanf("%f%*c",&final);

        char CharScore = convertScore(mid*(new->midRate)/100 + final*(new->final)/100);
        strcpy(grade," ");
        grade[1] = CharScore; grade[2] = ' '; grade[3] = '\0';
        root = insert_node(root,mssv,familyName,name,mid,final,grade);

        f = fopen(fileName,"w");
        fprintf(f,"SubjectID|%s\n",new->subjectID);
        fprintf(f,"Subject|%s\n",new->subjectName);
        fprintf(f,"F|%d|%d\n",new->midRate,new->final);
        fprintf(f,"Semester|%s\n",new->semester);
        fprintf(f,"StudentCount|%d\n",countStudent(root));
        student p;
        for(p=root;p!=NULL;p=p->next){
            fprintf(f,"S|%s|%-35s|%-20s|%-6.1f|%-6.1f|%s|\n",p->mssv,p->familyName,p->realName,p->midterm,p->finalTerm,p->grade);
        }
        fclose(f);
        root = freeAllList(root);
        free(new);
        printf("Add a student's score successfully!");
    }
    printf("\n");
}

void Search1StudentScore(){
    char id[10], semester[10], MSSV[9], fileName[25], contain[100], *token;
    const char cchar[2] = "|";
    int flag=0,i=0;

    printf("\tSubjectID: ");
    gets(id);
    printf("\tSemester: ");
    gets(semester);
    printf("\tEnter MSSV: ");
    gets(MSSV);

    strcpy(fileName,"");    // if not --> weird characters will be mixed in
    strcat(fileName,id);
    strcat(fileName,"_");
    strcat(fileName,semester);
    strcat(fileName,".txt");

    FILE* f = fopen(fileName,"r");
    if (f == NULL) printf("Cannot open. File doesn't exit\n");
    else{
        for(i=0;i<5;i++){fscanf(f, "%[^\n]%*c", contain);}
        
        printf("\nSEARCH RESULT:\n\n");
        printf("%-16s%-35s%-20s%-16s%-16s%s\n","MSSV","FAMILY NAME","REAL NAME","MIDTERM","FINAL","GRADE");
        while(1){ 
            i = 0; flag = 0;
            fscanf(f, "%[^\n]%*c", contain);
            if(feof(f)) break;
            token = strtok(contain,cchar);
            
            while(token!=NULL){  
                //printf("len(%s) = %ld\n",token, strlen(token));
                if (i==1 && (strcmp(MSSV,token) == 0)) flag = 1;
                if (flag == 1) printf("%-16s",token);
                i++;
                token = strtok(NULL, "|");
            }
        }
        fclose(f);
    }
    printf("\n");
}

void removeScore1Student(){
    char id[10], semester[10], MSSV[9], fileName[25], contain[100], *token, temp;
    const char cchar[2] = "|";
    int i=0;

    student root = NULL;
    subject new = (subject)malloc(sizeof(struct Subject));
    char mssv[9], familyName[40], name[25], grade[5];
    float mid, final;

    printf("\tSubjectID: ");
    gets(id);
    printf("\tSemester: ");
    gets(semester);
    printf("\tEnter MSSV: ");
    gets(MSSV);

    strcpy(fileName,""); // if not --> weird characters will be mixed in
    strcat(fileName,id);
    strcat(fileName,"_");
    strcat(fileName,semester);
    strcat(fileName,".txt");

    FILE* f = fopen(fileName,"r");
    if (f == NULL) printf("Cannot open. File doesn't exit\n");
    else{
        i=0;
        while(1){
            switch(i){
                case 0:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->subjectID,token);
                    break;
                case 1:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->subjectName,token);
                    break;
                case 2:
                    fscanf(f, "%c|%d|%d\n", &temp, &(new->midRate), &(new->final));
                    break;
                case 3:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->semester,token);
                    break;
                case 4:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    new->studentCount = atoi(token);
                    break;
                default:
                    printf("No suitable!\n");
                    break;
            } 
            i++;
            if (i>4) break;
        }

        while(1){ 
            i = 0;;
            fscanf(f, "%[^\n]%*c", contain);
            if(feof(f)) break;
            token = strtok(contain,cchar); //token(here) = 'S'
            token = strtok(NULL, cchar); //token(here) = MSSV

            while(token!=NULL){  
                if (i==0){
                    strcpy(mssv,token);  
                } else if (i==1){
                    strcpy(familyName,token);  
                } else if (i==2){
                    strcpy(name,token);
                } else if (i==3){
                    mid = atof(token);
                } else if (i==4){
                    final = atof(token);
                } else if (i==5){
                    strcpy(grade,token);
                } else printf("No suitable!\n");
                i++; token = strtok(NULL, cchar);
            }
            root = insert_node(root,mssv,familyName,name,mid,final,grade);
        }
        fclose(f);
        root = delete_node(root,MSSV);
        f = fopen(fileName,"w");

        fprintf(f,"SubjectID|%s\n",new->subjectID);
        fprintf(f,"Subject|%s\n",new->subjectName);
        fprintf(f,"F|%d|%d\n",new->midRate,new->final);
        fprintf(f,"Semester|%s\n",new->semester);
        fprintf(f,"StudentCount|%d\n",countStudent(root));
        student p;
        
        for(p=root;p!=NULL;p=p->next){
            fprintf(f,"S|%s|%-35s|%-20s|%-6.1f|%-6.1f|%s|\n",p->mssv,p->familyName,p->realName,p->midterm,p->finalTerm,p->grade);
        }
        fclose(f);
        root = freeAllList(root);
        free(new);
        printf("Remove selected student successfully");
    }
    printf("\n");
}

void showReport(){
    char id[9], semester[10], fileName[25], *token;
    student root = NULL;
    subject new = (subject)malloc(sizeof(struct Subject));
    char mssv[9], familyName[40], name[25], grade[5], temp, contain[100];
    float mid, final;
    const char cchar[2] = "|";
    int i = 0;

    printf("\tSubjectID: ");
    gets(id);
    printf("\tSemester: ");
    gets(semester);

    strcpy(fileName,""); // if not --> weird characters will be mixed in
    strcat(fileName,id);
    strcat(fileName,"_");
    strcat(fileName,semester);
    strcat(fileName,".txt");

    FILE* f = fopen(fileName,"r");
    if (f == NULL) printf("Cannot open. File doesn't exit\n");
    else{
        i=0;
        while(1){
            switch(i){
                case 0:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->subjectID,token);
                    break;
                case 1:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->subjectName,token);
                    break;
                case 2:
                    fscanf(f, "%c|%d|%d\n", &temp, &(new->midRate), &(new->final));
                    break;
                case 3:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    strcpy(new->semester,token);
                    break;
                case 4:
                    fscanf(f, "%[^\n]%*c", contain);
                    token = strtok(contain,cchar);
                    token = strtok(NULL,cchar);
                    new->studentCount = atoi(token);
                    break;
                default:
                    printf("No suitable!\n");
                    break;
            } 
            i++;
            if (i>4) break;
        }

        while(1){ 
            i = 0;;
            fscanf(f, "%[^\n]%*c", contain);
            if(feof(f)) break;
            token = strtok(contain,cchar); //token(here) = 'S'
            token = strtok(NULL, cchar); //token(here) = MSSV

            while(token!=NULL){  
                if (i==0){
                    strcpy(mssv,token);  
                } else if (i==1){
                    strcpy(familyName,token);  
                } else if (i==2){
                    strcpy(name,token);
                } else if (i==3){
                    mid = atof(token);
                } else if (i==4){
                    final = atof(token);
                } else if (i==5){
                    strcpy(grade,token);
                } else printf("No suitable!\n");
                i++; token = strtok(NULL, cchar);
            }
            root = insert_node(root,mssv,familyName,name,mid,final,grade);
        }
        fclose(f);
        
        strcpy(fileName,""); // if not --> weird characters will be mixed in
        strcat(fileName,id);
        strcat(fileName,"_");
        strcat(fileName,semester);
        strcat(fileName,"_rp.txt");
        FILE* f1 = fopen(fileName,"w");
        fclose(f1);
        f1 = fopen(fileName,"a");

        printf("\n*****REPORT SUBJECT*****\n");
        printf("SubjectID: %s\n",new->subjectID);
        printf("SubjectName: %s\n",new->subjectName);
        printf("Midterm Rate: %d %%, Final-Term Rate: %d %%\n",new->midRate,new->final);
        printf("Semester: %s\n",new->semester);
        printf("Student count: %d\n",new->studentCount);
        printf("\n\t\t\tLIST OF STUDENT(S) AND THEIR SCORE(S)\n\n");
        student p = root;
        for(p=root;p!=NULL;p=p->next){
             printf("S|%s|%-35s|%-20s|%-6.1f|%-6.1f|%s|\n",p->mssv,p->familyName,p->realName,p->midterm,p->finalTerm,p->grade);
        }
        printf("\n");
        findHighestPoint(root,new->midRate,new->final,f1);
        findLowestPoint(root,new->midRate,new->final,f1);
        calculateAverage(root,new->midRate,new->final,f1);
        fprintf(f1,"\n\nA histogram of the subject %s is: \n",id);
        printf("\nA histogram of the subject %s is: \n",id);
        printStars(root," A ",f1);
        printStars(root," B ",f1);
        printStars(root," C ",f1);
        printStars(root," D ",f1);
        printStars(root," F ",f1);
        fclose(f1);

        root = freeAllList(root);
        free(new);
        printf("\nResult was exported successfully to file \"%s\"",fileName);
    }
    printf("\n");
}

char* ltrim(char* s){
    while(isspace(*s)) s++;
    return s;
}

char* rtrim(char* s){
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char* trim(char* s){
    return rtrim(ltrim(s)); 
}