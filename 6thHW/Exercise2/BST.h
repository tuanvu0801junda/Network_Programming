#ifndef __BST_H_
#define __BST_H_

#define LEN 100

struct tree{
  char userId[LEN];
  char password[LEN];
  int status;
  struct tree *left,*right;
};

typedef struct tree *node;
node create_node(char *userId, char *password, int status);
node look_for_node(node root, char* userId);
node insert_node(node root, char *userId, char *password, int status);
void free_tree(node node);
void print_file(node root, FILE *f);
#endif