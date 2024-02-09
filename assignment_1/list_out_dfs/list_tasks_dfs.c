#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("DFS iteration over tasks");

// Function to perform DFS on the task list
static void dfs(struct task_struct *task) {
    struct task_struct *child;
    struct list_head *list;

    // Output the current task's name, state, and PID
    //printk(KERN_INFO "%s\t\t%ld\t%d\n", task->comm, task->__state, task->pid);
    //printk(KERN_INFO "%-16s\t%-6u\t%d\n", task->comm, task->__state, task->pid);

    printk(KERN_INFO "%-16s\t%x\t%d\n", task->comm, task->__state, task->pid);

    // Recursively apply DFS to each child
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        dfs(child);
    }
}

static int __init dfs_init(void) {
    printk(KERN_INFO "Loading DFS module...\n");
    printk(KERN_INFO "Command\t\tState\tPID\n");

    // Start DFS from the init_task
    dfs(&init_task);

    return 0; // A non 0 return means init_module failed; module can't be loaded.
}

static void __exit dfs_exit(void) {
    printk(KERN_INFO "Removing DFS module...\n");
}

module_init(dfs_init);
module_exit(dfs_exit);

