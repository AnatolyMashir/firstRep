#include <stdio.h>
#include <stdlib.h>

struct node { int data;
                      struct node * next; };

int mergeSort(int* buf, long int start, long int size)
{
	if(size>1)
	{
		int size1,size2;
		size2 = size/2; 
		size1 = size - size2;
		if(mergeSort(buf, start,size1) || mergeSort(buf, start+size1,size2))
		{
			return 1;
		}
		int *tmp = malloc(size*sizeof(int));
		if(tmp==NULL)
		{
			fprintf (stderr, "Хозяина, памяти мала\n");
			return 1;
		}
		long int i=0,j=0,k=0;
		while(i!=size1 && j!=size2)
		{
			if(buf[start+i]<buf[start+size1+j])
			{
				tmp[k++] = buf[start+i++];
			}
			else
			{
				tmp[k++] = buf[start+size1+j++];
			}
		}
		while(i!=size1)
		{
			tmp[k++] = buf[start+i++];
		}
		while(j!=size2)
		{
			tmp[k++] = buf[start+size1+j++];
		}
		for(i=0; i<size; i++)
		{
			buf[start+i] = tmp[i];
		}
		free(tmp);
	}
	return 0;
}

void del(struct node *el)
{
	if(el->next!=NULL)
	{
		del(el->next);
	}
	free(el);
}
		  
int main(argc, argv)
int argc;
char *argv[];
{
    int value;
    if(argc<3)
    {
        fprintf (stderr, "Недостаточно аргументов\n");
        return 1;
    }
    FILE *files[argc-1];
    FILE *stream;
    int i;
    if ((stream=fopen(argv[argc-1], "w"))==NULL)
    {
        fprintf (stderr, "%s не может открыть файл %s\n",
        argv[0], argv[argc-1]);
        return 1;
    }
    files[0] = stream;
    for(i=1; i<argc-1; i++)
    {
        if ((stream=fopen(argv[i], "r"))==NULL)
        {
            fprintf (stderr, "%s не может открыть файл %s\n",
            argv[0], argv[i]);
            fcloseall();
            return 1;
        }
		files[i] = stream;
    }
    struct node first;
    first.data = 11111;
    first.next = NULL;
    struct node *cur = &first;
    long int count = 0;
    for(i=1; i<argc-1; i++)
    {
        stream = files[i];
	int err = err = fscanf(stream,"%d", &value);
        while(err!=0 && err!=EOF)
        {
		struct node *tmp = malloc(sizeof(struct node));
		count++;
		if(tmp==NULL)
		{
			fprintf (stderr, "Хозяина, памяти мала\n");
			fcloseall();
			return 1;
		}
		tmp->data = value;
		tmp->next = NULL;
		cur->next = tmp;
		cur = cur->next;
		err = fscanf(stream,"%d", &value);
        }
	if(err==0)
	{
	    fprintf (stderr, "Недопустимые символы в файле %s\n",
	    argv[i]);
	    return 1;
	}
    }
    int *buf = malloc(count*sizeof(int *));
    if(buf==NULL)
    {
	fprintf (stderr, "Хозяина, памяти мала\n");
	fcloseall();
	return 1;
    }
    long int j = 0;
    cur = first.next;
    while(cur!=NULL)
    {
	buf[j] = cur->data;
	cur = cur->next;
	if(cur!=NULL)
	{
		j++;
	}
    }
    del(first.next);
    if(!mergeSort(buf, 0, j+1))
    {
    	stream = files[0];
    	for(i=0; i<=j; i++)
    	{
		fprintf(stream,"%d\n", buf[i]);
	}
    }
    else
    {//not enough memory to merge
        fprintf (stderr, "Хозяина, памяти мала\n");
        free(buf);
        fcloseall();
        return 1; // error signal out
    }
    free(buf);
    fcloseall();
    return 0;
}
