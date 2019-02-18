#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/sort.h>
#include <linux/slab.h>
#include <linux/time.h>

#define SIZE_LIST_ANIMALS		(50)
#define SIZE_ANIMAL_TYPE		(10)

//#define NSEC_PER_SEC (1000000000)
//#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_MICROSEC (1000)

/*Function prototypes*/
int removeDuplicates(char **listAnimals);
void filterTypeandCount(void);
void filterAnimalType(void);
void filterCount(void);
void noFilter(void);

/*Module information*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hardyk");
MODULE_DESCRIPTION("A kernel list module");

/*Global Variables*/
int i = 0;
static int countArrayElements;
static char *listAnimals[countArrayElements];
module_param_array(listAnimals, charp, &countArrayElements, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myName,"A char array to store animals");

static char *requiredFilter;
module_param(requiredFilter, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(requiredFilter, "A parameter to save required condition to filter the animal list. Supported filters - nofilter, animaltype, count, typeandcount");

static char *requiredType;
module_param(requiredType, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(requiredType, "A parameter to save required type to filter the animal list");

static int requiredCount;
module_param(requiredCount, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(requiredCount, "A parameter to specify required number of count to filter the list. 0 for any count. Specify value otherwise");

/*Structure for linked list, creates two linked lists
 * i)uniqueAnimalsList - contains animals wihtout duplicates
 * ii)filteredAnimalList - contains animals based on the given condition
 */
struct animalsLists{
	int freq;
	char animalType[SIZE_ANIMAL_TYPE];
	struct list_head uniqueAnimalsList;
	struct list_head filteredAnimalsList;
};


/*Initialize list head*/
LIST_HEAD(uniqueList);
LIST_HEAD(filteredList);


/* @brief - this function removes duplicates from the list of the animals
 * @param in - list of the animals
 * @return - 0 if successful, else 1
 */

int removeDuplicates(char **listAnimals){
	
	int index = 0;
	int count = 1;
	int nodeCount = 0;

	struct animalsLists *newAnimal, *pos;

	if(listAnimals == NULL){
		return 1;
	}

	printk(KERN_INFO "Duplicates present in the list\n");

	for(index = 0; index < countArrayElements-1; index++){

		while(0 == strcasecmp(*(listAnimals + index), *(listAnimals + index + 1))){
			++count; 
			++index;
		}

		if(count > 1){
			printk(KERN_INFO "%s ", *(listAnimals + index));
		}

		/*Create a new node for the list*/
		newAnimal = (struct animalsLists *)kmalloc(sizeof(struct animalsLists), GFP_KERNEL);
		strcpy(newAnimal->animalType, *(listAnimals + index));
		newAnimal->freq = count;
		count = 1;

		/*Insert the new node at the tail of the list*/
		list_add_tail(&newAnimal->uniqueAnimalsList, &uniqueList);
		
	}

	printk(KERN_INFO "The Unique List is:\n");
	list_for_each_entry(pos, &uniqueList, uniqueAnimalsList){
		printk("Animal Type = %s \t and \t Frequency = %d\n", pos->animalType, pos->freq);
		nodeCount++;
	}

	printk(KERN_INFO "Total Number of Node in the uniqueList = %d\n", nodeCount);
	printk(KERN_INFO "Total Memory allocated in the uniqueList = %d\n", nodeCount * sizeof(struct animalsLists));

	return 0;
}


/*	@brief - this function filters the given list and creates a
 * 	new list which contains only a specific type of a animal of 
 *  given frequency
 *	@param - void
 *  @return - void
 */

void filterTypeandCount(){
	
	int nodeCount = 0;
	struct animalsLists *newAnimal, *pos;

	printk(KERN_INFO "Filter Criteria - Type and Count\n");
	printk(KERN_INFO"Data Structure used - Linked List\n");

	list_for_each_entry(pos, &uniqueList, uniqueAnimalsList){
		
		/*Check if the node in the unique list has same animal type and frequency is greater
		than required count. if yes then add the element to a new filtered list*/
		if((0 == strcasecmp(pos->animalType, requiredType)) && (pos->freq > requiredCount)){

			newAnimal = (struct animalsLists *)kmalloc(sizeof(struct animalsLists), GFP_KERNEL);
			strcpy(newAnimal->animalType, pos->animalType);

			/*Insert the new node at the tail of the list*/
			list_add_tail(&newAnimal->filteredAnimalsList, &filteredList);
		}
	}


	printk(KERN_INFO "Contents of the list after filtering with type = %s and count = %d\n", requiredType, requiredCount);

	list_for_each_entry(pos, &filteredList, filteredAnimalsList){
		printk("%s ", pos->animalType);
		nodeCount++;
	}

	printk(KERN_INFO "Total number of nodes in the filtered list = %d\n", nodeCount);
	printk(KERN_INFO "Total memory allocated in the filtered list = %d\n", nodeCount * sizeof(struct animalsLists));

}

/*	@brief - this function filters the given list and creates a
 * 	new list which contains only a specific type of a animal
 *	@param - void
 *  @return - void
 */
void filterAnimalType(){
	
	int nodeCount = 0;

	struct animalsLists *newAnimal, *pos;

	printk(KERN_INFO "Filter Criteria - Type\n");
	printk(KERN_INFO "Data Structure used - Lined List\n");

	list_for_each_entry(pos, &uniqueList, uniqueAnimalsList){
		
		/*Check if the node in the unique list has same animal type and frequency is greater
		than required count. if yes then add the element to a new filtered list*/
		if(0 == strcasecmp(pos->animalType, requiredType)){

			newAnimal = (struct animalsLists *)kmalloc(sizeof(struct animalsLists), GFP_KERNEL);
			strcpy(newAnimal->animalType, pos->animalType);

			/*Insert the new node at the tail of the list*/
			list_add_tail(&newAnimal->filteredAnimalsList, &filteredList);
		}
	}


	printk(KERN_INFO "Contents of the list after filtering with type = %s\n", requiredType);

	list_for_each_entry(pos, &filteredList, filteredAnimalsList){
		printk("%s ", pos->animalType);
		nodeCount++;
	}

	printk(KERN_INFO "Total number of nodes in the filtered list = %d\n", nodeCount);
	printk(KERN_INFO "Total memory allocated in the filtered list = %d\n", nodeCount * sizeof(struct animalsLists));
}

/*	@brief - this function filters the given list and creates a
 * 	new list which contains only a specific type of a animal of 
 *  frequency greater than given frequency
 *	@param - void
 *  @return - void
 */

void filterCount(){
	
	int nodeCount = 0;
	struct animalsLists *newAnimal, *pos;

	printk(KERN_INFO "Filter Criteria - Count\n");
	printk(KERN_INFO"Data Structure used - Linked List\n");

	list_for_each_entry(pos, &uniqueList, uniqueAnimalsList){
		
		/*Check if the node in the unique list has a frequency greater
		than required count. if yes then add the element to a new filtered list*/
		if(pos->freq > requiredCount){

			newAnimal = (struct animalsLists *)kmalloc(sizeof(struct animalsLists), GFP_KERNEL);
			strcpy(newAnimal->animalType, pos->animalType);

			/*Insert the new node at the tail of the list*/
			list_add_tail(&newAnimal->filteredAnimalsList, &filteredList);
		}
	}


	printk(KERN_INFO "Contents of the list after filtering with count = %d\n", requiredCount);

	list_for_each_entry(pos, &filteredList, filteredAnimalsList){
		printk("%s ", pos->animalType);
		nodeCount++;
	}

	printk(KERN_INFO "Total number of nodes in the filtered list = %d\n", nodeCount);
	printk(KERN_INFO "Total memory allocated in the filtered list = %d\n", nodeCount * sizeof(struct animalsLists));

}

/*	@brief - this function filters the given list and creates a
 * 	new list which is same as the original list with duplicates removed
 *	@param - void
 *  @return - void
 */
void noFilter(){

	int nodeCount = 0;

	struct animalsLists *newAnimal, *pos;

	printk(KERN_INFO "Filter Criteria - No Filter\n");
	printk(KERN_INFO "Data Structure used - Linked List\n");

	/*Create a new list with all the duplicates removed*/
	list_for_each_entry(pos, &uniqueList, uniqueAnimalsList){
		
		newAnimal = (struct animalsLists *)kmalloc(sizeof(struct animalsLists), GFP_KERNEL);
		strcpy(newAnimal->animalType, pos->animalType);

		/*Insert the new node at the tail of the list*/
		list_add_tail(&newAnimal->filteredAnimalsList, &filteredList);
	}

	printk(KERN_INFO "Contents of the list after filtering with type = %s\n", requiredType);

	list_for_each_entry(pos, &filteredList, filteredAnimalsList){
		printk("%s ", pos->animalType);
		nodeCount++;
	}

	printk(KERN_INFO "Total number of nodes in the filtered list = %d\n", nodeCount);
	printk(KERN_INFO "Total memory allocated in the filtered list = %d\n", nodeCount * sizeof(struct animalsLists));
}

int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }
  else
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }

  return(1);
}



/*	@brief - this function compares two strings
 *	@param in - two strings to be compared (s1 and s2)
 *	@return - returns value < 0 if s1 < s2
 *					  value = 0 if s1 = s2
 *					  value > 0 if s1 > s2
 */
static int compareStrings(const void *string1, const void *string2){
	
	return strcasecmp(*(char**)string1, *(char**)string2);
}

/*	@brief - this function is called when 
 *	the module is loaded.
 *	returns 0 if successfully loaded, nonzero otherwise.
 */
static int myListModule_init(void){

	int ret = 0;

	struct timespec start_time = {0, 0};
    struct timespec finish_time = {0, 0};
    struct timespec delta_time = {0, 0};

	/*Record start time*/
    getrawmonotonic(&start_time);

	printk(KERN_INFO "Initalizing my module - myListModule\n");

	printk(KERN_INFO "Data Structure used - Linked List\n");

	printk(KERN_INFO "Entries in the Animal list\n");
	for(i = 0; i < countArrayElements; i++){
		printk(KERN_INFO "%s ", *(listAnimals + i));
	}

	/*First sort the array alphabetically*/
	sort(listAnimals, countArrayElements-1, sizeof(const char*), &compareStrings, NULL);

	/*Remove duplicates and create a unique List of Animals*/
	ret = removeDuplicates(listAnimals);

	/*Create Filtered list based on the given requirement*/
	if(0 == strcmp(requiredFilter, "typeandcount")){
		filterTypeandCount();
	}
	else if(0 == strcmp(requiredFilter, "animaltype")){
		filterAnimalType();
	}
	else if(0 == strcmp(requiredFilter, "count")){
		filterCount();
	}
	else if(0 == strcmp(requiredFilter, "nofilter")){
		noFilter();
	}
	else{
		printk(KERN_WARNING "Invalid Parameters\n");
	}

	/*Record end tiem and calculates total time*/
	getrawmonotonic(&finish_time);
	delta_t(&finish_time, &start_time, &delta_time);
	printk(KERN_INFO "\nTotal time taken to perform module initilization = %ld sec, %ld msec (%ld microsec)\n", delta_time.tv_sec, (delta_time.tv_nsec / NSEC_PER_MSEC), (delta_time.tv_nsec / NSEC_PER_MICROSEC));	
	return 0;
}

/*	
 * This function is called when a module is removed
 */
static void myListModule_exit(void){

    struct list_head *pos, *pos_1;
	int freedNode = 0;

    struct timespec start_time = {0, 0};
    struct timespec finish_time = {0, 0};
    struct timespec delta_time = {0, 0};

	/*Record start time*/
    getrawmonotonic(&start_time);
 
	/*Free used resources*/
	printk(KERN_INFO "Freeeing allocated memory\n");

	/*Free memory of Unique list*/
	list_for_each_safe(pos, pos_1, &uniqueList){
		struct animalsLists *animal;

		animal = list_entry(pos, struct animalsLists, uniqueAnimalsList);
		//freedMemory += sizeof(animal);
		freedNode++;
		list_del(pos);
		kfree(animal);
	}	
	
	printk(KERN_INFO "Amount of memory freed from the uniqueAnimalsList = %d\n", freedNode * sizeof(struct animalsLists));

	/*free memory of the filtered list*/
	freedNode = 0;
	list_for_each_safe(pos, pos_1, &filteredList){
		struct animalsLists *animal;

		animal = list_entry(pos, struct animalsLists, filteredAnimalsList);
		//freedMemory += sizeof(animal);
		freedNode++;
		list_del(pos);
		kfree(animal);
	}	
	
	printk(KERN_INFO "Amount of memory freed from the filteredAnimalsList = %d\n", freedNode * sizeof(struct animalsLists));

	printk(KERN_INFO "Exiting my module - myListModule\n");

	/*Record end tiem and calculates total time*/
	getrawmonotonic(&finish_time);
	delta_t(&finish_time, &start_time, &delta_time);
	printk(KERN_INFO "\nTotal time taken to remove the module = %ld sec, %ld msec (%ld microsec)\n", delta_time.tv_sec, (delta_time.tv_nsec / NSEC_PER_MSEC), (delta_time.tv_nsec / NSEC_PER_MICROSEC));
}


module_init(myListModule_init);
module_exit(myListModule_exit);

