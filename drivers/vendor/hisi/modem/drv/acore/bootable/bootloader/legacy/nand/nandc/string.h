#ifndef __BSP_STRING_LIB_H__
#define __BSP_STRING_LIB_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef EOS
#define EOS		'\0'
#endif

#ifndef FAST
#define FAST	register
#endif

#define ALIGNMENT	3		/* quad byte alignment mask */

char itoa(int i);


void hextoa(unsigned int hex,char *str);

void dectoa(unsigned int dec,char *str);

int strcmp
(
    const char * s1,   /* string to compare */
    const char * s2    /* string to compare <s1> to */
);

char * strcpy
(
    char *       s1,	/* string to copy to */
    const char * s2     /* string to copy from */
);

char * strcat
(
    char *       destination, /* string to be appended to */
    const char * append       /* string to append to <destination> */
);

void bcopy
(
    const char *source,       	/* pointer to source buffer      */
    char *destination,  	/* pointer to destination buffer */
    int nbytes          	/* number of bytes to copy       */
);

void bfill
(
    FAST char *buf,           /* pointer to buffer              */
    int nbytes,               /* number of bytes to fill        */
    FAST int ch     	      /* char with which to fill buffer */
);


#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_GLOBAL_H_*/
