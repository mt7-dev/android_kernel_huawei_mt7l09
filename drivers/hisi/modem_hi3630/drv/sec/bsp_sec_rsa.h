/*
    NN.H - header file for NN.C

    Copyright (c) J.S.A.Kapp 1994 - 1995.

    RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

    All functions prototypes are the Same as for RSAREF(tm).
    To aid compatiblity the source and the files follow the
    same naming comventions that RSAREF(tm) uses.  This should aid
    direct importing to your applications.

    This library is legal everywhere outside the US.  And should
    NOT be imported to the US and used there.

    All Trademarks Acknowledged.

    Math Library Routines Header File.

    Revision 1.00 - JSAK 23/6/95, Final Release Version
*/

#ifndef _NNEX_H_
#define _NNEX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
     PGP 2.6.2 Now allows 2048-bit keys changing below will allow this.
     It does lengthen key generation slightly if the value is increased.
*/
#define MAX_RSA_MODULUS_BITS 2048
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

/* Type definitions. */
typedef unsigned long NN_DIGIT_EX;
typedef unsigned short NN_HALF_DIGIT_EX;

/* Constants.

    Note: MAX_NN_DIGITS_EX is long enough to hold any RSA modulus, plus
    one more digit as required by R_GeneratePEMKeys (for n and phiN,
    whose lengths must be even). All natural numbers have at most
    MAX_NN_DIGITS_EX digits, except for double-length intermediate values
    in NN_Multex (t), NN_ModMultex (t), NN_ModInvex (w), and NN_Divex (c).
*/

/* Length of digit in bits */
#define NN_DIGIT_BITS 32
#define NN_HALF_DIGIT_BITS 16
/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS / 8)
/* Maximum length in digits */
#define MAX_NN_DIGITS_EX \
  ((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN - 1) / NN_DIGIT_LEN + 1)
/* Maximum digits */
#define MAX_NN_DIGIT 0xffffffff
#define MAX_NN_HALF_DIGIT 0xffff

#define NN_LT   -1
#define NN_EQ   0
#define NN_GT 1

/* Macros. */

#define LOW_HALF(x) ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x) (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((NN_DIGIT_EX)(x)) << NN_HALF_DIGIT_BITS)
#ifndef DIGIT_MSB
#define DIGIT_MSB(x) (unsigned int)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#endif
#ifndef DIGIT_2MSB
#define DIGIT_2MSB(x) (unsigned int)(((x) >> (NN_DIGIT_BITS - 2)) & 3)
#endif
/* CONVERSIONS
   NN_Decodeex (a, digits, b, len)   Decodes character string b into a.
   NN_Encodeex (a, len, b, digits)   Encodes a into character string b.

   ASSIGNMENTS
   NN_Assignex (a, b, digits)        Assigns a = b.
   NN_ASSIGN_DIGIT (a, b, digits)    Assigns a = b, where b is a digit.
   NN_AssignZeroex (a, b, digits)    Assigns a = 0.
   NN_Assign2Expex (a, b, digits)    Assigns a = 2^b.

   ARITHMETIC OPERATIONS
   NN_Addex (a, b, c, digits)        Computes a = b + c.
   NN_Subex (a, b, c, digits)        Computes a = b - c.
   NN_Multex (a, b, c, digits)       Computes a = b * c.
   NN_LShiftex (a, b, c, digits)     Computes a = b * 2^c.
   NN_RShiftex (a, b, c, digits)     Computes a = b / 2^c.
   NN_Divex (a, b, c, cDigits, d, dDigits)  Computes a = c div d and b = c mod d.

   NUMBER THEORY
   NN_Modex (a, b, bDigits, c, cDigits)  Computes a = b mod c.
   NN_ModMultex (a, b, c, d, digits) Computes a = b * c mod d.
   NN_ModExpex (a, b, c, cDigits, d, dDigits)  Computes a = b^c mod d.
   NN_ModInvex (a, b, c, digits)     Computes a = 1/b mod c.
   NN_Gcdex (a, b, c, digits)        Computes a = gcd (b, c).

   OTHER OPERATIONS
   NN_EVEN (a, digits)               Returns 1 iff a is even.
   NN_Cmpex (a, b, digits)           Returns sign of a - b.
   NN_EQUAL (a, digits)              Returns 1 iff a = b.
   NN_Zeroex (a, digits)             Returns 1 iff a = 0.
   NN_Digitsex (a, digits)           Returns significant length of a in digits.
   NN_Bitsex (a, digits)             Returns significant length of a in bits.
 */
void NN_Decodeex
  (NN_DIGIT_EX *, unsigned int, unsigned char *, unsigned int);
void NN_Encodeex
  (unsigned char *, unsigned int, NN_DIGIT_EX *, unsigned int);

void NN_Assignex (NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);
void NN_AssignZeroex (NN_DIGIT_EX *, unsigned int);
void NN_Assign2Expex (NN_DIGIT_EX *, unsigned int, unsigned int);

NN_DIGIT_EX NN_Addex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);
NN_DIGIT_EX NN_Subex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);
void NN_Multex (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);
void NN_Divex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int, NN_DIGIT_EX *,
        unsigned int);
NN_DIGIT_EX NN_LShiftex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int, unsigned int);
NN_DIGIT_EX NN_RShiftex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int, unsigned int);
NN_DIGIT_EX NN_LRotateex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int, unsigned int);

void NN_Modex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int, NN_DIGIT_EX *, unsigned int);
void NN_ModMultex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);
void NN_ModExpex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int, NN_DIGIT_EX *,
        unsigned int);
void NN_ModInvex
    (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);
void NN_Gcdex (NN_DIGIT_EX *, NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);

int NN_Cmpex (NN_DIGIT_EX *, NN_DIGIT_EX *, unsigned int);
int NN_Zeroex (NN_DIGIT_EX *, unsigned int);
unsigned int NN_Bitsex (NN_DIGIT_EX *, unsigned int);
unsigned int NN_Digitsex (NN_DIGIT_EX *, unsigned int);
#ifndef NN_ASSIGN_DIGIT
#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZeroex (a, digits); a[0] = b;}
#endif
#define NN_EQUAL(a, b, digits) (! NN_Cmpex (a, b, digits))
#define NN_EVEN(a, digits) (((digits) == 0) || ! (a[0] & 1))

#ifdef __cplusplus
}
#endif

#endif /* _NNEX_H_ */

