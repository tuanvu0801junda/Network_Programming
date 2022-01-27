#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structure.h"
#include "support.h"

student create_node(char*mssv, char*familyName, char*realName, float midterm, float finalTerm, char* grade){
  student new = (student)malloc(sizeof(struct Student));
  if(new == NULL) printf("Cannot create linked list\n");
  else{
    strcpy(new->mssv,mssv);
    strcpy(new->familyName,familyName);
    strcpy(new->realName,realName);
    new->midterm = midterm;
    new->finalTerm = finalTerm;
    strcpy(new->grade,grade);
    new->next = NULL;
  }
  return new;
}

student look_for_node(student root, char* mssv){
   student result,p; 
   if(root == NULL) return NULL;
   else{
     for(p=root;p!=NULL;p=p->next){
        if (strcmp(p->mssv,mssv) == 0) result = p;
    }
  }
  return result;
}

student insert_node(student root, char*mssv, char*familyName, char*realName, float midterm, float finalTerm, char* grade){
    student new = create_node(mssv,familyName,realName,midterm,finalTerm,grade);
    if(root == NULL) root = new;
    else{
        student p = root;
        while(p->next!=NULL) {p = p->next;}
        p->next = new;
    }
    return root;
}

student delete_node(student root, char*mssv){
    if (root == NULL) printf("Empty list!\n");
    else{
      student searchResult = look_for_node(root,mssv);
      if (searchResult == NULL) printf("MSSV not found!\n");
      else{
        if (searchResult == root){
            student temp = root->next;
            free(root);
            root = temp;
        }
        else if (searchResult->next == NULL){
            student p = root;
            while(p->next->next!=NULL) {p = p->next;}
            free(p->next);
            p->next = NULL;
        }
        else{
            student p = root;
            while(p->next != searchResult) {p=p->next;}
            p->next = searchResult->next;
            //searchResult->next = NULL;
            free(searchResult);
        }
      }
    }
  return root;
}

student freeAllList(student root){
  student p = NULL;
  while(root!=NULL){
    p = root;
    root = root->next;
    free(p);
  }
  return root;
}

int countStudent(student root){
  int count = 0;
  student p = root;
  while(p!=NULL){
    count = count+1;
    p = p->next;
  }
  return count;
}

void findHighestPoint(student root, int midRate, int finalRate, FILE* file){
  student p = root; 
  char fullname[40], temp[40];
  float compare, high = midRate*(root->midterm)/100 + finalRate*(root->finalTerm)/100;

  strcpy(fullname,""); strcpy(temp,"");
  for (p=root;p!=NULL;p=p->next){
    compare = midRate*(p->midterm)/100 + finalRate*(p->finalTerm)/100;
    if (high <= compare) {
        high = compare;
        char* fname = trim(p->familyName); strcpy(temp,fname);
        strcat(temp," ");
        char* rname = trim(p->realName); strcat(temp,rname);
        strcpy(fullname,temp);
    }
  }
  fprintf(file, "The student with the highest mark is: %s\n",fullname);
  printf("The student with the highest mark is: %s\n",fullname);
}

void findLowestPoint(student root, int midRate, int finalRate, FILE* file){
  student p = root; 
  char fullname[40], temp[40];
  float compare, high = midRate*(root->midterm)/100 + finalRate*(root->finalTerm)/100;

  strcpy(fullname,""); strcpy(temp,"");
  for (p=root;p!=NULL;p=p->next){
    compare = midRate*(p->midterm)/100 + finalRate*(p->finalTerm)/100;
    if (high >= compare) {
        high = compare;
        char* fname = trim(p->familyName); strcpy(temp,fname);
        strcat(temp," ");
        char* rname = trim(p->realName); strcat(temp,rname);
        strcpy(fullname,temp);
    }
  }
  fprintf(file, "The student with the lowest mark is: %s\n",fullname);
  printf("The student with the lowest mark is: %s\n",fullname);
}

void calculateAverage(student root, int midRate, int finalRate, FILE* file){
  int num = countStudent(root);
  float sum = 0;
  student p = root;
  for (p=root;p!=NULL;p=p->next){
    sum += midRate*(p->midterm)/100 + finalRate*(p->finalTerm)/100;
  }
  fprintf(file, "The average mark is: %.2f\n",sum/num);
  printf("The average mark is: %.2f\n",sum/num);
}

void printStars(student root, char* grade, FILE* file){
  fprintf(file, "\n%s: ",grade);
  printf("\n%s: ",grade);
  int i=0,count = 0;
  student p = root;
  for(p=root;p!=NULL;p=p->next){
    if (strcmp(p->grade,grade) == 0) count += 1;
  }
  for(i=0;i<count;i++){
    fprintf(file, "*");
    printf("*");
  }
}



// int main(){
//   char mssv[9],familyName[20], realName[10], grade; 
//   float midterm, finalTerm;
//   student root = NULL;
//   FILE* f = fopen("student.txt","r");
//   root = insert_node(root, "20101160", "NGUYEN_VAN", "AN", 8.5, 7.0, 'B');
//   root = insert_node(root, "20191182", "TRAN_QUOC", "BO", 10.0, 9.0, 'A' );
//   root = insert_node(root, "20111191", "LE_BANG", "CHAN", 5.5, 4.5, 'D');
//   root = insert_node(root, "20101216", "NGUYEN_VAN", "DUONG", 8.5, 8.5, 'A');
//   root = insert_node(root, "20170812", "NGUYEN_TRUNG", "THANH", 9.0, 8.5, 'A');

//   printf("Before\n");
//   for(student p=root;p!=NULL;p=p->next){
//     printf("%s, %s, %s, %.1f, %.1f, %c\n", p->mssv, p->familyName, p->realName, p->midterm, p->finalTerm, p->grade);
//   }

//   printf("number of student(s): %d\n",countStudent(root));

//   root = delete_node(root, "20101160");
//   printf("After\n");
//   for(student p=root;p!=NULL;p=p->next){
//     printf("%s, %s, %s, %.1f, %.1f, %c\n", p->mssv, p->familyName, p->realName, p->midterm, p->finalTerm, p->grade);
//   }

//   printf("After 1\n");
//   root = delete_node(root, "20111191");
//   for(student p=root;p!=NULL;p=p->next){
//     printf("%s, %s, %s, %.1f, %.1f, %c\n", p->mssv, p->familyName, p->realName, p->midterm, p->finalTerm, p->grade);
//   }

//   printf("After 2\n");
//   root = delete_node(root, "20170812");
//   for(student p=root;p!=NULL;p=p->next){
//     printf("%s, %s, %s, %.1f, %.1f, %c\n", p->mssv, p->familyName, p->realName, p->midterm, p->finalTerm, p->grade);
//   }

//   printf("After 3\n");
//   root = freeAllList(root);
//   for(student p=root;p!=NULL;p=p->next){
//     printf("%s, %s, %s, %.1f, %.1f, %c\n", p->mssv, p->familyName, p->realName, p->midterm, p->finalTerm, p->grade);
//   }
// }