
#ifndef _NODEATTR_H_
#define _NODEATTR_H_

#define NODE_NAME_MAX_LENGTH  50

typedef struct{
  int id;
  char name[NODE_NAME_MAX_LENGTH];
  char mode;
  int status;
  Group_Table *group_table;
}NodeAttr,*pNodeAttr;

#endif//_NODEATTR_H_