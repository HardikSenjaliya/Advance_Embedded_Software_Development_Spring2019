#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hardyk");
MODULE_DESCRIPTION("A Timer module");

/*Timer*/
struct timer_list my_timer;
static int count_callbacks = 0;

static char *myName;
module_param(myName, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myName,"A variable to store name");

static int timerExpiry;
module_param(timerExpiry, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(timerExpiry,"A variable to store timer expiration time");

void myTimerFunction(unsigned long data){

	int err;

	err = mod_timer(&my_timer, jiffies + msecs_to_jiffies(timerExpiry));
	if(err){
		printk(KERN_ERR "Error in setting expiration time\n");
	}

	++count_callbacks;

	printk(KERN_INFO "My Timer Handler called\n");
	printk(KERN_INFO "Name = %s\n", myName);
	printk(KERN_INFO "Number of times the timer has fired = %d", count_callbacks);
}

/*	@brief - this function is called when 
 *	the module is loaded.
 *	returns 0 if successfully loaded, nonzero otherwise.
 */
static int myModule_init(void){

	int err;

	printk(KERN_INFO "Initalizing my module - myTimerModule\n");

	/*Initialize timer*/
	setup_timer(&my_timer, myTimerFunction, 0);

	/*Set expiration time for the timer*/
	err = mod_timer(&my_timer, jiffies + msecs_to_jiffies(timerExpiry));
	if(err){
		printk(KERN_ERR "Error in setting expiration time\n");
	}

	return 0;
}

/*	
 * This function is called when a module is removed
 */

static void myModule_exit(void){

	/*Free used resources*/
	del_timer(&my_timer);
	
	printk(KERN_INFO "Exiting my module - myTimerModule\n");
}


module_init(myModule_init);
module_exit(myModule_exit);


