/*
 ------------------------------------------------------------------------
|  This C4.5 code for CS570 is my own work written without consuting a   |
|  tutor  or code written by other students - Yuguang Meng, Oct 15, 2015 |
 ------------------------------------------------------------------------
*/

#include "C4.5.h"

int main(int argc, char ** argv)
{
  Node_C45 *Node=NULL;
  Node_data *train_dat=NULL, *test_dat=NULL;
  char attr_name[][32]= {"ediblity", "cap-shape","cap-surface"," cap-color","bruises","odor","gill-attachment",
                         "gill-spacing","gill-size","gill-color","stalk-shape","stalk-surface-above-ring",
                         "stalk-surface-below-ring","stalk-color-above-ring","stalk-color-below-ring",
                         "veil-type","veil-color","ring-number","ring-type","spore-print-color",
                         "population","habitat"
                        };
  FILE *fp=NULL;
  int i, n_attr;
  Attr_list *attr_list=NULL, *p_attr_list=NULL;
  char *value=NULL;
  Filter *f=NULL;
  int return_type;
  int label[128];
  int n_total=0, n_correct=0;

  if(argc!=4)
  {
    printf("Error: Usage: %s training_file test_file output_file!\n", argv[0]);
    exit(0);
  }

  printf("\n");
  printf(" -----------------------------------------------------------------------\n");
  printf("|  This C4.5 code for CS570 is my own work written without consuting a  |\n");
  printf("|  tutor  or code written by other students - Yuguang Meng, Oct 15 2015 |\n");
  printf(" -----------------------------------------------------------------------\n");
  printf("\n");

  n_attr=Read_Data(&train_dat, argv[1]);
  Read_Data(&test_dat, argv[2]);

  printf("The traning data file is:   %s.\n", argv[1]);
  printf("The test data file is:   %s.\n\n", argv[2]);
  do
  {
    printf("Please enter the order of class attribute (1-%d): ", n_attr);
    scanf ("%d", &class_attr);
  } while(class_attr<1 || class_attr>n_attr);

  class_attr--;

  for(i=0; i<n_attr; i++)
  {
    p_attr_list=(Attr_list *)malloc(sizeof(Attr_list));
    p_attr_list->attr=i;
    p_attr_list->next=NULL;
    if(!attr_list)
      attr_list=p_attr_list;
    else
    {
      p_attr_list->next=attr_list;
      attr_list=p_attr_list;
    }
  }

  Build_C45(train_dat, &attr_list, f, &Node, &value, &return_type);

  if(!(fp=fopen(argv[3],"w")))
  {
    printf("Cannot create output file %s!\n", argv[3]);
    exit(0);
  }

  printf("\nUsing C4.5, the decision tree (the number in parentheses is the attribute order):\n");
  fprintf(fp, "Using C4.5, the decision tree (the number in parentheses is the attribute order):\n");
  if(return_type==VALUE)
  {
    printf("%s\n", value);
    fprintf(fp, "%s\n", value);
  }
  if(return_type==NODE)
  {
    for(i=0; i<128; i++)
      label[i]=0;
    Print_C45(fp, label, Node, 0);
  }

  n_total=n_correct=0;
  printf("\nUsing C4.5, the calculated class labels for the training data:\n");
  fprintf(fp, "\nUsing C4.5, the calculated class labels for the training data:\n");
  Calculate_Accuracy(fp, Node, test_dat, &n_total, &n_correct);
  printf("\nUsing C4.5, the accuracy for the test data is %.1f%% in %d records.\n", 100.0*(double)n_correct/(double)n_total, n_total);
  fprintf(fp, "\nUsing C4.5, the accuracy for the test data is %.1f%% in %d records.\n", 100.0*(double)n_correct/(double)n_total, n_total);

  n_total=n_correct=0;
  printf("\nUsing naive Bayesian classification, the calculated class labels for the training data:\n");
  fprintf(fp, "\nUsing naive Bayesian classification, the calculated class labels for the training data:\n");
  Naive_Bayesian(fp, train_dat, test_dat, n_attr, &n_total, &n_correct);
  printf("\nUsing naive Bayesian classification, the accuracy for the test data is %.1f%% in %d records.\n", 100.0*(double)n_correct/(double)n_total, n_total);
  fprintf(fp, "\nUsing naive Bayesian classification, the accuracy for the test data is %.1f%% in %d records.", 100.0*(double)n_correct/(double)n_total, n_total);

  fclose(fp);

  printf("\nResults saved in the file %s.\n\n", argv[3]);

  return 0;
}

void Print_C45(FILE *fp, int *label, Node_C45 *Node, int n_tab)
{
  int i;
  Child *p_child=NULL;

  p_child=Node->child;

  while(p_child)
  {
    for(i=0; i<n_tab; i++)
    {
      if(label[i])
      {
        printf("|");
        fprintf(fp, "|");
      }
      printf("\t");
      fprintf(fp, "\t");
    }
    if(p_child->type==VALUE)
    {
      printf("(%d) = %s: %s\n", Node->attr+1, p_child->attr_value, p_child->node.value);
      fprintf(fp, "(%d) = %s: %s\n", Node->attr+1, p_child->attr_value, p_child->node.value);
    }
    if(p_child->type==NODE)
    {
      printf("(%d) = %s\n", Node->attr+1, p_child->attr_value);
      fprintf(fp, "(%d) = %s\n", Node->attr+1, p_child->attr_value);
      if(p_child->next)
        label[n_tab]=1;
      Print_C45(fp, label, p_child->node.node, n_tab+1);
    }
    p_child=p_child->next;
  }
}

void Calculate_Accuracy(FILE *fp, Node_C45 *Node, Node_data *dat, int *n_total, int *n_correct)
{
  int r;
  char *value=NULL;
  while(dat)
  {
    (*n_total)++;
    r=Look_For_Class(Node, dat, &value);
    printf("%s ", value);
    fprintf(fp, "%s ", value);
    if(r)
      (*n_correct)++;
    dat=dat->next;
  }
}

int Look_For_Class(Node_C45 *Node, Node_data *dat, char **value)
{
  Child *p_child=Node->child;
  while(p_child)
  {
    if(strcmp(dat->value[Node->attr], p_child->attr_value)==0)
    {
      if(p_child->type==VALUE)
      {
        (*value)=(char *)malloc(strlen(p_child->node.value));
        strcpy(*value, p_child->node.value);
        if(strcmp(dat->value[class_attr], p_child->node.value)==0)
          return 1;
        else
          return 0;
      }
      if(p_child->type==NODE)
        return Look_For_Class(p_child->node.node, dat, value);
    }
    p_child=p_child->next;
  }
}

void Build_C45(Node_data *dat, Attr_list **attr_list, Filter *f, Node_C45 **Node, char **value, int *return_type)
{
  int splitting_attr;
  Node_C45 *Node_child=NULL;
  char *value_child=NULL;
  int return_type_child;
  Node_attr_value_class *splitting_attr_value_list=NULL;
  Filter *f_child=NULL;
  Child *p_child=NULL;
  Node_class *p_class_list=NULL;


  if(Is_Same_Value(value, dat, f))
  {
    *return_type=VALUE;
    return;
  }

  if(!(*attr_list)->next)
  {
    Get_Majority_Class(dat, f, value);
    *return_type=VALUE;
    return;
  }

  Attribute_Selection_C45(dat, f, *attr_list, &splitting_attr, &splitting_attr_value_list);

  *Node=(Node_C45 *)malloc(sizeof(Node_C45));
  *return_type=NODE;
  (*Node)->attr=splitting_attr;

  Update_attr_list(attr_list, splitting_attr);

  p_child=(Child *)malloc(sizeof(Child));
  p_child->next=NULL;
  (*Node)->child=p_child;

  while(splitting_attr_value_list)
  {
    strcpy(p_child->attr_value, splitting_attr_value_list->value);
    Build_Child_Filter(f, &f_child, splitting_attr, splitting_attr_value_list->value);

    if(Is_Data_Empty(dat, f_child))
    {
      Get_Majority_Class(dat, f, value);
      *return_type=VALUE;
    }
    else
      Build_C45(dat, attr_list, f_child, &Node_child, &value_child, &return_type_child);

    p_child->type=return_type_child;
    if(return_type_child==VALUE)
      strcpy(p_child->node.value, value_child);
    if(return_type_child==NODE)
      p_child->node.node=Node_child;

    Clear_Filter(&f_child);
    if(splitting_attr_value_list->next)
    {
      p_child->next=(Child *)malloc(sizeof(Child));
      p_child=p_child->next;
      p_child->next=NULL;
    }

    splitting_attr_value_list=splitting_attr_value_list->next;
  }
}

void Read_Filter(Filter *f)
{
  printf("Filter------------------------Start\n");
  while(f)
  {
    printf("attr=%d, value=%s\n", f->attr, f->value);
    f=f->next;
  }
  printf("Filter------------------------End\n");
}

void Clear_Filter(Filter **f)
{
  Filter *p_f=*f, *p_f_prev;
  while(p_f)
  {
    p_f_prev=p_f;
    p_f=p_f->next;
    free(p_f_prev);
  }
}

int Is_Data_Empty(Node_data *pdat, Filter *f)
{
  while(pdat)
  {
    if(Is_Selected_Tuple(pdat, f))
      return 0;
    pdat=pdat->next;
  }
  return 1;
}

void Build_Child_Filter(Filter *f, Filter **f_child, int splitting_attr, char *value)
{
  Filter *p_f=NULL;

  p_f=(Filter *)malloc(sizeof(Filter));
  *f_child=p_f;

  while(f)
  {
    p_f->attr=f->attr;
    strcpy(p_f->value, f->value);
    p_f->next=NULL;
    p_f->next=(Filter *)malloc(sizeof(Filter));
    p_f=p_f->next;
    f=f->next;
  }

  p_f->attr=splitting_attr;
  strcpy(p_f->value, value);
  p_f->next=NULL;
}

void Update_attr_list(Attr_list **attr_list, int splitting_attr)
{
  Attr_list *p_attr_list=*attr_list, *p_attr_list_prev=NULL;
  while(p_attr_list)
  {
    if(p_attr_list->attr==splitting_attr)
    {
      if(!p_attr_list_prev)
        *attr_list=p_attr_list->next;
      else
        p_attr_list_prev->next=p_attr_list->next;
      free(p_attr_list);
      break;
    }
    p_attr_list_prev=p_attr_list;
    p_attr_list=p_attr_list->next;
  }
}

void Attribute_Selection_C45(Node_data *dat, Filter *f, Attr_list *attr_list,
                             int *splitting_attr, Node_attr_value_class **splitting_attr_value_list)
{
  Node_data *pdat;
  Attr_list *p_attr_list;
  Node_class *class_list=NULL, *p_class_list=NULL;
  Node_attr_value_class *attr_value_class_list[MAX_ATTR], *p_attr_value_class_list;
  double sum=0;
  int i=0, n_attr_list=0;
  double p, Info_D_A_sub=0, Info_D=0, Info_D_A=0, Split_Info_A=0, *Gain_A, *Gain_Ratio_A, Mean_Gain_A=0;
  double Max_Gain_Ratio_A=0;

  //Calculated Info_D
  pdat=dat;
  while(pdat)
  {
    InsertClassList(&class_list, pdat->value[class_attr]);
    pdat=pdat->next;
  }

  p_class_list=class_list;
  while(p_class_list)
  {
    sum+=p_class_list->count;
    p_class_list=p_class_list->next;
  }

  p_class_list=class_list;
  while(p_class_list)
  {
    p=(double)p_class_list->count/(double)sum;
    Info_D+=-(p*(log(p)/log(2.0)));
    p_class_list=p_class_list->next;
  }

  //Calculated Info_D_A
  p_attr_list=attr_list;
  while(p_attr_list)
  {
    n_attr_list++;
    p_attr_list=p_attr_list->next;
  }

  Gain_A=(double *)malloc(sizeof(double)*n_attr_list);
  Gain_Ratio_A=(double *)malloc(sizeof(double)*n_attr_list);

  p_attr_list=attr_list;
  while(p_attr_list)
  {
    Gain_A[p_attr_list->attr]=Gain_Ratio_A[p_attr_list->attr]=0;
    p_attr_list=p_attr_list->next;
  }

  p_attr_list=attr_list;
  while(p_attr_list)
  {
    if(p_attr_list->attr==class_attr)
    {
      p_attr_list=p_attr_list->next;
      continue;
    }
    attr_value_class_list[p_attr_list->attr]=NULL;
    pdat=dat;
    Info_D_A=0;
    Split_Info_A=0;

    while(pdat)
    {
      if(Is_Selected_Tuple(pdat, f))
        InsertAttrValueList(&attr_value_class_list[p_attr_list->attr],
                            pdat->value[p_attr_list->attr],
                            pdat->value[class_attr]);
      pdat=pdat->next;
    }

    p_attr_value_class_list=attr_value_class_list[p_attr_list->attr];
    while(p_attr_value_class_list)
    {
      Info_D_A_sub=0;
      p=(double)(p_attr_value_class_list->count)/(double)sum;
      Split_Info_A+=-(p*(log(p)/log(2.0)));
      p_class_list=p_attr_value_class_list->class_list;
      while(p_class_list)
      {
        p=(double)(p_class_list->count)/(double)(p_attr_value_class_list->count);
        Info_D_A_sub+=-(p*(log(p)/log(2.0)));
        p_class_list=p_class_list->next;
      }
      Info_D_A_sub*=(double)(p_attr_value_class_list->count)/(double)sum;
      Info_D_A+=Info_D_A_sub;
      p_attr_value_class_list=p_attr_value_class_list->next;
    }

    Gain_A[p_attr_list->attr]=Info_D-Info_D_A;
    Gain_Ratio_A[p_attr_list->attr]=Gain_A[p_attr_list->attr]/Split_Info_A;
    p_attr_list=p_attr_list->next;
  }

  p_attr_list=attr_list;
  while(p_attr_list)
  {
    if(p_attr_list->attr!=class_attr)
      Mean_Gain_A+=Gain_A[p_attr_list->attr]/(n_attr_list-1);
    p_attr_list=p_attr_list->next;
  }

  p_attr_list=attr_list;
  while(p_attr_list)
  {
    if(Gain_A[p_attr_list->attr]>=Mean_Gain_A && p_attr_list->attr!=class_attr)
    {
      Max_Gain_Ratio_A=Gain_Ratio_A[p_attr_list->attr];
      break;
    }
    p_attr_list=p_attr_list->next;
  }

  p_attr_list=attr_list;
  while(p_attr_list)
  {
    if(Max_Gain_Ratio_A<Gain_Ratio_A[p_attr_list->attr] && Gain_A[p_attr_list->attr]>=Mean_Gain_A && p_attr_list->attr!=class_attr)
      Max_Gain_Ratio_A=Gain_Ratio_A[p_attr_list->attr];
    p_attr_list=p_attr_list->next;
  }

  p_attr_list=attr_list;
  while(p_attr_list)
  {
    if(Max_Gain_Ratio_A==Gain_Ratio_A[p_attr_list->attr])
    {
      *splitting_attr=p_attr_list->attr;
      break;
    }
    p_attr_list=p_attr_list->next;
  }

  *splitting_attr_value_list=attr_value_class_list[p_attr_list->attr];
}

int Read_Data(Node_data **dat, char *filename)
{
  FILE *fp;
  char buf[256];
  int n_attr;

  if((fp=fopen(filename,"r"))==NULL)
  {
    printf("File %s not exist!\n", filename);
    exit(0);
  }

  while(fgets (buf, 256, fp)!=NULL)
    n_attr=InsertData(dat, buf);

  fclose(fp);
  return n_attr;
}

int InsertData(Node_data **dat, char *s)
{
  int i=0;
  Node_data *pInsert;
  Node_data *pHead=*dat;

  char *p, *str, buf[256];
  const char *d = " \t\n";

  pInsert = (Node_data *)malloc(sizeof(Node_data));
  pInsert->next=NULL;

  strcpy(buf,s);
  p = strtok(buf,d);
  while(p)
  {
    str=(char *)malloc(strlen(p)+1);
    RemoveSpace(str, p);
    if(strlen(str)!=0)
    {
      strcpy(pInsert->value[i], str);
      i++;
    }
    free(str);
    p=strtok(NULL,d);
  }

  if(!pHead)
    *dat=pInsert;
  else
  {
    while(pHead->next)
      pHead=pHead->next;
    pHead->next=pInsert;
  }
  return i;
}

void RemoveSpace(char *s, char *p)
{
  char *p1=p;
  while(*p1!='\0')
  {
    if(IsLetter(*p1) || IsDigit(*p1))
    {
      *s=*p1;
      s++;
    }
    p1++;
  }
  *s='\0';
}

int IsLetter(char c) //Letter
{
  if(((c<='z')&&(c>='a'))||((c<='Z')&&(c>='A')))
    return 1;
  else
    return 0;
}

int IsDigit(char c) //Digit
{
  if(c>='0'&&c<='9')
    return 1;
  else
    return 0;
}

void InsertAttrValueList(Node_attr_value_class **pNode, char *attr_value, char *class_value)
{
  Node_attr_value_class *pInsert=NULL;
  Node_attr_value_class *pHead=NULL, *pHead_prev=NULL;
  Node_class *pClass=NULL;

  pHead = *pNode;

  pInsert = (Node_attr_value_class *)malloc(sizeof(Node_attr_value_class));
  pInsert->value = (char *)malloc(strlen(attr_value)+1);
  strcpy(pInsert->value, attr_value);
  pInsert->count=1;
  pInsert->next=NULL;

  InsertClassList(&pClass, class_value);
  pInsert->class_list=pClass;

  if(!pHead)
  {
    pHead = pInsert;
    *pNode=pHead;
  }
  else
  {
    while(pHead)
      if(strcmp(pHead->value, pInsert->value)==0)
      {
        (pHead->count)++;
        InsertClassList(&pHead->class_list, class_value);
        free(pInsert);
        if(pClass)
          free(pClass);
        return;
      }
      else
      {
        pHead_prev=pHead;
        pHead=pHead->next;
      }
    pHead_prev->next = pInsert;
  }
}

void InsertClassList(Node_class **pNode, char *value)
{
  int i;
  Node_class *pInsert=NULL;
  Node_class *pHead, *pHead_prev=NULL;

  pHead = *pNode;

  pInsert = (Node_class *)malloc(sizeof(Node_class));
  pInsert->value = (char *)malloc(strlen(value)+1);
  strcpy(pInsert->value, value);
  pInsert->count=1;
  pInsert->next=NULL;

  if(!pHead)
  {
    pHead = pInsert;
    *pNode=pHead;
  }
  else
  {
    while(pHead)
      if(strcmp(pHead->value, pInsert->value)==0)
      {
        (pHead->count)++;
        free(pInsert);
        return;
      }
      else
      {
        pHead_prev=pHead;
        pHead=pHead->next;
      }
    pHead_prev->next = pInsert;
  }
}

int Is_Same_Value(char **value, Node_data *dat, Filter *f)
{
  Node_data *pdat=dat;
  while(pdat)
    if(Is_Selected_Tuple(pdat, f))
    {
      *value=(char *)malloc(sizeof(pdat->value[class_attr])+1);
      strcpy(*value, pdat->value[class_attr]);
      break;
    }
    else
      pdat=pdat->next;

  pdat=dat;
  while(pdat)
  {
    if(Is_Selected_Tuple(pdat, f))
      if(strcmp(pdat->value[class_attr], *value)!=0)
      {
        free(*value);
        return 0;
      }
    pdat=pdat->next;
  }
  return 1;
}

int Is_Selected_Tuple(Node_data *pdat, Filter *f)
{
  while(f)
  {
    if(strcmp(pdat->value[f->attr], f->value)!=0)
      return 0;
    f=f->next;
  }
  return 1;
}

void Get_Majority_Class(Node_data *pdat, Filter *f, char **majority_class)
{
  Node_class *class_list=NULL, *class_list_head=NULL, *class_list_prev=NULL;
  int n_majority_class;

  while(pdat)
  {
    if(Is_Selected_Tuple(pdat, f))
      InsertClassList(&class_list, pdat->value[class_attr]);
    pdat=pdat->next;
  }

  class_list_head=class_list;

  n_majority_class=class_list->count;
  *majority_class=(char *)malloc(sizeof(class_list->value)+1);
  strcpy(*majority_class, class_list->value);
  while(class_list)
  {
    if(class_list->count>n_majority_class)
    {
      free(majority_class);
      *majority_class=(char *)malloc(sizeof(class_list->value)+1);
      strcpy(*majority_class, class_list->value);
      n_majority_class=class_list->count;
    }
    class_list=class_list->next;
  }

  class_list=class_list_head;
  while(class_list)
  {
    class_list_prev=class_list;
    class_list=class_list->next;
    free(class_list_prev);
  }
}

void Print_Class_List(Node_class *p)
{
  printf("----------------------------\n");
  while(p)
  {
    printf("%s:  %d\n", p->value, p->count);
    p=p->next;
  }
  printf("----------------------------\n");
}

void Print_Attr_List(Attr_list *p)
{
  printf("----------------------------\n");
  while(p)
  {
    printf("%d\n", p->attr);
    p=p->next;
  }
  printf("----------------------------\n");
}

void Print_attr_value_class(Node_attr_value_class *p)
{
  Node_class *p2=NULL;
  printf("----------------------------\n");
  while(p)
  {
    printf("attr value=%s, count=%d\n", p->value, p->count);
    p2=p->class_list;
    while(p2)
    {
      printf("class value=%s, count=%d\n", p2->value, p2->count);
      p2=p2->next;
    }
    p=p->next;
  }
  printf("----------------------------\n");
}

void Naive_Bayesian(FILE *fp, Node_data *p_train, Node_data *p_test, int n_attr, int *n_total, int *n_correct)
{
  int i, n_i;
  Node_attr_value_class *attr_class_value_list[MAX_ATTR], *p_attr_class_value_list;
  Node_class *p_value_list=NULL;
  Node_PXCi_PCi *PXCi_PCi=NULL, *p_PXCi_PCi=NULL;
  double n_Ci=0;
  char *value=NULL;

  for(i=0; i<n_attr; i++)
    attr_class_value_list[i]=NULL;

  while(p_train)
  {
    for(i=0; i<n_attr; i++)
    {
      if(i!=class_attr)
      {
        n_i=i;
        InsertAttrValueList(&attr_class_value_list[i],
                            p_train->value[class_attr],
                            p_train->value[i]);
      }
    }
    p_train=p_train->next;
  }

  p_attr_class_value_list=attr_class_value_list[n_i];
  while(p_attr_class_value_list)
  {
    n_Ci+=p_attr_class_value_list->count;
    p_attr_class_value_list=p_attr_class_value_list->next;
  }

  while(p_test)
  {
    (*n_total)++;
    Create_PXCi_PCi(&PXCi_PCi, attr_class_value_list[n_i], n_Ci);


    for(i=0; i<n_attr; i++)
      if(i!=class_attr)
      {
        p_attr_class_value_list=attr_class_value_list[i];
        while(p_attr_class_value_list)
        {
          p_PXCi_PCi=PXCi_PCi;
          while(p_PXCi_PCi)
            if(strcmp(p_PXCi_PCi->value, p_attr_class_value_list->value)==0)
            {

              p_value_list=p_attr_class_value_list->class_list;
              while(p_value_list)
              {
                if(strcmp(p_value_list->value, p_test->value[i])==0)
                {
                  (p_PXCi_PCi->P_value)*=(double)(1.0+p_value_list->count)/(double)(1.0+p_attr_class_value_list->count);
                  break;
                }
                else
                  p_value_list=p_value_list->next;
              }
              break;
            }
            else
              p_PXCi_PCi=p_PXCi_PCi->next;
          p_attr_class_value_list=p_attr_class_value_list->next;
        }
      }

    Look_For_Ci(PXCi_PCi, &value);
    printf("%s ", value);
    fprintf(fp, "%s ", value);

    if(strcmp(p_test->value[class_attr], value)==0)
      (*n_correct)++;

    Clear_PXCi_PCi(&p_PXCi_PCi);
    p_test=p_test->next;
  }
}

void Create_PXCi_PCi(Node_PXCi_PCi **Node, Node_attr_value_class *p_attr_class_value_list, double n_Ci)
{
  Node_PXCi_PCi *p_PXCi_PCi=(Node_PXCi_PCi *)malloc(sizeof(Node_PXCi_PCi));
  *Node=p_PXCi_PCi;
  while(p_attr_class_value_list)
  {
    strcpy(p_PXCi_PCi->value, p_attr_class_value_list->value);
    p_PXCi_PCi->P_value=(double)(p_attr_class_value_list->count)/n_Ci;
    if(p_attr_class_value_list->next)
    {
      p_PXCi_PCi->next=(Node_PXCi_PCi *)malloc(sizeof(Node_PXCi_PCi));
      p_PXCi_PCi=p_PXCi_PCi->next;
      p_PXCi_PCi->next=NULL;
    }
    p_attr_class_value_list=p_attr_class_value_list->next;
  }
}

void Clear_PXCi_PCi(Node_PXCi_PCi **Node)
{
  Node_PXCi_PCi *p=*Node, *p2=NULL;
  while(p)
  {
    p2=p;
    p=p->next;
    free(p2);
  }
  *Node=NULL;
}

void Look_For_Ci(Node_PXCi_PCi *Node, char **value)
{
  double max=0.0;
  Node_PXCi_PCi *p=Node;

  if(p)
    max=p->P_value;
  while(p)
  {
    if(max<p->P_value)
      max=p->P_value;
    p=p->next;
  }

  p=Node;
  while(p)
  {
    if(p->P_value==max)
    {
      *value=(char *)malloc(strlen(p->value)+1);
      strcpy(*value, p->value);
      break;
    }
    else
      p=p->next;
  }
}

void Print_PXCi_PCi(Node_PXCi_PCi *p)
{
  printf("--------------\n");
  while(p)
  {
    printf("%s %f\n", p->value, 1e6*p->P_value);
    p=p->next;
  }
  printf("--------------\n");
}	