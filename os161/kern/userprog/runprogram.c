/*
 * Sample/test code for running a user program.  You can use this for
 * reference when implementing the execv() system call. Remember though
 * that execv() needs to do more than this function does.
 */

#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>

/*
 * Load program "progname" and start running it in usermode.
 * Does not return except on error.
 *
 * Calls vfs_open on progname and thus may destroy it.
 */

//void printbitmap();


int
runprogram(char *progname, char **argv, int argc){
//kprintf("\nEntering runprogram: (runprogram.c 26)\n");

	if(curthread->t_vmspace != NULL){
		as_destroy(curthread->t_vmspace);
		curthread->t_vmspace = NULL;
	}

	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, &v);

	
	if (result) {
		//kprintf("\n-----------------------------------1\n");	
		return result;
	}

	/* We should be a new thread. */
	assert(curthread->t_vmspace == NULL);

	/* Create a new address space. */
	curthread->t_vmspace = as_create();
	if (curthread->t_vmspace==NULL) {
		//vfs_close(v);
		//kprintf("\n-----------------------------------2\n");
		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_vmspace);

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		//vfs_close(v);
		//kprintf("\n-----------------------------------3\n");
		return result;
	}

	/* Done with the file now. */
	//vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		//kprintf("\n-----------------------------------4\n");
		return result;
	}
	
	/* Copy arguments from kernel level back into the user level */
	int i = 0;
	while(i < argc){
		int length = 1 + strlen(argv[i]);
		int rem = length % 4;
		if(rem){
			length = (length - rem)/4 + 2;
		} else {
			length = length/4 + 1;
		}
		stackptr -= length * 4;
		
				
		result = copyout(argv[i], (userptr_t)stackptr, strlen(argv[i])+1);
		


		if(result){
			//kprintf("\n%d\n", result);	
			//kprintf("\n-----------------------------------5\n");	
			return result;
		}
		argv[i] = (char*)stackptr;
		i++;
	}
	//argv[i] = NULL;

	stackptr -= argc * 4;

	result = copyout(argv, (userptr_t)stackptr, argc*4);
	if(result){
		//kprintf("\n-----------------------------------6\n");
		return result;
	}
	//printbitmap();

//kprintf("\nEntering user mode: (runprgram.c 114)\n");
	/* Warp to user mode. */
	md_usermode(argc /*argc*/, (userptr_t)stackptr /*userspace addr of argv*/,
		    stackptr, entrypoint);
	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}


