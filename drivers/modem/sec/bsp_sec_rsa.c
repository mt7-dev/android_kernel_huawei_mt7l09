/*
    NN.C - natural numbers routines

    Copyright (c) J.S.A.Kapp 1994 - 1995.

    RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

    All functions prototypes are the Same as for RSAREF(tm).
    To aid compatiblity the source and the files follow the
    same naming comventions that RSAREF(tm) uses.  This should aid
    direct importing to you applications.

    This library is legal everywhere outside the US.  And should
    NOT be imported to the US and used there.

    All Trademarks Acknowledged.

    Revision hisitory
        0.90 First revision, this revision was the basic routines.
        Routines slower than final revision.

        0.91 Second revision, this is the current revision, all
        routines have been altered for speed increases.  Also the
        addition of assembler equivalents.
*/

#include "bsp_sec_rsa.h"
//#pragma warning( disable : 4131 )
#pragma GCC diagnostic ignored "-Wsequence-point"
#pragma GCC diagnostic ignored "-Wframe-larger-than="


/* internal static functions */

static NN_DIGIT_EX subdigitmult(NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX, NN_DIGIT_EX *, unsigned int);

static void dmult(NN_DIGIT_EX, NN_DIGIT_EX, NN_DIGIT_EX *, NN_DIGIT_EX *);

static unsigned int NN_DigitBits(NN_DIGIT_EX);

#ifndef USEASM
/* Decodes character string b into a, where character string is ordered
     from most to least significant.

     Lengths: a[digits], b[len].
     Assumes b[i] = 0 for i < len - digits * NN_DIGIT_LEN. (Otherwise most
     significant bytes are truncated.)
 */
void NN_Decodeex (NN_DIGIT_EX *a, unsigned int digits, unsigned char *b, unsigned int len)
{
  NN_DIGIT_EX t;
  int j;
  unsigned int i, u;

  for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
    t = 0;
    for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
            t |= ((NN_DIGIT_EX)b[j]) << u;
        a[i] = t;
  }

  for (; i < digits; i++)
    a[i] = 0;
}

/* Encodes b into character string a, where character string is ordered
   from most to least significant.

     Lengths: a[len], b[digits].
     Assumes NN_Bitsex (b, digits) <= 8 * len. (Otherwise most significant
     digits are truncated.)
 */
void NN_Encodeex (unsigned char *a, unsigned int len, NN_DIGIT_EX *b, unsigned int digits)
{
    NN_DIGIT_EX t;
    int j;
    unsigned int i, u;

    for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
        t = b[i];
        for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
            a[j] = (unsigned char)(t >> u);
    }

    for (; j >= 0; j--)
        a[j] = 0;
}

/* Assigns a = 0. */

void NN_AssignZeroex (NN_DIGIT_EX *a, unsigned int digits)
{
    if(digits) {
        do {
            *a++ = 0;
        }while(--digits);
    }
}

#endif

/* Assigns a = 2^b.

   Lengths: a[digits].
     Requires b < digits * NN_DIGIT_BITS.
 */
void NN_Assign2Expex (NN_DIGIT_EX *a, unsigned int b, unsigned int digits)
{
  NN_AssignZeroex (a, digits);

    if (b >= digits * NN_DIGIT_BITS)
    return;

  a[b / NN_DIGIT_BITS] = (NN_DIGIT_EX)1 << (b % NN_DIGIT_BITS);
}

/* Computes a = b - c. Returns borrow.

     Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT_EX NN_Subex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX *c, unsigned int digits)
{
    NN_DIGIT_EX temp, borrow = 0;

    if(digits)
        do {
            if((temp = (*b++) - borrow) == MAX_NN_DIGIT)
                temp = MAX_NN_DIGIT - *c++;
            else
                /* coverity[read_write_order] */
                if((temp -= *c) > (MAX_NN_DIGIT - *c++))    /*lint !e564*/
                    borrow = 1;
                else
                    borrow = 0;
            *a++ = temp;
        }while(--digits);

    return(borrow);
}

/* Computes a = b * c.

     Lengths: a[2*digits], b[digits], c[digits].
     Assumes digits < MAX_NN_DIGITS_EX.
*/

void NN_Multex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX *c, unsigned int digits)
{
    /* coverity[uvar_decl] */
    NN_DIGIT_EX t[2*MAX_NN_DIGITS_EX];
    NN_DIGIT_EX dhigh, dlow, carry;
    unsigned int bDigits, cDigits, i, j;

    NN_AssignZeroex (t, 2 * digits);

    bDigits = NN_Digitsex (b, digits);
    cDigits = NN_Digitsex (c, digits);

    for (i = 0; i < bDigits; i++) {
        carry = 0;
        if(*(b+i) != 0) {
            for(j = 0; j < cDigits; j++) {
                dmult(*(b+i), *(c+j), &dhigh, &dlow);
                /* coverity[uninit_use] */
                if((*(t+(i+j)) = *(t+(i+j)) + carry) < carry)
                    carry = 1;
                else
                    carry = 0;
                if((*(t+(i+j)) += dlow) < dlow)
                    carry++;
                carry += dhigh;
            }
        }
        /* coverity[uninit_use] */
        *(t+(i+cDigits)) += carry;
    }


    NN_Assignex(a, t, 2 * digits);
}

/* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.

     Requires c < NN_DIGIT_BITS. */

NN_DIGIT_EX NN_LShiftex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, unsigned int c, unsigned int digits)
{
    NN_DIGIT_EX temp, carry = 0;
    unsigned int t;

    if(c < NN_DIGIT_BITS)
        if(digits) {

            t = NN_DIGIT_BITS - c;

            do {
                temp = *b++;
                *a++ = (temp << c) | carry;
                carry = c ? (temp >> t) : 0;
            }while(--digits);
        }

    return (carry);
}

/* Computes a = c div 2^c (i.e., shifts right c bits), returning carry.

     Requires: c < NN_DIGIT_BITS. */

NN_DIGIT_EX NN_RShiftex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, unsigned int c, unsigned int digits)
{
    NN_DIGIT_EX temp, carry = 0;
    unsigned int t;

    if(c < NN_DIGIT_BITS)
        if(digits) {

            t = NN_DIGIT_BITS - c;

            do {
                digits--;
                temp = *(b+digits);
                *(a+digits) = (temp >> c) | carry;
                carry = c ? (temp << t) : 0;
            }while(digits);
        }

    return (carry);
}

/* Computes a = c div d and b = c mod d.

     Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
     Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS_EX,
                     dDigits < MAX_NN_DIGITS_EX.
*/

void NN_Divex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX *c, unsigned int cDigits, NN_DIGIT_EX *d, unsigned int dDigits)
{
    /* coverity[var_decl] */
    NN_DIGIT_EX ai, cc[2*MAX_NN_DIGITS_EX+1], dd[MAX_NN_DIGITS_EX], s;
    NN_DIGIT_EX t[2], u, v, *ccptr;
    NN_HALF_DIGIT_EX aHigh, aLow, cHigh, cLow;
    int i;
    unsigned int ddDigits, shift;

    ddDigits = NN_Digitsex (d, dDigits);
    if(ddDigits == 0)
        return;

    shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
    NN_AssignZeroex (cc, ddDigits);
    cc[cDigits] = NN_LShiftex (cc, c, shift, cDigits);
    NN_LShiftex (dd, d, shift, ddDigits);
    /* coverity[uninit_use] */
    s = dd[ddDigits-1];

    NN_AssignZeroex (a, cDigits);

    for (i = cDigits-ddDigits; i >= 0; i--) {
        if (s == MAX_NN_DIGIT)
            ai = cc[i+ddDigits];
        else {
            ccptr = &cc[i+ddDigits-1];

            s++;
            cHigh = (NN_HALF_DIGIT_EX)HIGH_HALF (s);
            cLow = (NN_HALF_DIGIT_EX)LOW_HALF (s);

            *t = *ccptr;
            *(t+1) = *(ccptr+1);

            if (cHigh == MAX_NN_HALF_DIGIT)
                aHigh = (NN_HALF_DIGIT_EX)HIGH_HALF (*(t+1));
            else
                aHigh = (NN_HALF_DIGIT_EX)(*(t+1) / (cHigh + 1));
            u = (NN_DIGIT_EX)aHigh * (NN_DIGIT_EX)cLow;
            v = (NN_DIGIT_EX)aHigh * (NN_DIGIT_EX)cHigh;
            if ((*t -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))
                t[1]--;
            *(t+1) -= HIGH_HALF (u);
            *(t+1) -= v;

            while ((*(t+1) > cHigh) ||
                         ((*(t+1) == cHigh) && (*t >= TO_HIGH_HALF (cLow)))) {
                if ((*t -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow))
                    t[1]--;
                *(t+1) -= cHigh;
                aHigh++;
            }

            if (cHigh == MAX_NN_HALF_DIGIT)
                aLow = (NN_HALF_DIGIT_EX)LOW_HALF (*(t+1));
            else
                aLow =
            (NN_HALF_DIGIT_EX)((TO_HIGH_HALF (*(t+1)) + HIGH_HALF (*t)) / (cHigh + 1));
            u = (NN_DIGIT_EX)aLow * (NN_DIGIT_EX)cLow;
            v = (NN_DIGIT_EX)aLow * (NN_DIGIT_EX)cHigh;
            if ((*t -= u) > (MAX_NN_DIGIT - u))
                t[1]--;
            if ((*t -= TO_HIGH_HALF (v)) > (MAX_NN_DIGIT - TO_HIGH_HALF (v)))
                t[1]--;
            *(t+1) -= HIGH_HALF (v);

            while ((*(t+1) > 0) || ((*(t+1) == 0) && *t >= s)) {
                if ((*t -= s) > (MAX_NN_DIGIT - s))
                    t[1]--;
                aLow++;
            }

            ai = TO_HIGH_HALF (aHigh) + aLow;
            s--;
        }

        cc[i+ddDigits] -= subdigitmult(&cc[i], &cc[i], ai, dd, ddDigits);

        while (cc[i+ddDigits] || (NN_Cmpex (&cc[i], dd, ddDigits) >= 0)) {
            ai++;
            cc[i+ddDigits] -= NN_Subex (&cc[i], &cc[i], dd, ddDigits);
        }

        a[i] = ai;
    }

    NN_AssignZeroex(b, dDigits);
    NN_RShiftex(b, cc, shift, ddDigits);
}


/* Computes a = b mod c.

     Lengths: a[cDigits], b[bDigits], c[cDigits].
     Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS_EX, cDigits < MAX_NN_DIGITS_EX.
*/
void NN_Modex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, unsigned int bDigits, NN_DIGIT_EX *c, unsigned int cDigits)
{
    NN_DIGIT_EX t[2 * MAX_NN_DIGITS_EX];

    NN_Divex (t, a, b, bDigits, c, cDigits);
}

/* Computes a = b * c mod d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes d > 0, digits < MAX_NN_DIGITS_EX.
 */
void NN_ModMultex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX *c, NN_DIGIT_EX *d, unsigned int digits)
{
    NN_DIGIT_EX t[2*MAX_NN_DIGITS_EX];

    NN_Multex (t, b, c, digits);
    NN_Modex (a, t, 2 * digits, d, digits);
}

/* Computes a = b^c mod d.

   Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
     Assumes d > 0, cDigits > 0, dDigits < MAX_NN_DIGITS_EX.
 */
void NN_ModExpex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX *c, unsigned int cDigits, NN_DIGIT_EX *d, unsigned int dDigits)
{
    NN_DIGIT_EX bPower[3][MAX_NN_DIGITS_EX], ci, t[MAX_NN_DIGITS_EX];
    int i;
    unsigned int ciBits, j, s;

    /* Store b, b^2 mod d, and b^3 mod d.
     */
    NN_Assignex (bPower[0], b, dDigits);
    NN_ModMultex (bPower[1], bPower[0], b, d, dDigits);
    NN_ModMultex (bPower[2], bPower[1], b, d, dDigits);

    NN_ASSIGN_DIGIT (t, 1, dDigits);

    cDigits = NN_Digitsex (c, cDigits);
    /* coverity[overflow_const] */
    for (i = cDigits - 1; i >= 0; i--) {
        ci = c[i];
        ciBits = NN_DIGIT_BITS;

        /* Scan past leading zero bits of most significant digit.
         */
        if (i == (int)(cDigits - 1)) {
            while (! DIGIT_2MSB (ci)) {
                ci <<= 2;
                ciBits -= 2;
            }
        }

        for (j = 0; j < ciBits; j += 2, ci <<= 2) {
        /* Compute t = t^4 * b^s mod d, where s = two MSB's of ci. */
            NN_ModMultex (t, t, t, d, dDigits);
            NN_ModMultex (t, t, t, d, dDigits);
            if ((s = DIGIT_2MSB (ci)) != 0)
            NN_ModMultex (t, t, bPower[s-1], d, dDigits);
        }
    }

    NN_Assignex (a, t, dDigits);
}

/* Compute a = 1/b mod c, assuming inverse exists.

   Lengths: a[digits], b[digits], c[digits].
     Assumes gcd (b, c) = 1, digits < MAX_NN_DIGITS_EX.
 */
void NN_ModInvex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX *c, unsigned int digits)
{
    NN_DIGIT_EX q[MAX_NN_DIGITS_EX], t1[MAX_NN_DIGITS_EX], t3[MAX_NN_DIGITS_EX],
        u1[MAX_NN_DIGITS_EX], u3[MAX_NN_DIGITS_EX], v1[MAX_NN_DIGITS_EX],
        v3[MAX_NN_DIGITS_EX], w[2*MAX_NN_DIGITS_EX];
    int u1Sign;

    /* Apply extended Euclidean algorithm, modified to avoid negative
       numbers.
    */
    NN_ASSIGN_DIGIT (u1, 1, digits);
    NN_AssignZeroex (v1, digits);
    NN_Assignex (u3, b, digits);
    NN_Assignex (v3, c, digits);
    u1Sign = 1;

    while (! NN_Zeroex (v3, digits)) {
        NN_Divex (q, t3, u3, digits, v3, digits);
        NN_Multex (w, q, v1, digits);
        NN_Addex (t1, u1, w, digits);
        NN_Assignex (u1, v1, digits);
        NN_Assignex (v1, t1, digits);
        NN_Assignex (u3, v3, digits);
        NN_Assignex (v3, t3, digits);
        u1Sign = -u1Sign;
    }

    /* Negate result if sign is negative. */
    if (u1Sign < 0)
        NN_Subex (a, c, u1, digits);
    else
        NN_Assignex (a, u1, digits);
}

/* Computes a = gcd(b, c).

     Assumes b > c, digits < MAX_NN_DIGITS_EX.
*/

#define iplus1  ( i==2 ? 0 : i+1 )      /* used by Euclid algorithms */
#define iminus1 ( i==0 ? 2 : i-1 )      /* used by Euclid algorithms */
#define g(i) (  &(t[i][0])  )

void NN_Gcdex(NN_DIGIT_EX *a ,NN_DIGIT_EX *b ,NN_DIGIT_EX *c, unsigned int digits)
{
    short i;
    NN_DIGIT_EX t[3][MAX_NN_DIGITS_EX];

    NN_Assignex(g(0), c, digits);
    NN_Assignex(g(1), b, digits);

    i=1;

    while(!NN_Zeroex(g(i),digits)) {
        NN_Modex(g(iplus1), g(iminus1), digits, g(i), digits);
        i = iplus1;
    }

    NN_Assignex(a , g(iminus1), digits);
}

/* Returns the significant length of a in bits.

     Lengths: a[digits]. */

unsigned int NN_Bitsex (NN_DIGIT_EX *a, unsigned int digits)
{
    if ((digits = NN_Digitsex (a, digits)) == 0)
        return (0);

    return ((digits - 1) * NN_DIGIT_BITS + NN_DigitBits (a[digits-1]));
}

#ifndef USEASM

/* Returns sign of a - b. */

int NN_Cmpex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, unsigned int digits)
{

    if(digits) {
        do {
            digits--;
            if(*(a+digits) > *(b+digits))
                return(1);
            if(*(a+digits) < *(b+digits))
                return(-1);
        }while(digits);
    }

    return (0);
}

/* Returns nonzero iff a is zero. */

int NN_Zeroex (NN_DIGIT_EX *a, unsigned int digits)
{
    if(digits) {
        do {
            if(*a++)
                return(0);
        }while(--digits);
    }

    return (1);
}

/* Assigns a = b. */

void NN_Assignex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, unsigned int digits)
{
    if(digits) {
        do {
            *a++ = *b++;
        }while(--digits);
    }
}

/* Returns the significant length of a in digits. */

unsigned int NN_Digitsex (NN_DIGIT_EX *a, unsigned int digits)
{

    if(digits) {
        digits--;

        do {
            if(*(a+digits))
                break;
        }while(digits--);

        return(digits + 1);
    }

    return(digits);
}

/* Computes a = b + c. Returns carry.

     Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT_EX NN_Addex (NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX *c, unsigned int digits)
{
    NN_DIGIT_EX temp, carry = 0;

    if(digits)
        do {
            if((temp = (*b++) + carry) < carry)
                temp = *c++;
            else
                /* coverity[read_write_order] */
                if((temp += *c) < *c++) /*lint !e564*/
                    carry = 1;
                else
                    carry = 0;
            *a++ = temp;
        }while(--digits);

    return (carry);
}

#endif

static NN_DIGIT_EX subdigitmult(NN_DIGIT_EX *a, NN_DIGIT_EX *b, NN_DIGIT_EX c, NN_DIGIT_EX *d, unsigned int digits)
{
    NN_DIGIT_EX borrow, thigh, tlow;
    unsigned int i;

    borrow = 0;

    if(c != 0) {
        for(i = 0; i < digits; i++) {
            dmult(c, d[i], &thigh, &tlow);
            if((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
                borrow = 1;
            else
                borrow = 0;
            if((a[i] -= tlow) > (MAX_NN_DIGIT - tlow))
                borrow++;
            borrow += thigh;
        }
    }

    return (borrow);
}

/* Returns the significant length of a in bits, where a is a digit. */

static unsigned int NN_DigitBits (NN_DIGIT_EX a)
{
    unsigned int i;

    for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
        if (a == 0)
            break;

    return (i);
}

/* Computes a * b, result stored in high and low. */

static void dmult(NN_DIGIT_EX a, NN_DIGIT_EX b, NN_DIGIT_EX *high, NN_DIGIT_EX *low)
{
    NN_HALF_DIGIT_EX al, ah, bl, bh;
    NN_DIGIT_EX m1, m2, m, ml, mh, carry = 0;

    al = (NN_HALF_DIGIT_EX)LOW_HALF(a);
    ah = (NN_HALF_DIGIT_EX)HIGH_HALF(a);
    bl = (NN_HALF_DIGIT_EX)LOW_HALF(b);
    bh = (NN_HALF_DIGIT_EX)HIGH_HALF(b);

    *low = (NN_DIGIT_EX) al*bl;
    *high = (NN_DIGIT_EX) ah*bh;

    m1 = (NN_DIGIT_EX) al*bh;
    m2 = (NN_DIGIT_EX) ah*bl;
    m = m1 + m2;

    if(m < m1)
        carry = 1L << (NN_DIGIT_BITS / 2);

    ml = (m & MAX_NN_HALF_DIGIT) << (NN_DIGIT_BITS / 2);
    mh = m >> (NN_DIGIT_BITS / 2);

    *low += ml;

    if(*low < ml)
        carry++;

    *high += carry + mh;
}


