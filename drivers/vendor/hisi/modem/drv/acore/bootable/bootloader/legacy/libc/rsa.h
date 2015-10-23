// Copyright 2008 Google Inc. All Rights Reserved.
// Author: mschilder@google.com (Marius Schilder)

#ifndef _EMBEDDED_RSA_H_
#define _EMBEDDED_RSA_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define RSANUMBYTES 256  // 2048 bit key length
#define RSANUMWORDS (RSANUMBYTES / sizeof(uint32_t))

typedef struct RSAPublicKeyInstance {
  int len;  // Length of n[] in number of uint32_t
  uint32_t n0inv;  // -1 / n[0] mod 2^32
  uint32_t n[RSANUMWORDS];  // modulus as little endian array
  uint32_t rr[RSANUMWORDS];  // R^2 as little endian array
} RSAPublicKeyInstance;

typedef const RSAPublicKeyInstance * const RSAPublicKey;

int RSA_verify(RSAPublicKey mod,
               const uint8_t* signature,
               const int len,
               const uint8_t* sha);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // _EMBEDDED_RSA_H_
