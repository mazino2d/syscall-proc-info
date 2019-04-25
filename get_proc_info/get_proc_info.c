#include <linux/kernel.h>
#include <linux/sched.h>    // task_struct
#include <linux/errno.h>    // EINVAL
#include <linux/string.h>   // strcpy
#include <linux/syscalls.h> // SYSCALL_DEFINE2
#include <linux/uaccess.h>  // copy_to_user


struct proc_info { //info about a single process
    pid_t pid;                          //pid of the process
    char name[16];                      //file name of the program executed
};

struct procinfos { //info about processes we need
    long   studentID;                   //for the assignment testing
    struct proc_info proc;              // process with pid or current process
    struct proc_info parent_proc;       // parent process
    struct proc_info oldest_child_proc; // oldest child process
};


/**
 * * @param pid  : pid  >= -1 (Process ID) 
 * * @param info : info != NULL
 */
SYSCALL_DEFINE2(get_proc_info, pid_t, pid, struct procinfos*, info)
{
    // * Initialize varible
    struct task_struct  *proc   = NULL; // process with pid or current process
    struct task_struct  *parent = NULL; // parent process
    struct task_struct  *child  = NULL; // oldest child process
    struct procinfos kpinfo;

    // * My student ID - 1711807
    kpinfo.studentID = 1711807;
    printk(KERN_INFO "1711807");

    // * System call exception
    if(pid < -1 || info == NULL) return EINVAL;

    // * Current process or process with pid
    if(pid == -1) proc = current;
    if(pid ==  0) proc = find_task_by_vpid(1)->parent;
    if(pid >=  1) proc = find_task_by_vpid(pid);
    if(proc == NULL)   return EINVAL;

    kpinfo.proc.pid = proc->pid;
    strcpy(kpinfo.proc.name, proc->comm); 

    // * Parent process
    parent = proc->parent;

    if(proc == parent) {
        kpinfo.parent_proc.pid = -1;
        strcpy(kpinfo.parent_proc.name, "NULL");
    } else {
        kpinfo.parent_proc.pid = parent->pid;
        strcpy(kpinfo.parent_proc.name, parent->comm);
    }
    

    // * Oldest child process
    if(list_empty_careful(&proc->children)) {
        kpinfo.oldest_child_proc.pid =  -1;
        strcpy(kpinfo.oldest_child_proc.name, "NULL"); 
    }
    else {
        child  = list_last_entry(&proc->children, struct task_struct, sibling); 

        kpinfo.oldest_child_proc.pid = child->pid;
        strcpy(kpinfo.oldest_child_proc.name, child->comm); 
    }

    // * Copy data from kernel space to user space
    copy_to_user(info, &kpinfo, sizeof(struct procinfos));

    return 0;
}
