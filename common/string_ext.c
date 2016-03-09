#include "string.h"
#include "stdlib.h"

char** split(char * string, int *q, char delim) {
	char ** result;
		//printf("\naddr result = '%d'\n", (int)result);
	int len = strlen(string);
	int count = 0;
	for (int i = 0; i < len; i++)
	{
		if(string[i] == delim) count++;
	}
	
	if(count == 0) {
		result = (char**)kmalloc(1);
		result[0] = (char*)kmalloc(len);
		//memcpy(result[0], string);
		
		result[0] = string;
		//printf("\n\"%s\"\n", result[0]);
		*q = 1;
		return result;
	}
	
	if(string[len-1] == delim) {
		//printf("Last is delim\n");
		count -= 1;
	}
	int * size = (int*)kmalloc((count+1)*sizeof(int));
	
	int indx = 0, s = 0, last = 0;
	for (int i = 0; i < len; i++)
	{
		if(string[i] == delim) {
			size[indx] = s;
			last = i;
			//printf("\nI %d: %s\n", size[indx], result[indx]);
			//printf("\nI %d\n", size[indx]);
			++indx;
			s = 0;
		}
		else ++s;
	}
	if(string[len-1] != delim) { 
		size[indx] = len - (last+1);
		//printf("\nI %d\n", size[indx]);
		//printf("Not Delim\n");
	}
	result = (char**)kmalloc(count+1);
	
	
	indx = 0;
	s = 0;
	result[0] = (char*)kmalloc(size[0]);
	for (int i = 0; i < len; i++)
	{
		result[indx][s] = string[i];
		if(string[i] == delim) {
			///printf("\nR %d: %s\n", size[indx], result[indx]);
			result[indx][size[indx]] = '\0';
			++indx;
			result[indx] = (char*)kmalloc(size[indx]+1);
			s = 0;
		}
		else ++s;
	}
	if(string[len-1] != delim) { 
		result[indx][s] = '\0';
		//printf("\nI %d\n", size[indx]);
		//printf("Not Delim\n");
	}
	
	kfree(size);
	//printf("\naddr result2 = '%d'\n", (int)result);
	*q = count+1;
	return result;
}

char *strcpy(char* str1, const char*str2) {
    int len = strlen(str2);
    for(int i = 0; i < len; i++) {
        str1[i] = str2[i];
    }
    str1[len] = '\0';
}


char *strcat(char *dest, const char *src)
{
     size_t offset = strlen(dest);
     strcpy(dest+offset, src);
     return dest;
}

void *memmem(const void *buf1, size_t size1, const void *buf2, size_t size2) {
  for(; size1 >= size2; size1--, buf1++)
    if (!memcmp(buf1, buf2, size2))
      return (void*)buf1;
  return NULL;
}

int next;
char qbuf[8];

char *itoa(n)
{
    register int r, k;
    int flag = 0;

    next = 0;
    if (n < 0) {
        qbuf[next++] = '-';
        n = -n;
    }
    if (n == 0) {
        qbuf[next++] = '0';
    } else {
        k = 10000;
        while (k > 0) {
            r = n / k;
            if (flag || r > 0) {
                 qbuf[next++] = '0' + r;
                  flag = 1;
            }
            n -= r * k;
            k = k / 10;
        }
    }
    qbuf[next] = 0;
    return(qbuf);
}



