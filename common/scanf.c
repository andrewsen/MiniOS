
#include "types.h"
#include "scanf.h"
#include "printf.h"
#include "keyboard.h"

int scanf(char * chars) {
	char ch;
	int i = 0;
	while((ch = keyboard_getchar()) != '\n') {
		
		if(ch == 0x08) {
			--i;
			printf("%c", ch);
		}
		else {
			chars[i] = ch;
			printf("%c", ch);
			++i;
		}
	}
    printf("\n");
	chars[i] = '\0';
	return i-1;
}
