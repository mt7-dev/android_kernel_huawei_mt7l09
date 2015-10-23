#include <osl_list.h>


struct my_struct {
	int			data;
	struct list_head	list;
};



int test_osl_list_case1()
{
	struct list_head *item;
	struct my_struct  head = {1, {NULL, NULL}};
	struct my_struct  s1 = {1, {NULL, NULL}};
	struct my_struct  s2 = {2, {NULL, NULL}};
	struct my_struct  s3 = {3, {NULL, NULL}};

	printk("init\n");
	INIT_LIST_HEAD(&head.list);
	INIT_LIST_HEAD(&s1.list);
	INIT_LIST_HEAD(&s2.list);
	INIT_LIST_HEAD(&s3.list);

	printk("add\n");
	list_add(&s1.list, &head.list);
	list_add(&s2.list, &head.list);
	list_add(&s3.list, &head.list);
	printk("del\n");
	list_del(&s3.list);
	list_del(&s2.list);
	
	printk("add\n");
	INIT_LIST_HEAD(&s2.list);
	INIT_LIST_HEAD(&s3.list);
	list_add(&s3.list, &head.list);
	list_add(&s2.list, &head.list);

	printk("list_for_each\n");
	list_for_each(item, &head.list) {
		struct my_struct *v = list_entry (item, struct my_struct, list);
		printk("list_for_each:%d\n", v->data);
	}

	return 0;
}
