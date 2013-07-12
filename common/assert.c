 /*
  * assert.c - diagnostics
  */
 /* $Header: /opt/proj/minix/cvsroot/src/lib/ansi/assert.c,v 1.1.1.1 2005/04/21 14:56:04 beng Exp $ */
 
 #include        "assert.h"
 #include "printf.h"
 #include "screen.h"
 
 void bad_assertion(char *file, int line, char *mess) {
 		
	enum colors col = get_color();
	set_color(Red);
    printf("\n*******************");
    printf("\nASSERTATION FAILED:\n");
    printf(mess);
    printf("\n*******************\n");
	set_color(col);
 }
