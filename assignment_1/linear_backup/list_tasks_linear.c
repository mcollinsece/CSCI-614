#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/init.h>

static int __init list_tasks_linear_init(void) {
    struct task_struct *task;

    printk(KERN_INFO "Loading module to list tasks linearly.\n");
    for_each_process(task) {
        /* on each iteration task points to the next task */
        printk(KERN_INFO "Command: %-20s State: %ld\tPID: %d\n", task->comm, task->__state, task->pid);
    }

    return 0; // A non 0 return means init_module failed; module can't be loaded.
}

static void __exit list_tasks_linear_exit(void) {
    printk(KERN_INFO "Removing module that listed tasks linearly.\n");
}

module_init(list_tasks_linear_init);
module_exit(list_tasks_linear_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("List tasks linearly");
MODULE_AUTHOR("Your Name");
