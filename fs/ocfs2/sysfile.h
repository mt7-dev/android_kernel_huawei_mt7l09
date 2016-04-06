

#ifndef OCFS2_SYSFILE_H
#define OCFS2_SYSFILE_H

struct inode * ocfs2_get_system_file_inode(struct ocfs2_super *osb,
					   int type,
					   u32 slot);

#endif /* OCFS2_SYSFILE_H */
