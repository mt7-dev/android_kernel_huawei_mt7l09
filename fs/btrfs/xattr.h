

#ifndef __XATTR__
#define __XATTR__

#include <linux/xattr.h>

extern const struct xattr_handler btrfs_xattr_acl_access_handler;
extern const struct xattr_handler btrfs_xattr_acl_default_handler;
extern const struct xattr_handler *btrfs_xattr_handlers[];

extern ssize_t __btrfs_getxattr(struct inode *inode, const char *name,
		void *buffer, size_t size);
extern int __btrfs_setxattr(struct btrfs_trans_handle *trans,
			    struct inode *inode, const char *name,
			    const void *value, size_t size, int flags);
extern ssize_t btrfs_getxattr(struct dentry *dentry, const char *name,
		void *buffer, size_t size);
extern int btrfs_setxattr(struct dentry *dentry, const char *name,
		const void *value, size_t size, int flags);
extern int btrfs_removexattr(struct dentry *dentry, const char *name);

extern int btrfs_xattr_security_init(struct btrfs_trans_handle *trans,
				     struct inode *inode, struct inode *dir,
				     const struct qstr *qstr);

#endif /* __XATTR__ */
