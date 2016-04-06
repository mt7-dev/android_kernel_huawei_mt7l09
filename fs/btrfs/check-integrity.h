

#if !defined(__BTRFS_CHECK_INTEGRITY__)
#define __BTRFS_CHECK_INTEGRITY__

#ifdef CONFIG_BTRFS_FS_CHECK_INTEGRITY
int btrfsic_submit_bh(int rw, struct buffer_head *bh);
void btrfsic_submit_bio(int rw, struct bio *bio);
#else
#define btrfsic_submit_bh submit_bh
#define btrfsic_submit_bio submit_bio
#endif

int btrfsic_mount(struct btrfs_root *root,
		  struct btrfs_fs_devices *fs_devices,
		  int including_extent_data, u32 print_mask);
void btrfsic_unmount(struct btrfs_root *root,
		     struct btrfs_fs_devices *fs_devices);

#endif
