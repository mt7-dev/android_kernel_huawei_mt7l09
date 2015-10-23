//#include "config.h"
#include "string.h"
#include "rsa.h"
#include "rsann.h"

int16 rsa_public_block(uint8 *input, uint16 input_len, uint8 *output, uint16 *output_len, rsa_public_key *public_key)
{
  uint32 c[MAX_NN_DIGITS], e[MAX_NN_DIGITS], m[MAX_NN_DIGITS],
  n[MAX_NN_DIGITS];
  uint16 eDigits, nDigits;

  memset (c, 0, sizeof (c));
  memset (m, 0, sizeof (m));
  memset (n, 0, sizeof (n));

  nn_decode (m, MAX_NN_DIGITS, input, input_len);
  nn_decode (n, MAX_NN_DIGITS, public_key->modulus, MAX_RSA_MODULUS_LEN);
  nn_decode (e, MAX_NN_DIGITS, public_key->public_exponent, MAX_RSA_MODULUS_LEN);
  nDigits = nn_digits (n, MAX_NN_DIGITS);
  eDigits = nn_digits (e, MAX_NN_DIGITS);

  if (nn_cmp (m, n, nDigits) >= 0)
  return -1;

  /* Compute c = m^e mod n. */
  nn_mod_exp (c, m, e, eDigits, n, nDigits);

	*output_len = (uint16)(public_key->bits + 7) / 8;
	nn_encode (output, *output_len, c, nDigits);

	return (0);
}

int16 rsa_private_block(uint8 *input, uint16 input_len, uint8 *output, uint16 *output_len, rsa_private_key *private_key)
{
  uint32 c[MAX_NN_DIGITS], cP[MAX_NN_DIGITS], cQ[MAX_NN_DIGITS],
  dP[MAX_NN_DIGITS], dQ[MAX_NN_DIGITS], mP[MAX_NN_DIGITS],
  mQ[MAX_NN_DIGITS], n[MAX_NN_DIGITS], p[MAX_NN_DIGITS], q[MAX_NN_DIGITS],
  qInv[MAX_NN_DIGITS], t[MAX_NN_DIGITS];
  uint16 cDigits, nDigits, pDigits;

  memset (c, 0, sizeof (c));
  memset (cP, 0, sizeof (cP));
  memset (cQ, 0, sizeof (cQ));
  memset (dP, 0, sizeof (dP));
  memset (dQ, 0, sizeof (dQ));
  memset (mP, 0, sizeof (mP));
  memset (mQ, 0, sizeof (mQ));
  memset (p, 0, sizeof (p));
  memset (q, 0, sizeof (q));
  memset (qInv, 0, sizeof (qInv));
  memset (t, 0, sizeof (t));

  nn_decode (c, MAX_NN_DIGITS, input, input_len);
  nn_decode (n, MAX_NN_DIGITS, private_key->modulus, MAX_RSA_MODULUS_LEN);
  nn_decode (p, MAX_NN_DIGITS, private_key->prime[0], MAX_RSA_PRIME_LEN);
  nn_decode (q, MAX_NN_DIGITS, private_key->prime[1], MAX_RSA_PRIME_LEN);
  nn_decode (dP, MAX_NN_DIGITS, private_key->prime_exponent[0], MAX_RSA_PRIME_LEN);
  nn_decode (dQ, MAX_NN_DIGITS, private_key->prime_exponent[1], MAX_RSA_PRIME_LEN);
  nn_decode (qInv, MAX_NN_DIGITS, private_key->coefficient, MAX_RSA_PRIME_LEN);


  cDigits = nn_digits (c, MAX_NN_DIGITS);
  nDigits = nn_digits (n, MAX_NN_DIGITS);
  pDigits = nn_digits (p, MAX_NN_DIGITS);

  if (nn_cmp (c, n, nDigits) >= 0)
  return -1;

  /* Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has length at most pDigits, i.e., p > q.) */
  nn_mod (cP, c, cDigits, p, pDigits);
  nn_mod (cQ, c, cDigits, q, pDigits);
  nn_mod_exp (mP, cP, dP, pDigits, p, pDigits);

  nn_assign_zero (mQ, nDigits);
  nn_mod_exp (mQ, cQ, dQ, pDigits, q, pDigits);

  /* Chinese Remainder Theorem:  m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ.   */
  if (nn_cmp (mP, mQ, pDigits) >= 0)
  {
    nn_sub (t, mP, mQ, pDigits);
  }
  else
  {
    nn_sub (t, mQ, mP, pDigits);
    nn_sub (t, p, t, pDigits);
  }
  nn_mod_mult (t, t, qInv, p, pDigits);
  nn_mult (t, t, q, pDigits);
  nn_add (t, t, mQ, nDigits);

	*output_len = (uint16)(private_key->bits + 7) / 8;
	nn_encode (output, *output_len, t, nDigits);

	return (0);
}

int16 rsa_private_encrypt(uint8 *input, uint16 input_len, uint8 *output, uint16 *output_len, rsa_private_key *private_key )
{
  int16 status;
  uint8 pkcsBlock[MAX_RSA_MODULUS_LEN];
  int16 i, modulusLen;

	memset (pkcsBlock, 0, sizeof (pkcsBlock));
	modulusLen = (uint16)(private_key->bits + 7) / 8;
	if (input_len + 11 > modulusLen)
		return -1;

  pkcsBlock[0] = 0;
  pkcsBlock[1] = 1;

  for (i = 2; i < modulusLen - input_len - 1; i++)
  {
    pkcsBlock[i] = 0xff;
  }
  /* separator */
  pkcsBlock[i++] = 0;
  memcpy (&pkcsBlock[i], input, input_len);
  status = rsa_private_block(pkcsBlock, modulusLen, output, output_len, private_key);

  return status;

}

int16 rsa_public_decrypt(uint8 *input, uint16 input_len, uint8 *output, uint16 *output_len, rsa_public_key *public_key)
{
  int16 status;
  uint8 pkcsBlock[MAX_RSA_MODULUS_LEN];
  int16 i;
  uint16 modulusLen, pkcsBlockLen;

	memset (pkcsBlock, 0, sizeof (pkcsBlock));
	modulusLen = (uint16)(public_key->bits + 7) / 8;
	if (input_len > modulusLen)
		return -1;

  status = rsa_public_block(input, input_len, pkcsBlock, &pkcsBlockLen, public_key);
  if (0 != status)
  {
    return (status);
  }

  if (pkcsBlockLen != modulusLen)
    return -1;

  /* Require block type 1. */
  if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 1))
    return -1;

  for (i = 2; i < modulusLen-1; i++)
    if (pkcsBlock[i] != 0xff)
      break;

  /* separator */
  if (pkcsBlock[i++] != 0)
    return -1;

  *output_len = modulusLen - i;

  if (*output_len + 11 > modulusLen)
    return -1;

  memcpy (output, &pkcsBlock[i], *output_len);
  return (0);
}

int16 rsa_public_encrypt(uint8 *input, uint16 input_len, uint8 *output, uint16 *output_len, rsa_public_key *public_key )
{
  int16 status;
  uint8 pkcsBlock[MAX_RSA_MODULUS_LEN];
  int16 i, modulusLen;

	memset (pkcsBlock, 0, sizeof (pkcsBlock));
	modulusLen = (uint16)(public_key->bits + 7) / 8;
	if (input_len + 11 > modulusLen)
		return -1;

  pkcsBlock[0] = 0;
  pkcsBlock[1] = 1;

  for (i = 2; i < modulusLen - input_len - 1; i++)
  {
    pkcsBlock[i] = 0xff;
  }
  /* separator */
  pkcsBlock[i++] = 0;
  memcpy (&pkcsBlock[i], input, input_len);
  status = rsa_public_block(pkcsBlock, modulusLen, output, output_len, public_key);

  return status;

}


int16 rsa_private_decrypt(uint8 *input, uint16 input_len, uint8 *output, uint16 *output_len, rsa_private_key *private_key)
{
  int16 status;
  uint8 pkcsBlock[MAX_RSA_MODULUS_LEN];
  int16 i;
  uint16 modulusLen, pkcsBlockLen;

	memset (pkcsBlock, 0, sizeof (pkcsBlock));
	modulusLen = (uint16)(private_key->bits + 7) / 8;
	if (input_len > modulusLen)
		return -1;

  status = rsa_private_block(input, input_len, pkcsBlock, &pkcsBlockLen, private_key);
  if (0 != status)
  {
    return (status);
  }

  if (pkcsBlockLen != modulusLen)
    return -1;

  /* Require block type 1. */
  if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 1))
    return -1;

  for (i = 2; i < modulusLen-1; i++)
    if (pkcsBlock[i] != 0xff)
      break;

  /* separator */
  if (pkcsBlock[i++] != 0)
    return -1;

  *output_len = modulusLen - i;

  if (*output_len + 11 > modulusLen)
    return -1;

  memcpy (output, &pkcsBlock[i], *output_len);
  return (0);
}

