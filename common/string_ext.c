#include "string.h"
#include "stdlib.h"

char** strspl(char * string, int *q, char delim)
{
    int len = strlen(string)+1;
    int dup = 0;
    *q = 0;
    for(int i = 0; i < len; ++i)
    {
        if(string[i] == delim)
        {
            if(!dup)
                ++*q;
            else
                dup = true;
        }
        dup = false;
    }

    if(*q == 0)
    {
        char **result = (char**)kmalloc(sizeof(char**));
        result[0] = (char*)kmalloc(len);
        memcpy(result[0], string, len);
        //printf("null str {%d:%s}\n", len, result[0]);
        return result;
    }

    ++*q;

    char ** result = (char**)kmalloc(*q * sizeof(char**));
    int idx = 0;
    int s_start = 0;
    dup = 0;
    //printf("str: %d\n", *q);
    for(int i = 0; i < len; ++i)
    {
        if(string[i] == delim || string[i] == '\0')
        {
            if(!dup)
            {
                result[idx] = (char*)calloc(i - s_start + 1, 1);
                memcpy(result[idx], string+s_start, i - s_start);
                //printf("'%s : %d'\n", result, i - s_start + 1);
                ++idx;
            }
            dup = true;
            s_start = i+1;
        }
        //else if(dup && string[i] != delim)
        //{
        //    s_start = i-1;
        //}
        else
            dup = false;
    }

    return result;
}

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
char qbuf[32];

char *itoa(int n)
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
    return qbuf;
}

double atof(const char *str)
{
    char *s = str;
    double a = 0.0;
    int e = 0;
    int c;
    int sign = 1;
    if(*s == '-')
    {
        s++;
        sign = -1;
    }
    while ((c = *s++) != '\0' && isdigit(c)) {
        a = a*10.0 + (c - '0');
    }
    if (c == '.') {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a*10.0 + (c - '0');
            e = e-1;
        }
    }
    if (c == 'e' || c == 'E') {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+')
            c = *s++;
        else if (c == '-') {
            c = *s++;
            sign = -1;
        }
        while (isdigit(c)) {
            i = i*10 + (c - '0');
            c = *s++;
        }
        e += i*sign;
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    return sign*a;
}

char *lltoa(int64_t val)
{
    next = 0;
    if(val < 0)
    {
        qbuf[next++] = '-';
        val = -val;
    }
    uint start = next;
    while(val)
    {
        qbuf[next++] = '0' + val % 10;
        val /= 10;
    }
    uint len = next-start;
    for(int i = 0; i < len/2; ++i)
    {
        int idx = start+len-i-1;
        char ch = qbuf[start+i];
        qbuf[start+i] = qbuf[idx];
        qbuf[idx] = ch;
    }
    qbuf[next++] = '\0';
    return qbuf;
}

char *dtoa(double d)
{
    next = 0;
    uint64_t draft = *(uint64_t*)(&d);

    if(draft >> 63)
    {
        qbuf[next++] = '-';
    }

    int e = (draft >> 52) & 0b11111111111;
    if(e == 0)
    {
        qbuf[next++] = '0';
        qbuf[next++] = '\0';
        return qbuf;
    }
    if(e == 0b11111111111)
    {
        strcpy(qbuf, "inf/nan");
        return qbuf;
    }
    int E = e - 1023;
    if(E < 0)
    {
        qbuf[next++] = '0';
        qbuf[next++] = '.';
        uint64_t m = (0x10000000000000 | (draft & 0xFFFFFFFFFFFFF));
        //printf("M = %X\n", m);
        //m >>= (-E+1);
        int divi = 1 << -E;
        m /= divi;
        for(int c = 0; m && c < 15; ++c)
        {
            m *= 10;
            uint digit = (int)((m >> 52) & 0b1111);
            qbuf[next++] = '0' + digit;
            m &= 0xFFFFFFFFFFFFF;
        }
    }
    else if(E == 0)
    {
        qbuf[next++] = '1';
        qbuf[next++] = '.';
        uint64_t m = draft & 0xFFFFFFFFFFFFF;
        for(int c = 0; m && c < 15; ++c)
        {
            m *= 10;
            uint digit = (int)((m >> 52) & 0b1111);
            qbuf[next++] = '0' + digit;
            m &= 0xFFFFFFFFFFFFF;
        }
    }
    else
    {
        //E++;
        uint64_t mask = 0xFFFFFFFFFFFFF >> E;
        uint shift = 53 - E;
        uint64_t int_part =  ((0x10000000000000 | (draft & 0xFFFFFFFFFFFFF)) >> (shift-1));
        uint64_t fraction_part = draft & mask;
        int cur_next = next;
        int icount = 0;
        while(int_part)
        {
            qbuf[next++] = '0' + int_part % 10;
            int_part /= 10;
            ++icount;
        }
        for(int i = 0; i < icount/2; ++i)
        {
            char tmp = qbuf[cur_next+icount-i-1];
            qbuf[cur_next+icount-i-1] = qbuf[cur_next+i];
            qbuf[cur_next+i] = tmp;
        }

        if(fraction_part == 0)
        {
            qbuf[next++] = '\0';
            return qbuf;
        }
        qbuf[next++] = '.';
        for(int c = 0; fraction_part && c < 15; ++c)
        {
            fraction_part *= 10;
            uint digit = (int)((fraction_part >> (shift-1)) & 0b1111);
            qbuf[next++] = '0' + digit;
            fraction_part &= mask;
        }
    }
    qbuf[next++] = '\0';
    return qbuf;
}

char *ftoa(float f)
{
    next = 0;
    uint32_t draft = *(uint32_t*)(&f);

    if(draft >> 31)
    {
        qbuf[next++] = '-';
    }

    int e = (draft >> 23) & 0b11111111;
    if(e == 0)
    {
        qbuf[next++] = '0';
        qbuf[next++] = '\0';
        return qbuf;
    }
    if(e == 0b11111111)
    {
        strcpy(qbuf, "inf/nan");
        return qbuf;
    }
    int E = e - 127;
    if(E < 0)
    {
        qbuf[next++] = '0';
        qbuf[next++] = '.';
        uint64_t m = (0x800000 | (draft & 0x7FFFFF));
        //printf("M = %X\n", m);
        //m >>= (-E+1);
        int divi = 1 << -E;
        m /= divi;
        for(int c = 0; m && c < 6; ++c)
        {
            m *= 10;
            uint digit = (int)((m >> 23) & 0b1111);
            qbuf[next++] = '0' + digit;
            m &= 0x7FFFFF;
        }
    }
    else if(E == 0)
    {
        qbuf[next++] = '1';
        qbuf[next++] = '.';
        uint64_t m = draft & 0x7FFFFF;
        for(int c = 0; m && c < 6; ++c)
        {
            m *= 10;
            uint digit = (int)((m >> 23) & 0b1111);
            qbuf[next++] = '0' + digit;
            m &= 0x7FFFFF;
        }
    }
    else
    {
        //E++;
        uint64_t mask = 0x7FFFFF >> E;
        uint shift = 24 - E;
        uint64_t int_part =  ((0x800000 | (draft & 0x7FFFFF)) >> (shift-1));
        uint64_t fraction_part = draft & mask;
        int cur_next = next;
        int icount = 0;
        while(int_part)
        {
            qbuf[next++] = '0' + int_part % 10;
            int_part /= 10;
            ++icount;
        }
        for(int i = 0; i < icount/2; ++i)
        {
            char tmp = qbuf[cur_next+icount-i-1];
            qbuf[cur_next+icount-i-1] = qbuf[cur_next+i];
            qbuf[cur_next+i] = tmp;
        }

        if(fraction_part == 0)
        {
            qbuf[next++] = '\0';
            return qbuf;
        }
        qbuf[next++] = '.';
        for(int c = 0; fraction_part && c < 6; ++c)
        {
            fraction_part *= 10;
            uint digit = (int)((fraction_part >> (shift-1)) & 0b1111);
            qbuf[next++] = '0' + digit;
            fraction_part &= mask;
        }
    }
    qbuf[next++] = '\0';
    return qbuf;
}

int atoi(const char *val)
{
    int sign = 1;
    int res = 0;
    if(val[0] == '-')
    {
        sign = -1;
        ++val;
    }
    //int len = strlen(val);
    while(*val)
    {
        res *= 10;
        res += *val - '0';
        ++val;
    }
    return res*sign;
}

int hatoi(const char *val)
{
    int res = 0;
    while(*val)
    {
        int digit = 0;
        if(*val >= '0' && *val <= '9')
            digit = *val - '0';
        else if(*val >= 'A' && *val <= 'F')
            digit = 10 + *val - 'A';
        else if(*val >= 'a' && *val <= 'f')
            digit = 10 + *val - 'a';
        else
            break;
        ++val;

        res <<= 4;
        res |= digit;
    }
    return res;
}

long long int atoll(const char *val)
{
    long long int sign = 1;
    long long int res = 0;
    if(val[0] == '-')
    {
        sign = -1;
        ++val;
    }
    //int len = strlen(val);
    while(*val)
    {
        res *= 10;
        res += *val - '0';
        ++val;
    }
    return res*sign;
}

int isdigit(char ch)
{
    return ch >= '0' && ch <= '9';
}


int abs(int x)
{
    return x > 0 ? -x : x;
}
