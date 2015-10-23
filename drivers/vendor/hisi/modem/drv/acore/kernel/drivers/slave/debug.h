#include <linux/mmc/sdio_func.h>

#ifdef CONFIG_DEBUG_FS
extern int arasan_sdio_debugfs_init(struct sdio_func *);
extern void arasan_sdio_debugfs_exit(struct sdio_func *);
#else
static inline int arasan_sdio_debugfs_init(struct sdio_func *);
{  return 0;  }
static inline void arasan_sdio_debugfs_exit(struct sdio_func *);
{  }
#endif

