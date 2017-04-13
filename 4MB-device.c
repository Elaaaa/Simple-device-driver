# Simple-device-driver
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61/* forward declaration */
#define BUF_SIZE 4194304
int four_mbyte_open(struct inode *inode, struct file *filep);
int four_mbyte_release(struct inode *inode, struct file *filep);
ssize_t four_mbyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t four_mbyte_write(struct file *filep, const char *buf,size_t count, loff_t *f_pos);
static void four_mbyte_exit(void);

/* definition of file_operation structure */
struct file_operations four_mbyte_fops = {
read:   four_mbyte_read,
write:  four_mbyte_write,
open:   four_mbyte_open,
release:four_mbyte_release
};

char *four_mbyte_data = NULL;

int four_mbyte_open(struct inode *inode, struct file *filep)
{
return 0; // always successful
}

int four_mbyte_release(struct inode *inode, struct file *filep)
{
return 0; // always successful
}
ssize_t four_mbyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
/*Transferring a single byte from the driver buffer to the user space*/

/* Checking the reading position, if its in the beginning of the file as the device reads only one character. If position doesn't match beginning of the file, then return 0 */

  if (*f_pos == 0) {

      *f_pos+=1;
       copy_to_user(buf,four_mbyte_data,1);

    return 1;

  } else {

       return 0;

  }
}
ssize_t four_mbyte_write(struct file *filep, const char *buf,size_t count, loff_t *f_pos)
{

 char *tmp;
/*compute the data from the buffer in which the userspace function writes*/
 tmp=buf;

 if(count>1){
        /*Transfers data from userspace to kernel space*/
        copy_from_user(four_mbyte_data,tmp,1);
        return -ENOSPC;
        }

 else{
        /*Transfers data from userspace to kernel space*/
        copy_from_user(four_mbyte_data,tmp,1);
        return 1;
        }

}

static int four_mbyte_init(void)
{
int result;
// register the device
result = register_chrdev(MAJOR_NUMBER, "four_mbyte",&four_mbyte_fops);
if (result < 0) {
return result;
}
// allocate one byte of memory for storage
// kmalloc is just like malloc, the second parameter is// the type of memory to be allocated.
// To release the memory allocated by kmalloc, use kfree.
four_mbyte_data = kmalloc(BUF_SIZE, GFP_KERNEL);
if (!four_mbyte_data) {
four_mbyte_exit();
// cannot allocate memory
// return no memory error, negative signify a failure
return -ENOMEM;
}
// initialize the value to be X
*four_mbyte_data = 'X';
printk(KERN_ALERT "This is a four Mbyte device module\n");
return 0;
}

static void four_mbyte_exit(void)
{
// if the pointer is pointing to something
if (four_mbyte_data) {
// free the memory and assign the pointer to NULL
kfree(four_mbyte_data);
four_mbyte_data = NULL;
}
// unregister the device
unregister_chrdev(MAJOR_NUMBER, "four_mbyte");
printk(KERN_ALERT "Four Mbyte device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(four_mbyte_init);
module_exit(four_mbyte_exit);

