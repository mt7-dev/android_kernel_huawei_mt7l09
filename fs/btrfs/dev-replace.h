

#if !defined(__BTRFS_DEV_REPLACE__)
#define __BTRFS_DEV_REPLACE__

struct btrfs_ioctl_dev_replace_args;

int btrfs_init_dev_replace(struct btrfs_fs_info *fs_info);
int btrfs_run_dev_replace(struct btrfs_trans_handle *trans,
			  struct btrfs_fs_info *fs_info);
void btrfs_after_dev_replace_commit(struct btrfs_fs_info *fs_info);
int btrfs_dev_replace_start(struct btrfs_root *root,
			    struct btrfs_ioctl_dev_replace_args *args);
void btrfs_dev_replace_status(struct btrfs_fs_info *fs_info,
			      struct btrfs_ioctl_dev_replace_args *args);
int btrfs_dev_replace_cancel(struct btrfs_fs_info *fs_info,
			     struct btrfs_ioctl_dev_replace_args *args);
void btrfs_dev_replace_suspend_for_unmount(struct btrfs_fs_info *fs_info);
int btrfs_resume_dev_replace_async(struct btrfs_fs_info *fs_info);
int btrfs_dev_replace_is_ongoing(struct btrfs_dev_replace *dev_replace);
void btrfs_dev_replace_lock(struct btrfs_dev_replace *dev_replace);
void btrfs_dev_replace_unlock(struct btrfs_dev_replace *dev_replace);

static inline void btrfs_dev_replace_stats_inc(atomic64_t *stat_value)
{
	atomic64_inc(stat_value);
}
#endif
