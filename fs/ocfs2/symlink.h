

#ifndef OCFS2_SYMLINK_H
#define OCFS2_SYMLINK_H

extern const struct inode_operations ocfs2_symlink_inode_operations;
extern const struct address_space_operations ocfs2_fast_symlink_aops;

/*
 * Test whether an inode is a fast symlink.
 */
static inline int ocfs2_inode_is_fast_symlink(struct inode *inode)
{
	return (S_ISLNK(inode->i_mode) &&
		inode->i_blocks == 0);
}


#endif /* OCFS2_SYMLINK_H */
