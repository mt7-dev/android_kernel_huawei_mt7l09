

#ifndef __HASH__
#define __HASH__

#include <linux/crc32c.h>
static inline u64 btrfs_name_hash(const char *name, int len)
{
	return crc32c((u32)~1, name, len);
}

/*
 * Figure the key offset of an extended inode ref
 */
static inline u64 btrfs_extref_hash(u64 parent_objectid, const char *name,
				    int len)
{
	return (u64) crc32c(parent_objectid, name, len);
}

#endif
