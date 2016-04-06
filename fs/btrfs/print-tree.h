

#ifndef __PRINT_TREE_
#define __PRINT_TREE_
void btrfs_print_leaf(struct btrfs_root *root, struct extent_buffer *l);
void btrfs_print_tree(struct btrfs_root *root, struct extent_buffer *c);
#endif
