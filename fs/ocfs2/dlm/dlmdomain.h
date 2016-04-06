

#ifndef DLMDOMAIN_H
#define DLMDOMAIN_H

extern spinlock_t dlm_domain_lock;
extern struct list_head dlm_domains;

int dlm_joined(struct dlm_ctxt *dlm);
int dlm_shutting_down(struct dlm_ctxt *dlm);
void dlm_fire_domain_eviction_callbacks(struct dlm_ctxt *dlm,
					int node_num);

#endif
