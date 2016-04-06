#include <linux/module.h>
#include <linux/string.h>
#include <linux/kallsyms.h>
#include <linux/tty.h>
#ifdef CONFIG_HISI_BALONG_MODEM
#include <product_config.h>
#include <bsp_cshell.h>
#endif

#define ES_TTY_MAJOR		220
#define ES_TTY_MINORS		1
#define ES_TTY_IOCTL_SIGN	0xCAFA0000
#define ES_TTY_IOCTL_CALL	0xCAFA0001
#define VALID_SIGN(tty_termios)	((ES_TTY_MAJOR<<24) | C_BAUD(tty_termios))

typedef struct shell_tty_call_arg {
#ifdef CONFIG_COMPAT
	char __user*func_name;
#else
	char *func_name;
#endif
	int sign_word;
	union {
		int args[6];
		struct {
			int arg1;
			int arg2;
			int arg3;
			int arg4;
			int arg5;
			int arg6;
		};
	};
} shell_tty_call_arg, *p_shell_tty_call_arg;
#ifdef CONFIG_COMPAT
typedef struct shell_tty_call_arg32 {
	compat_caddr_t func_name;
	int sign_word;
	union {
		int args[6];
		struct {
			int arg1;
			int arg2;
			int arg3;
			int arg4;
			int arg5;
			int arg6;
		};
	};
}shell_tty_call_arg32;
#endif
typedef int (*call_ptr)(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

static struct tty_driver *shell_tty_drv;
static struct tty_port shell_tty_port;
static int shell_call(char* func_name, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

static int shell_open(struct tty_struct *tty, struct file *filp)
{
	return 0;
}

static void shell_close(struct tty_struct *tty, struct file *filp)
{

}

#ifdef CONFIG_COMPAT
static int get_cmd32(struct shell_tty_call_arg *kp, struct shell_tty_call_arg32 __user *up)
{
	u32 tmp;

	if (!access_ok(VERIFY_READ, up, sizeof(struct shell_tty_call_arg32)) ||
		get_user(kp->sign_word, &up->sign_word) ||
		get_user(tmp, &up->func_name) ||
		copy_from_user(kp->args, up->args, sizeof(kp->args)))
			return -EFAULT;
	kp->func_name = compat_ptr(tmp);
	return 0;
}
static long shell_ioctl(struct tty_struct *tty, unsigned int cmd, unsigned long arg)
{
	shell_tty_call_arg call_arg;

	void __user *up = compat_ptr(arg);
	
	get_cmd32(&call_arg, up);
	switch (cmd) {
	case ES_TTY_IOCTL_SIGN:
		return VALID_SIGN(tty);

	case ES_TTY_IOCTL_CALL:
		if (call_arg.sign_word & ~VALID_SIGN(tty)) {
			printk(KERN_ERR"Unallowed call\n");
			return -EPERM;
		}
		return shell_call(call_arg.func_name,
				call_arg.arg1,
				call_arg.arg2,
				call_arg.arg3,
				call_arg.arg4,
				call_arg.arg5,
				call_arg.arg6);
		break;

	default:
		printk("shell_ioctl unknown cmd\n");
		break;
	}

	return -ENOIOCTLCMD;
}
#else
static int shell_ioctl(struct tty_struct *tty, unsigned int cmd, unsigned long arg)
{
	p_shell_tty_call_arg call_arg = NULL;

    printk(KERN_DEBUG "received shell ioctl cmd=0x%02x\n, arg=0x%02lx\n",cmd,arg);

	switch (cmd)
	{
	case ES_TTY_IOCTL_SIGN:
		return VALID_SIGN(tty);

	case ES_TTY_IOCTL_CALL:
		call_arg = (p_shell_tty_call_arg) arg;
		if (call_arg->sign_word & ~VALID_SIGN(tty)) {
			printk("Unallowed call\n");
			return -EPERM;
		}

		return shell_call(call_arg->func_name,
				call_arg->arg1,
				call_arg->arg2,
				call_arg->arg3,
				call_arg->arg4,
				call_arg->arg5,
				call_arg->arg6);

	default:
		printk("shell_ioctl unknown cmd\n");
		break;
	}

	return -ENOIOCTLCMD;
}
#endif
static struct tty_operations shell_ops = {
	.open  = shell_open,
	.close = shell_close,
#ifdef CONFIG_COMPAT
	.compat_ioctl = shell_ioctl,
#else
	.ioctl = shell_ioctl,
#endif
};
static const struct tty_port_operations shell_port_ops = {
};


int shell_init(void ) //clean warning
{
	printk("Enter ecall init\n");

	shell_tty_drv = alloc_tty_driver(ES_TTY_MINORS);
	if (!shell_tty_drv) {
		printk("Cannot alloc shell tty driver\n");
		return -1;
	}

	shell_tty_drv->owner = THIS_MODULE;
	shell_tty_drv->driver_name = "es_serial";
	shell_tty_drv->name = "es_tty";
	shell_tty_drv->major = ES_TTY_MAJOR;
	shell_tty_drv->minor_start = 0;
	shell_tty_drv->type = TTY_DRIVER_TYPE_SERIAL;
	shell_tty_drv->subtype = SERIAL_TYPE_NORMAL;
	shell_tty_drv->flags = TTY_DRIVER_REAL_RAW;
	shell_tty_drv->init_termios = tty_std_termios;
	shell_tty_drv->init_termios.c_cflag = B921600 | CS8 | CREAD | HUPCL | CLOCAL;

	tty_set_operations(shell_tty_drv, &shell_ops);
	tty_port_init(&shell_tty_port);
	shell_tty_port.ops = &shell_port_ops;
	tty_port_link_device(&shell_tty_port, shell_tty_drv, 0);

	if (tty_register_driver(shell_tty_drv)) {
		printk("Error registering shell tty driver\n");
		put_tty_driver(shell_tty_drv);
		return -1;
	}

	printk("Finish ecall init\n");

	return 0;
}

static int shell_call(char* func_name, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
	int result = -1;
	call_ptr address;

	if (!func_name)
		goto call_error_input;

#ifdef CONFIG_HISI_BALONG_MODEM
#ifdef CONFIG_CSHELL
	if('^' == func_name[0])
	{
		return send_cmd_ccore(func_name, arg1, arg2, arg3, arg4, arg5, arg6);
	}
#endif
#endif

	address = (call_ptr) kallsyms_lookup_name(func_name);
	if (!address)
		goto call_no_symbol;

	result = address(arg1, arg2, arg3, arg4, arg5, arg6);

	printk("Call %s return, value = 0x%x\n", func_name, result);
	return result;

call_error_input:
	printk("Error input, value = -1\n");

call_no_symbol:
	printk("Invalid function, value = -1\n");
	return -1;
}

module_init(shell_init)
