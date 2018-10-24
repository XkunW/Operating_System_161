/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------0
/*
 * Globals
 */
struct semaphore *NW;
struct semaphore *NE;
struct semaphore *SW;
struct semaphore *SE;
struct semaphore *Entrance_Sem;


/*
 *
 * Constants
 *
 */

/*
 * Number of cars created.
 */

#define NCARS 20


/*
 *
 * Function Definitions
 *
 */

static const char *directions[] = { "N", "E", "S", "W" };

static const char *msgs[] = {
        "approaching:",
        "region1:    ",
        "region2:    ",
        "region3:    ",
        "leaving:    "
};

/* use these constants for the first parameter of message */
enum { APPROACHING, REGION1, REGION2, REGION3, LEAVING };

static void
message(int msg_nr, int carnumber, int cardirection, int destdirection)
{
        kprintf("%s car = %2d, direction = %s, destination = %s\n",
                msgs[msg_nr], carnumber,
                directions[cardirection], directions[destdirection]);
}
 
/*
 * gostraight()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement passing straight through the
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
gostraight(unsigned long cardirection,
           unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */
        
        //(void) cardirection;
        //(void) carnumber;
//------------------------------------------------------------------------------------------------------------------------------0
	/*
	 * cardirection = 0 --> destination = 2
	 * cardirection = 1 --> destination = 3
	 * cardirection = 2 --> destination = 0
	 * cardirection = 3 --> destination = 1
     */
	if(cardirection==0){//NW->SW
        message(APPROACHING, carnumber, cardirection, 2);		
		P(NW);
		message(REGION1, carnumber, cardirection, 2);
		P(SW);
        message(REGION2, carnumber, cardirection, 2);
		V(NW);
        message(LEAVING, carnumber, cardirection, 2);
		V(SW);
	}
	else if(cardirection==1){//NE->NW
        message(APPROACHING, carnumber, cardirection, 3);		
		P(NE);
		message(REGION1, carnumber, cardirection, 3);
		P(NW);        
        message(REGION2, carnumber, cardirection, 3);
		V(NE);
        message(LEAVING, carnumber, cardirection, 3);
		V(NW);
	}
	else if(cardirection==2){//SE->NE
        message(APPROACHING, carnumber, cardirection,0);		
		P(SE);
		message(REGION1, carnumber, cardirection,0);
		P(NE);
        message(REGION2, carnumber, cardirection,0);
		V(SE);
        message(LEAVING, carnumber, cardirection,0);
		V(NE);
	}
	else if(cardirection==3){//SW->SE
        message(APPROACHING, carnumber, cardirection,1);		
		P(SW);
		message(REGION1, carnumber, cardirection,1);
		P(SE);        
        message(REGION2, carnumber, cardirection,1);
		V(SW);        
		message(LEAVING, carnumber, cardirection,1);
		V(SE);
	}
	else{
	kprintf("BUGBUGBUG");
	}
}


/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(unsigned long cardirection,
         unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        //(void) cardirection;
        //(void) carnumber;
			/*
	 * cardirection = 0 --> destination = 1
	 * cardirection = 1 --> destination = 2
	 * cardirection = 2 --> destination = 3
	 * cardirection = 3 --> destination = 0
     */
	if(cardirection==0){//NW->SW->SE
        message(APPROACHING, carnumber, cardirection, 1);		
		P(NW);
		message(REGION1, carnumber, cardirection, 1);
		P(SW);
        message(REGION2, carnumber, cardirection, 1);
		V(NW);
		P(SE);
		message(REGION3, carnumber, cardirection, 1);
        V(SW);
        message(LEAVING, carnumber, cardirection, 1);
		V(SE);
	}
	else if(cardirection==1){//NE->NW->SW
        message(APPROACHING, carnumber, cardirection, 2);		
		P(NE);
		message(REGION1, carnumber, cardirection, 2);
		P(NW);
        message(REGION2, carnumber, cardirection, 2);
		V(NE);
		P(SW);
		message(REGION3, carnumber, cardirection, 2);
        V(NW);
        message(LEAVING, carnumber, cardirection, 2);
		V(SW);
	}
	else if(cardirection==2){//SE->NE->NW
        message(APPROACHING, carnumber, cardirection, 3);		
		P(SE);
		message(REGION1, carnumber, cardirection, 3);
		P(NE);
        message(REGION2, carnumber, cardirection, 3);
		V(SE);
		P(NW);
		message(REGION3, carnumber, cardirection, 3);
        V(NE);
        message(LEAVING, carnumber, cardirection, 3);
		V(NW);
	}
	else if(cardirection==3){//SW->SE->NE
        message(APPROACHING, carnumber, cardirection, 0);		
		P(SW);
		message(REGION1, carnumber, cardirection, 0);
		P(SE);
        message(REGION2, carnumber, cardirection, 0);
		V(SW);
		P(NE);
		message(REGION3, carnumber, cardirection, 0);
        V(SE);
        message(LEAVING, carnumber, cardirection, 0);
		V(NE);
	}
	else{
	kprintf("BUGBUGBUG");
	}
}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnright(unsigned long cardirection,
          unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        //(void) cardirection;
        //(void) carnumber;
//------------------------------------------------------------------------------------------------------------------------------0
	/*
	 * cardirection = 0 --> destination = 3
	 * cardirection = 1 --> destination = 0
	 * cardirection = 2 --> destination = 1
	 * cardirection = 3 --> destination = 2
     */
	if(cardirection==0){//NW
        message(APPROACHING, carnumber, cardirection, 3);		
		P(NW);
		message(REGION1, carnumber, cardirection, 3);
        message(LEAVING, carnumber, cardirection, 3);
		V(NW);
	}
	else if(cardirection==1){//NE
        message(APPROACHING, carnumber, cardirection, 0);		
		P(NE);
		message(REGION1, carnumber, cardirection, 0);
        message(LEAVING, carnumber, cardirection, 0);
		V(NE);
	}
	else if(cardirection==2){//SE
        message(APPROACHING, carnumber, cardirection,1);		
		P(SE);
		message(REGION1, carnumber, cardirection,1);
        message(LEAVING, carnumber, cardirection,1);
		V(SE);
	}
	else if(cardirection==3){//SW
        message(APPROACHING, carnumber, cardirection,2);		
		P(SW);
		message(REGION1, carnumber, cardirection,2);        
		message(LEAVING, carnumber, cardirection,2);
		V(SW);
	}
	else{
	kprintf("BUGBUGBUG");
	}
}


/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long carnumber: holds car id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createcars().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */
 
static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
        int cardirection;
//------------------------------------------------------------------------------------------------------------------------------0
		int turnDirection;

        /*
         * Avoid unused variable and function warnings.
         */

        (void) unusedpointer;
        (void) carnumber;
	(void) gostraight;
	(void) turnleft;
	(void) turnright;

        /*
         * cardirection is set randomly.
         */
        cardirection = random() % 4;
//------------------------------------------------------------------------------------------------------------------------------0
        /*
         * turnDirection is set randomly.
         */
		turnDirection = random() % 3;
		/*
		 * turnDirection = 0 --> gostraight
		 * turnDirection = 1 --> turnleft
		 * turnDirection = 2 --> turnright
		 */

		P(Entrance_Sem);

		if(turnDirection==0)
			gostraight(cardirection, carnumber);
		else if(turnDirection==1)
			turnleft(cardirection, carnumber);
		else if(turnDirection==2)
			turnright(cardirection, carnumber);
		else
			kprintf("THIS IS A BUG");
		
		V(Entrance_Sem);

}


/*
 * createcars()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createcars(int nargs,
           char ** args)
{
        int index, error;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
//-----------------------------------------------------------------------------------------------------------------------------------------------------0
		NW = sem_create("NW", 1);
		NE = sem_create("NE", 1);
		SW = sem_create("SW", 1);
		SE = sem_create("SE", 1);
		Entrance_Sem = sem_create("Entrance_Sem", 3);


        /*
         * Start NCARS approachintersection() threads.
         */

        for (index = 0; index < NCARS; index++) {

                error = thread_fork("approachintersection thread",
                                    NULL,
                                    index,
                                    approachintersection,
                                    NULL
                                    ); 

                /*
                 * panic() on error.
                 */

                if (error) {
                        
                        panic("approachintersection: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

        return 0;
}
