

#ifndef O2CLUSTER_QUORUM_H
#define O2CLUSTER_QUORUM_H

void o2quo_init(void);
void o2quo_exit(void);

void o2quo_hb_up(u8 node);
void o2quo_hb_down(u8 node);
void o2quo_hb_still_up(u8 node);
void o2quo_conn_up(u8 node);
void o2quo_conn_err(u8 node);
void o2quo_disk_timeout(void);

#endif /* O2CLUSTER_QUORUM_H */
