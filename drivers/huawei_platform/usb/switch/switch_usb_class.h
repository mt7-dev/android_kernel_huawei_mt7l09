
#define FSA9685_OPEN        0
#define FSA9685_USB1        1
#define FSA9685_USB2        2
#define FSA9685_MHL     4
#define FSA9685_USB1_ID_TO_IDBYPASS             1

struct switch_usb_info {
    struct atomic_notifier_head charger_type_notifier_head;
    spinlock_t reg_flag_lock;
};

extern int fsa9685_manual_sw(int input_select);
extern int fsa9685_manual_detach(void);
