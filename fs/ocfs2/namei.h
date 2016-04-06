

#ifndef OCFS2_NAMEI_H
#define OCFS2_NAMEI_H

extern const struct inode_operations ocfs2_dir_iops;

struct dentry *ocfs2_get_parent(struct dentry *child);

int ocfs2_orphan_del(struct ocfs2_super *osb,
		     handle_t *handle,
		     struct inode *orphan_dir_inode,
		     struct inode *inode,
		     struct buffer_head *orphan_dir_bh);
int ocfs2_create_inode_in_orphan(struct inode *dir,
				 int mode,
				 struct inode **new_inode);
int ocfs2_mv_orphaned_inode_to_new(struct inode *dir,
				   struct inode *new_inode,
				   struct dentry *new_dentry);

#endif /* OCFS2_NAMEI_H */
