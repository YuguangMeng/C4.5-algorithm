/*
 ------------------------------------------------------------------------
|  This C4.5 code for CS570 is my own work written without consuting a   |
|  tutor  or code written by other students - Yuguang Meng, Oct 15, 2015 |
 ------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>

#define MAX_ATTR 32

using namespace std;

int class_attr=0;

enum Children_type
{
  NODE,
  VALUE
};

struct Node_C45;

typedef struct Child
{
  char attr_value[8];
  int type;
  union
  {
    Node_C45 *node;
    char value[8];
  } node;
  Child *next;
} Child;

typedef struct Node_C45
{
  int attr;
  Child *child;
} Node_C45;

typedef struct Node_data
{
  char value[MAX_ATTR][8];
  Node_data *next;
} Node_data;

typedef struct Node_class
{
  char *value;
  int count;
  Node_class *next;
} Node_class;

typedef struct Node_attr_value_class
{
  char *value;
  int count;
  Node_class *class_list;
  Node_attr_value_class *next;
} Node_attr_value_class;

typedef struct Attr_list
{
  int attr;
  Attr_list *next;
} Attr_list;

typedef struct Filter
{
  int attr;
  char value[8];
  Filter *next;
} Filter;

typedef struct Node_PXCi_PCi
{
	char value[8];
  double P_value;
  Node_PXCi_PCi *next;
}	Node_PXCi_PCi;

void Build_C45(Node_data *, Attr_list **, Filter *, Node_C45 **, char **, int *);
void Clear_Filter(Filter **);
int Is_Data_Empty(Node_data *, Filter *);
void Build_Child_Filter(Filter *, Filter **, int, char *);
void Update_attr_list(Attr_list **, int);
void Attribute_Selection_C45(Node_data *, Filter *, Attr_list *, int *, Node_attr_value_class **);
int Read_Data(Node_data **, char *);
int InsertData(Node_data **, char *);
void RemoveSpace(char *, char *);
int IsLetter(char);
int IsDigit(char);
void InsertAttrValueList(Node_attr_value_class **, char *, char *);
void InsertClassList(Node_class **, char *);
int Is_Same_Value(char **, Node_data *, Filter *);
int Is_Selected_Tuple(Node_data *, Filter *);
void Get_Majority_Class(Node_data *, Filter *, char **);
void Print_C45(FILE *, int *, Node_C45 *, int);
void Read_Filter(Filter *);
void Print_Class_List(Node_class *);
void Print_Attr_List(Attr_list *);
void Print_attr_value_class(Node_attr_value_class *);
void Calculate_Accuracy(FILE *, Node_C45 *, Node_data *, int *, int *);
int Look_For_Class(Node_C45 *, Node_data *, char **);
void Naive_Bayesian(FILE *, Node_data *, Node_data *, int, int *, int *);
void Create_PXCi_PCi(Node_PXCi_PCi **, Node_attr_value_class *, double);
void Clear_PXCi_PCi(Node_PXCi_PCi **);
void Look_For_Ci(Node_PXCi_PCi *, char **);
void Print_PXCi_PCi(Node_PXCi_PCi *);