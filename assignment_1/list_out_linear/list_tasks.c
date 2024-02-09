#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("List tasks with their command and PID, and human-readable state.");

static int __init list_tasks_start(void) {
    struct task_struct *task;

    printk(KERN_INFO "Loading list_tasks module...\n");
    printk(KERN_INFO "Command\t\tState\tPID\n");

    for_each_process(task) {
        // Use task_state_to_char() to get the task's state as a character
        char task_state = task_state_to_char(task);
        
        // task->comm: command name of the task
        // task->pid: PID of the task
        // task_state: human-readable character representing the task's state
        printk(KERN_INFO "%s\t\t%c\t%d\n", task->comm, task_state, task->pid);
    }

    return 0; // A non 0 return means init_module failed; module can't be loaded.
}

static void __exit list_tasks_end(void) {
    printk(KERN_INFO "Removing list_tasks module...\n");
}

module_init(list_tasks_start);
module_exit(list_tasks_end);

