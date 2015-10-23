#ifndef __HASH_MD5_H__
#define __HASH_MD5_H__ 

#include "security_types.h"

/* MD5 context. */
typedef struct {
  uint32 state[4];                                   /* state (ABCD) */
  uint32 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8 buffer[64];                         /* input buffer */
}hash_md5;

void hash_md5_init (hash_md5 *context);
void hash_md5_update(hash_md5 *context, uint8 *input, uint32 inputLen);
void hash_md5_final (hash_md5 *context, uint8 digest[16]);
void encrypt_lock_md5_data (void* input_data, uint16 input_length, 
                 void* output_data);

#endif

