/*The extension to be considered
is printing the list to a file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct ladder{
  struct ladder *parent;
  int depth;
  int index;
  int used;
  char *s;
  struct ladder *next;
};
typedef struct ladder Ladder;

/*print list to a file*/
void filePrintList(Ladder *l,FILE *out);
/*Goes through the file and returns the length of the largest word*/
int findMaxLength(char *fileN);
/*Returns a string with the word the user types in */
char *userInput();
/*Takes input from file and builds a list using AllocateNode*/
int importList(char *fileN, Ladder *l, int m);
/*Returns a node to the end of the list*/
Ladder *AllocateNode(char *s, int m);
/*Print list to a file*/
int inputCheck(char *w1, char *w2, Ladder *start);
/*Checks if words are in the list - returns 1 if true*/
int check(char *w1, Ladder *l);
/*Marks input word's children*/
int mark(char *w1, char *w2,Ladder *l,Ladder *c);
/*Returns editing distance of two words*/
int fEditDist(char *t1, char *t2);
/*Returns a pointer to the struct that contains char w1*/
Ladder *searchWord(char *w1,Ladder *l);
/*Returns a pointer that contains char w1 and of a cirtain depth*/
Ladder *searchDepth(int d,Ladder *l);
/*print list from child to parent*/
void parentPrintList(Ladder *l);
/*freeing everything*/
void freedom(char *w1, char *w2, Ladder *l);

int main (int argc, char **argv)
{
  int i=1,r,maxWordLength;
  char *fileN;
  char *w1=NULL,*w2=NULL;
  FILE *dict, *output;
  Ladder *start=NULL, *current=NULL;
  if(argc==2){
    fileN=argv[1];
    dict=fopen(fileN,"r");
    if(dict==NULL){
      printf("Incorrect file name\n");
      return 1;
    }
    fclose(dict);
  }
  else{
    printf("File name not indicated\n");
    return 1;
  }
  maxWordLength = findMaxLength(fileN);
  /*Allocating a node to start so a list can be build*/
  start=AllocateNode(" ",maxWordLength);
  if(importList(fileN,start,maxWordLength)!=0){
    return -1;
  }
  fprintf(stderr,"Source word: ");
  w1=userInput();
  fprintf(stderr,"Target word: ");
  w2=userInput();
  assert(inputCheck(w1,w2,start));
  current=searchWord(w1,start);
  r=mark(w1,w2,start,current);
  while(current!=NULL && r){
    for(i=1;r;){
      current=searchDepth(i,start);
      if(current!=NULL){
        r=mark(current->s,w2,start,current);
      }
      /*if the ladder doesn't reach the target word
      it goes to the next value of depth*/
      if(r==-1&&i<100)
      i++;
    }
  }
  output=fopen("ladder.log","w");
  filePrintList(start,output);
  fclose(output);
  freedom(w1,w2,start);
  return 0;
}
char *userInput(int m)
{
  char *w1;
  w1=(char *)malloc(m);
  assert(fscanf(stdin,"%s",w1 ));
  if(w1==NULL){
    printf("w1=null\n");
    return 0;
  }

  return w1;
}
int findMaxLength(char *fileN)
{
  unsigned int max=0,i=0;
  char c;
  FILE *dictionary;
  dictionary = fopen(fileN,"r");
  while((c = fgetc(dictionary)) && c != EOF){
    if (c!='\n'){
      i++;
    }
    else{
      if(max<i){
        max=i;
      }
        i=0;
    }
  }
  return max;
}
void parentPrintList(Ladder *l)
{
  printf("%s ",l->s );
  l=l->parent;
  do{
    printf("-> %s ", l->s);
    l = l ->parent;
  }while(l!= NULL);
  printf("\n");
}
Ladder *searchWord(char *w1,Ladder *l)
{
  while(strcmp(w1,l->s)){
    l=l->next;
  }
  return l;
}
Ladder *searchDepth(int d,Ladder *l)
{
  while( l->used != 0 || (l->depth == 0||l->depth<d-1)){
    if(l->next==NULL){
      return NULL;
    }
    else{
      l=l->next;
    }
  }
  return l;
}
int mark( char *w1, char*w2, Ladder *l,Ladder *c)
{
  int cnt=0;
  c->used=1;
  while(l!=NULL){
   if(l->used==0 && strlen(w1)==strlen(l->s) && fEditDist(l->s,c->s)==1){
   l->depth=c->depth+1;
   l->parent=c;
   cnt--;
   if(!strcmp(l->s, w2)){
     parentPrintList(l);
    return 0;
  }
   }
   l=l->next;
  }
  if(cnt==0){
  return -1;
  }
  else{ return cnt-1;}
}
int fEditDist(char *t1, char *t2)
{
  unsigned int i;
  int eDist=0;
  for(i=0;i<strlen(t1);i++){
    if(t1[i]!=t2[i]){
      eDist++;
    }

  }
  return eDist;
}
int importList(char *fileN, Ladder *l, int m)
{
  int i=1;
  char *buffer;
  FILE *dictionary;
  dictionary = fopen(fileN,"r");
  buffer=(char *)malloc(m);
  assert(fscanf(dictionary,"%s",buffer));
  strcpy(l->s,buffer);
  l->index=i;
  while(fscanf(dictionary,"%s",buffer)==1){
    l->next=AllocateNode(buffer,m);
    l=l->next;
  i++;
  }
  printf("%d words read\n",i );
  free(buffer);
  return 0;
}
int inputCheck(char *w1, char *w2, Ladder *start)
{
  if(strlen(w1)!=strlen(w2)){
    printf("The two words must have the same length\n");
    return -1;
  }
  if (check(w1,start)!=1){
    return -1;
  }
  if (check(w2,start)!=1){
    return -1;
  }
  return 1;
}
Ladder *AllocateNode(char *s, int m)
{
   Ladder *p;
   p = (Ladder *)malloc(sizeof(Ladder));
   if(p==NULL){
      printf("Cannot Allocate Node\n");
      return 0;
   }
/*It allocates the largest string's lenght plus 2, for safety*/
   p->s=(char *)malloc(m+2);
   if (p->s==NULL){
     printf("NULL\n" );
     return 0;
   }
   strcpy(p->s, s);
   p->depth  = 0;
   p->used   = 0;
   p->next   = NULL;
   p->parent = NULL;
   return p;
}
int check(char *w1, Ladder *l)
{
  while(l!=NULL){
    if(strcmp(w1,l->s)==0){
      return 1;
    }
  l=l->next;
  }
  printf("%s is not in the dictionary\n",w1 );
  return 0;
}
void filePrintList(Ladder *l,FILE *out)
{
  int i=1;
  fprintf(stderr,"file printing\n");
  do{
    fprintf(out,"Word %6d : %10s, used: %d, depth %3d\n",i, l->s,l->used,l->depth);
    if(l->parent!=NULL){
      fprintf(out, "parent: %s\n",l->parent->s );
    }
    else{fprintf(out,"\n");}
    l = l ->next;
    i++;
  }while(l != NULL);
  fprintf(out,"END OF LIST\n");
}
void freedom(char *w1, char *w2, Ladder *l)
{
  free(w1);
  free(w2);
  do{
    free(l->s);
    free (l);
    l=l->next;
  }while(l!=NULL);
}
