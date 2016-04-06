

#ifndef _RAMSTER_NODEMANAGER_H
#define _RAMSTER_NODEMANAGER_H

#define R2NM_API_VERSION	5

#define R2NM_MAX_NODES		255
#define R2NM_INVALID_NODE_NUM	255

/* host name, group name, cluster name all 64 bytes */
#define R2NM_MAX_NAME_LEN        64    /* __NEW_UTS_LEN */

extern int r2nm_init(void);

#endif /* _RAMSTER_NODEMANAGER_H */
