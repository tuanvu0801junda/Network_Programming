#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BST.h"

node create_node(char *userId, char *password, int status){
  node newNode = (node)malloc(sizeof(struct tree));
  if(newNode == NULL) printf("Cannot create tree\n");
  else{
    strcpy(newNode->userId,userId);
    strcpy(newNode->password,password);
    newNode->status = status;
    newNode->left = NULL;
    newNode->right = NULL;
  }
  return newNode;
}

node look_for_node(node root, char* userId){
  if(root == NULL) return NULL;
  else{
    if(strcmp(root->userId,userId) <0){
      root = look_for_node(root->right,userId);
    }
    else if(strcmp(root->userId,userId) >0){
      root = look_for_node(root->left,userId);
    }
  }
  return root;
}

node insert_node(node root, char *userId, char *password, int status){
  if(root == NULL) root = create_node(userId,password,status);
  else{
    //NOT EXIST
    node result = look_for_node(root,password);
    if(result == NULL){
      if(strcmp(root->userId,userId) < 0) root->right = insert_node(root->right,userId,password,status);
      if(strcmp(root->userId,userId) > 0) root->left = insert_node(root->left,userId,password,status);
    }
  }
  return root;
}
void free_tree(node node){
   //post-order like FatalError hinted at
    if (node != NULL) {
        free_tree(node->right);
        free_tree(node->left);
        free(node);
    } else return;
}

void print_file(node root, FILE *f){
    if(root == NULL) return;
    else{
      print_file(root->left, f);
      fprintf(f, "%s\t%s\t\t%d\n",root->userId,root->password,root->status);
      print_file(root->right, f);
    }
}