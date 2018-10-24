/*
 * catlock.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use LOCKS/CV'S to solve the cat syncronization problem in 
 * this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>

#include <synch.h>

/*
 * 
 * Constants
 *
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2

//-----------------------------------------------------------------------------------------------------------------------------------------------------0
/*
 * Globals
 */
int numOfFullCat;//Indicating the total number of cats that finished 4 iteration of eating
int numOfFullMouse;//Indicating the total number of mice that finished 4 iteration of eating

struct lock *species;//Lock that indicates one species of animal (cat or mouse) is eating.

struct lock *writer;//Lock that allows only one thread modifying the global value(s) at a time.

struct lock *bowl1, *bowl2;

int foodBowl_1_ok, foodBowl_2_ok;//"0" means nobody is occupying the food bowl, "1" means someone is eating at the bowl.

int is_there_cat;
int is_there_mouse;

/*
 * 
 * Function Definitions
 * 
 */

/* who should be "cat" or "mouse" */
static void
lock_eat(const char *who, int num, int bowl, int iteration)
{
        kprintf("%s: %d starts eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
        clocksleep(1);
        kprintf("%s: %d ends eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
}


//The following two functions handle "cat go to eat" and "mouse go to eat" respectively.
//-----------------------------------------------------------------------------------------------------------------------------------------------------0
static int cat_go_eat(int catNum, int iteration, struct lock *bowl1, struct lock *bowl2){
	if(foodBowl_1_ok==0){
		lock_acquire(bowl1);
		foodBowl_1_ok = 1;
		lock_eat("cat", catNum, 1, iteration);
		foodBowl_1_ok = 0;
		lock_release(bowl1);
		return 0;
	}
	else if(foodBowl_2_ok==0){
		lock_acquire(bowl2);
		foodBowl_2_ok = 1;
		lock_eat("cat", catNum, 2, iteration);
		foodBowl_2_ok = 0;
		lock_release(bowl2);
		return 0;
	}
	else{return 1;}
}

static int mouse_go_eat(int mouseNum, int iteration, struct lock *bowl1, struct lock *bowl2){
	if(foodBowl_1_ok==0){
		lock_acquire(bowl1);
		foodBowl_1_ok = 1;
		lock_eat("mouse", mouseNum, 1, iteration);
		foodBowl_1_ok = 0;
		lock_release(bowl1);	
		return 0;
	}
	else if(foodBowl_2_ok==0){
		lock_acquire(bowl2);
		foodBowl_2_ok = 1;
		lock_eat("mouse", mouseNum, 2, iteration);
		foodBowl_2_ok = 0;
		lock_release(bowl2);
		return 0;
	}
	else{return 1;}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------0

/*
 * catlock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS -
 *      1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
catlock(void * unusedpointer, 
        unsigned long catnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        //(void) unusedpointer;
        //(void) catnumber;

//-----------------------------------------------------------------------------------------------------------------------------------------------------0
		int iteration = 0;
		int res;
		while(iteration<4){
			res = 0;
			lock_acquire(species);//wait until no mouse is eating.
			if(((foodBowl_1_ok==0)||(foodBowl_2_ok==0))&&(is_there_mouse==0)){
				
				is_there_cat++;
				lock_release(species);
				res = cat_go_eat(catnumber, iteration, bowl1, bowl2);
				
				is_there_cat--;
				
				if(res==0){
				iteration++;}
			}

			else{lock_release(species);}
			thread_yield();		
		}

		lock_acquire(writer);
		numOfFullCat++;
		lock_release(writer);
}
	

/*
 * mouselock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
mouselock(void * unusedpointer,
          unsigned long mousenumber)
{
        /*
         * Avoid unused variable warnings.
         */
        
        //(void) unusedpointer;
        //(void) mousenumber;
//-----------------------------------------------------------------------------------------------------------------------------------------------------0
		int iteration = 0;
		int res;
		while(iteration<4){
			res = 0;
			lock_acquire(species);//wait until no cat is eating.
			if(((foodBowl_1_ok==0)||(foodBowl_2_ok==0))&&(is_there_cat==0)){
				is_there_mouse++;
				lock_release(species);
				res = mouse_go_eat(mousenumber, iteration, bowl1, bowl2);
				is_there_mouse--;				
				
				if(res==0)
					iteration++;
			}
			else{lock_release(species);}
			thread_yield();		
		}
		lock_acquire(writer);
		numOfFullMouse++;
		lock_release(writer);
}


/*
 * catmouselock()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catlock() and mouselock() threads.  Change
 *      this code as necessary for your solution.
 */

int
catmouselock(int nargs,
             char ** args)
{
        int index, error;
   
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

//-----------------------------------------------------------------------------------------------------------------------------------------------------0
		species = lock_create("species");
		writer = lock_create("writer");
		bowl1 = lock_create("bowl1");
		bowl2 = lock_create("bowl2");

		numOfFullCat = 0;
		numOfFullMouse = 0;
		is_there_cat = 0;
		is_there_mouse = 0;

		foodBowl_1_ok = 0;//Initialization: food bowl 1 is ready for serving
		foodBowl_2_ok = 0;//Initialization: food bowl 2 is ready for serving
   
        /*
         * Start NCATS catlock() threads.
         */

        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catlock thread", 
                                    NULL, 
                                    index, 
                                    catlock, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catlock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        /*
         * Start NMICE mouselock() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mouselock thread", 
                                    NULL, 
                                    index, 
                                    mouselock, 
                                    NULL
                                    );
      
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mouselock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

		
		
		/*while((numOfFullCat!=NCATS)||(numOfFullMouse!=NMICE)){
			thread_yield();
		}
		//Clean up all the locks that used
		lock_destroy(species);
		lock_destroy(writer);*/

        return 0;
}

/*
 * End of catlock.c
 */
