/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_GLOBAL.h
*
*   作    者 :  wuzhen
*
*   描    述 :  公共定义头文件
*
*   修改记录 :  2009年3月12日  v1.00  wuzhen      创建
*              2009年3月25日  v2.00  wangxuesong 修改
*              2009年06月09日 v2.01  liumengcun 修改
*  BJ9D01075   修改BSP_ASSERT和BSP_TRACE实现
*************************************************************************/
/*lint -save -e* */
#ifdef __cplusplus
extern "C"
{
#endif

#include "string.h"

char itoa(int i)
{
    if(i >= 0 && i <= 9)
        return i + '0';
    else if(i >= 0xA && i <= 0xF) 
        return i -0xA + 'A';
    else
        return '0';
}

int strlen
(
    const char * s        /* string */
)
{
    const char *save = s + 1;

    while (*s++ != EOS)
    ;

    return (s - save);
}

void hextoa(unsigned int hex,char *str)
{
#define MAX_STR_LEN 32
    unsigned int tmp = hex;
    char tmpStr[MAX_STR_LEN] = "";
    int i = 0,j = 0;
    int minStrLen = 0;

    do
    {
        tmpStr[i] = itoa(tmp&0xf);
        tmp >>= 4;
        i++;
    }while(tmp&&(i<MAX_STR_LEN));
    
    tmpStr[i] = EOS;

    minStrLen = strlen(str)>i?i:strlen(str);

    for(j=0;j<minStrLen;j++)
    {
        str[j] = tmpStr[minStrLen-1-j];
    }

    str[j] = EOS;
}

void dectoa(unsigned int dec,char *str)
{
#define MAX_STR_LEN 32
    unsigned int tmp = dec;
    char tmpStr[MAX_STR_LEN] = "";
    unsigned int i = 0,j = 0;
    unsigned int minStrLen = 0;

    do
    {
        tmpStr[i] = itoa(tmp%10);
        tmp /= 10;
        i++;
    }while(tmp&&(i<MAX_STR_LEN));
    
    tmpStr[i] = EOS;

    minStrLen = sizeof(str)>(i+1)?i:(unsigned int)(sizeof(str)-1);

    for(j=0;j<minStrLen;j++)
    {
        str[j] = tmpStr[minStrLen-1-j];
    }

    str[j] = EOS;
}

int strcmp
(
    const char * s1,   /* string to compare */
    const char * s2    /* string to compare <s1> to */
)
{
    while (*s1++ == *s2++)
        if (s1 [-1] == EOS) return (0);

    return ((int)((unsigned char)s1 [-1] - (unsigned char)s2 [-1]));
}

char * strcpy
(
    char *       s1,    /* string to copy to */
    const char * s2     /* string to copy from */
)
{
    char *save = s1;

    while ((*s1++ = *s2++) != EOS)
    ;

    return (save);
}

char * strcat
(
    char *       destination, /* string to be appended to */
    const char * append       /* string to append to <destination> */
)
{
    char *save = destination;

    while (*destination++ != '\0')        /* find end of string */
        ;

    destination--;

    while ((*destination++ = *append++) != '\0')    
        ;

    return (save);
}

/*******************************************************************************
*
* bcopy - copy one buffer to another
*
* This routine copies the first <nbytes> characters from <source> to
* <destination>.  Overlapping buffers are handled correctly.  Copying is done
* in the most efficient way possible, which may include long-word, or even
* multiple-long-word moves on some architectures.  In general, the copy
* will be significantly faster if both buffers are long-word aligned.
* (For copying that is restricted to byte, word, or long-word moves, see
* the manual entries for bcopyBytes(), bcopyWords(), and bcopyLongs().)
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bcopyBytes(), bcopyWords(), bcopyLongs()
*/
void bcopy
(
    const char *source,           /* pointer to source buffer      */
    char *destination,      /* pointer to destination buffer */
    int nbytes              /* number of bytes to copy       */
)
{
    FAST char *dstend;
    FAST long *src;
    FAST long *dst;
    int tmp = destination - source;

    if (tmp <= 0 || tmp >= nbytes)
    {
        /* forward copy */
    	dstend = destination + nbytes;

        /* do byte copy if less than ten or alignment mismatch */
    	if (nbytes < 10 || (((int)destination ^ (int)source) & ALIGNMENT))
            goto byte_copy_fwd;

        /* if odd-aligned copy byte */
    	while ((int)destination & ALIGNMENT)
            *destination++ = *source++;

    	src = (long *) source;
    	dst = (long *) destination;
    	do
        {
            *dst++ = *src++;
        }while (((char *)dst + sizeof (long)) <= dstend);

    	destination = (char *)dst;
    	source      = (char *)src;

    byte_copy_fwd:
    	while (destination < dstend)
            *destination++ = *source++;
    }
    else
    {
        /* backward copy */
    	dstend       = destination;
    	destination += nbytes;
    	source      += nbytes;

        /* do byte copy if less than ten or alignment mismatch */
    	if (nbytes < 10 || (((int)destination ^ (int)source) & ALIGNMENT))
            goto byte_copy_bwd;

        /* if odd-aligned copy byte */
    	while ((int)destination & ALIGNMENT)
            *--destination = *--source;

    	src = (long *) source;
    	dst = (long *) destination;

    	do
        {
            *--dst = *--src;
        }while (((char *)dst - sizeof(long)) >= dstend);
        
    	destination = (char *)dst;
    	source      = (char *)src;

    byte_copy_bwd:
    	while (destination > dstend)
            *--destination = *--source;
    }
}
    
/*******************************************************************************
*
* bfill - fill a buffer with a specified character
*
* This routine fills the first <nbytes> characters of a buffer with the
* character <ch>.  Filling is done in the most efficient way possible,
* which may be long-word, or even multiple-long-word stores, on some
* architectures.  In general, the fill will be significantly faster if
* the buffer is long-word aligned.  (For filling that is restricted to
* byte stores, see the manual entry for bfillBytes().)
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: bfillBytes()
*/
void bfill
(
    FAST char *buf,           /* pointer to buffer              */
    int nbytes,               /* number of bytes to fill        */
    FAST int ch               /* char with which to fill buffer */
)
{
    FAST long *pBuf;
    char *bufend = buf + nbytes;
    FAST char *buftmp;
    FAST long val;

    if (nbytes < 10)
    	goto byte_fill;

    val = (ch << 24) | (ch << 16) | (ch << 8) | ch;

    /* start on necessary alignment */

    while ((int)buf & ALIGNMENT)
        *buf++ = ch;

    buftmp = bufend - sizeof (long); /* last word boundary before bufend */
    pBuf = (long *)buf;

    /* fill 4 bytes at a time; don't exceed buf endpoint */
    do
    {
        *pBuf++ = val;
    }while ((char *)pBuf < buftmp);

    buf = (char *)pBuf - sizeof (long);

    /* fill remaining bytes one at a time */
byte_fill:
    while (buf < bufend)
        *buf++ = ch;
}

/*******************************************************************************
*
* memcmp - compare two blocks of memory (ANSI)
*
* This routine compares successive elements from two arrays of `unsigned char',
* beginning at the addresses <s1> and <s2> (both of size <n>), until it finds
* elements that are not equal.
*
* INCLUDE FILES: string.h
*
* RETURNS:
* If all elements are equal, zero.  If elements differ and the differing
* element from <s1> is greater than the element from <s2>, the routine
* returns a positive number; otherwise, it returns a negative number.
*/
int memcmp
(
    const void * s1,        /* array 1 */
    const void * s2,        /* array 2 */
    int       n	    /* size of memory to compare */
)
{
    const unsigned char *p1;
    const unsigned char *p2;

    /* size of memory is zero */

    if (n == 0)
        return (0);

    /* compare array 2 into array 1 */

    p1 = s1;
    p2 = s2;

    while (*p1++ == *p2++)
    {
        if (--n == 0)
            return (0);
    }

    return ((*--p1) - (*--p2));
}

/*******************************************************************************
*
* memcpy - copy memory from one location to another (ANSI)
*
* This routine copies <size> characters from the object pointed
* to by <source> into the object pointed to by <destination>. If copying
* takes place between objects that overlap, the behavior is undefined.
*
* INCLUDE FILES: string.h
*
* RETURNS: A pointer to <destination>.
*/

void * memcpy
(
    void *       destination,   /* destination of copy */
    const void * source,        /* source of copy */
    int       size           /* size of memory to copy */
)
{
    bcopy ((char *) source, (char *) destination, (int) size);
    return (destination);
}

/*******************************************************************************
*
* memset - set a block of memory (ANSI)
*
* This routine stores <c> converted to an `unsigned char' in each of the
* elements of the array of `unsigned char' beginning at <m>, with size <size>.
*
* INCLUDE FILES: string.h
*
* RETURNS: A pointer to <m>.
*/
void * memset
(
    void * m,                   /* block of memory */
    int    c,                   /* character to store */
    int size                 /* size of memory */
)
{
    bfill ((char *) m, (int) size, c);
    return (m);
}

#ifdef __cplusplus
}
#endif
/*lint -restore */
