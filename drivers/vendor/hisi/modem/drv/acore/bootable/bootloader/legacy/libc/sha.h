// Copyright 2008 Google Inc. All Rights Reserved.
// Author: mschilder@google.com (Marius Schilder)

#ifndef _EMBEDDED_SHA_H_
#define _EMBEDDED_SHA_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct SHA_CTX {
  uint64_t count;
  uint32_t state[5];
  union {
    uint8_t b[64];
    uint32_t w[16];
  } buf;
} SHA_CTX;

void SHA_init(SHA_CTX* ctx);
void SHA_update(SHA_CTX* ctx, const void* data, int len);
const uint8_t* SHA_final(SHA_CTX* ctx);

// Convenience method. Returns digest parameter value.
const uint8_t* SHA(const void* data, int len, uint8_t* digest);

#define SHA_DIGEST_SIZE 20

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // _EMBEDDED_SHA_H_
