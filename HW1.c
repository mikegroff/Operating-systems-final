// Michael Groff
// COP 4600

#include <stdio.h>
#include <stdlib.h>

typedef struct process
{
  char pname[];
  int arrival;
  int burst;
  int start;
  int end;
  int tleft;

}process;

typedef struct tstep
{
   char arrived[];
   char finished[];
   char selected[];
}tstep;

typedef struct pFile
{
  process pList[];
  int totTime, pCount, quantime;
  char saType[];

}pfile;

tstep[] fcfs(process plist[],int n)
{

}

void swap(process *xp, process *yp)
{
    process temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void selectionSortf(process arr[], int n)
{
    int i, j, min_idx;

    for (i = 0; i < n-1; i++)
    {
        min_idx = i;
        for (j = i+1; j < n; j++)
          if (arr[j].arrival < arr[min_idx].arrival)
            min_idx = j;

        swap(&arr[min_idx], &arr[i]);
    }
}

void selectionSorts(process arr[], int n)
{
    int i, j, min_idx;

    for (i = 0; i < n-1; i++)
    {
        min_idx = i;
        for (j = i+1; j < n; j++)
          if (arr[j].burst < arr[min_idx].burst)
            min_idx = j;

        swap(&arr[min_idx], &arr[i]);
    }
}

pFile scanIn(void)
{
    int end = 0, k = 0;
    char phold[];
    pFile newPFile;
    FILE *f = fopen("processes.in", "r")

    while(end == 0)
    {
      fscanf(f,"%s" phold);
      if(strcmp(phold,"processcount") == 0)
        fscanf(f, "%d", newPFile.pcount);
      else if(strcmp(phold,"runfor") == 0)
        fscanf(f, "%d", newPFile.totTime);
      else if(strcmp(phold,"use") == 0)
        fscanf(f, "%s", newPFile.saType);
      else if(strcmp(phold,"quantum") == 0)
        fscanf(f, "%d", newPFile.quantime);
      else if(strcmp(phold,"process") == 0)
      {
        fscanf(f, "%s", phold);
        if(strcmp(phold,"name") == 0)
        {
          fscanf(f, "%s", newPFile.pList[k].pname);
          fscanf(f, "%s", phold);
            if(strcmp(phold,"arrival") == 0)
            {
              fscanf(f, "%d", newPFile.pList[k].arrival);
              if(strcmp(phold,"burst") == 0)
              {
                fscanf(f, "%d", newPFile.pList[k].burst);
                newPFile.phold[k].tleft = newPFile.pList[k].burst;
                k++;
              }
            }
        }
      }
      else if(strcmp(phold,"end") == 0)
        end = 1;
    }
   return newPFile;
}



int main(void)
{
    tstep timeSteps[];
    pFile pIn = scanIn();
    if(strcmp(pIn.saType,"sjf") == 0)
    {
      selectionSorts(pIn.pList,pCount);
    }
    else if(strcmp(pIn.saType,"rr") == 0)
    {
      selectionSortf(pIn.pList,pCount);
    }
    else if(strcmp(pIn.saType,"fcfs") == 0)
    {
      selectionSortf(pIn.pList,pCount);
      timeSteps = fcfs(pIn.pList);
    }

}
