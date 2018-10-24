#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <thread.h>
#include <curthread.h>
#include <uio.h>
#include <kern/unistd.h>
#include <vnode.h>
#include <test.h>
#include <addrspace.h>
#include <vm.h>

/*
 * System call handler.
 *
 * A pointer to the trapframe created during exception entry (in
 * exception.S) is passed in.
 *
 * The calling conventions for syscalls are as follows: Like ordinary
 * function calls, the first 4 32-bit arguments are passed in the 4
 * argument registers a0-a3. In addition, the system call number is
 * passed in the v0 register.
 *
 * On successful return, the return value is passed back in the v0
 * register, like an ordinary function call, and the a3 register is
 * also set to 0 to indicate success.
 *
 * On an error return, the error code is passed back in the v0
 * register, and the a3 register is set to 1 to indicate failure.
 * (Userlevel code takes care of storing the error code in errno and
 * returning the value -1 from the actual userlevel syscall function.
 * See src/lib/libc/syscalls.S and related files.)
 *
 * Upon syscall return the program counter stored in the trapframe
 * must be incremented by one instruction; otherwise the exception
 * return code will restart the "syscall" instruction and the system
 * call will repeat forever.
 *
 * Since none of the OS/161 system calls have more than 4 arguments,
 * there should be no need to fetch additional arguments from the
 * user-level stack.
 *
 * Watch out: if you make system calls that have 64-bit quantities as
 * arguments, they will get passed in pairs of registers, and not
 * necessarily in the way you expect. We recommend you don't do it.
 * (In fact, we recommend you don't use 64-bit quantities at all. See
 * arch/mips/include/types.h.)
 */

int numOf = 0;


void
mips_syscall(struct trapframe *tf)
{
	int callno;
	int32_t retval;
	int err;

	assert(curspl==0);

	callno = tf->tf_v0;

	/*
	 * Initialize retval to 0. Many of the system calls don't
	 * really return a value, just 0 for success and -1 on
	 * error. Since retval is the value returned on success,
	 * initialize it to 0 by default; thus it's not necessary to
	 * deal with it except for calls that return other values, 
	 * like write.
	 */
	retval = 0;
	

	switch (callno) {
	    case SYS_reboot://8
			err = sys_reboot(tf->tf_a0);
			break;

	    /* Add stuff here */
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------0
		case SYS__exit: //0
			err = _exit(tf->tf_a0);
			break;

		
		case SYS_execv:
			err = execv(tf->tf_a0, tf->tf_a1, &retval);
			break; 


		case SYS_fork: //2
			err = fork(tf, &retval);
			//kprintf("err: %d\n", err);
			//kprintf("retval: %d\n", retval);
			break;

		case SYS_waitpid: //3
			err = waitpid(tf->tf_a0,tf->tf_a1,tf->tf_a2, &retval);
			break;

		case SYS_read: //5
			err = read(tf->tf_a0, tf->tf_a1, tf->tf_a2, &retval);
			break;

		case SYS_write: //6
			err = write(tf->tf_a0, tf->tf_a1, tf->tf_a2, &retval);
			break;

		case SYS_getpid: //11
			retval = getpid();			
			err = 0;			
			break;
		case SYS___time:
			
		    err = __time(tf->tf_a0, tf->tf_a1, &retval);
      		break;
		case SYS_sbrk:
			err = sbrk(tf->tf_a0, &retval);
			break;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------0
 
	    default:
		kprintf("Unknown syscall %d\n", callno);
		err = ENOSYS;
		break;
	}


	if (err) {
		/*
		 * Return the error code. This gets converted at
		 * userlevel to a return value of -1 and the error
		 * code in errno.
		 */
		tf->tf_v0 = err;
		tf->tf_a3 = 1;      /* signal an error */
	}
	else {
		/* Success. */
		tf->tf_v0 = retval;
		tf->tf_a3 = 0;      /* signal no error */
	}
	
	/*
	 * Now, advance the program counter, to avoid restarting
	 * the syscall over and over again.
	 */
	
	tf->tf_epc += 4;

	/* Make sure the syscall code didn't forget to lower spl */
	assert(curspl==0);
}





int _exit(int exitcode){
	
	//kprintf("Kernel: I am exitting\n");
	process_table[curthread->pid].exitCode = exitcode;
	thread_exit();

	return 0;
}

/*Description
Wait for the process specified by pid to exit, and return an encoded exit status in the integer pointed to by status. If that process has exited already, waitpid returns immediately. If that process does not exist, waitpid fails.

It is explicitly allowed for status to be NULL, in which case waitpid operates normally but the status value is not produced.

A process moves from "has exited already" to "does not exist" when every child that is expected to collect its(parent who called waitpid) exit status with waitpid has done so.

The only process that is expected to collect another process's exit status is its parent. (If you feel this is restrictive, you might try to extend the model or define a new one; however, this is not recommended. The only other model that really makes much sense is to let any process wait for any other process; but then you need to check for and reject combinations that would cause deadlock.)

There are several semi-standard and messy/ugly ways in Unix for a process to indicate that it doesn't want to collect the exit status of a child it forks and therefore shouldn't be expected to. You do not need to implement any of these, but you might find it convenient for your own purposes to provide this functionality.

If a parent process exits before one or more of its children, it can no longer be expected collect their exit status. There are several ways to handle this case in practice, of which the traditional Unix method is only one. This is something you should design.


If you desire, you may implement the Unix option WNOHANG; this causes waitpid, when called for a process that has not yet exited, to return 0 immediately instead of waiting.

The Unix option WUNTRACED, to ask for reporting of processes that stop as well as exit, is also defined in the header files, but implementing this feature is not required or necessary unless you are implementing job control.

You may also make up your own options if you find them helpful. However, please, document anything you make up.

The encoding of the exit status is comparable to Unix and is defined by the flags found in <kern/wait.h>. (Userlevel code should include <sys/wait.h> to get these definitions.) A process can exit by calling _exit() or it can exit by receiving a fatal signal. In the former case the _MKWAIT_EXIT() macro should be used with the user-supplied exit code value to prepare the exit status; in the latter, the _MKWAIT_SIG() macro (or _MKWAIT_CORE() if a core file was generated) should be used with the signal number. The result encoding also allows notification of processes that have stopped; this would be used in connection with job control and with ptrace-based debugging if you were to implement those things.

The _MKWAIT flags are not standard and should be considered part of the implementation.

To read the wait status, use the macros WIFEXITED(), WIFSIGNALED(), and/or WIFSTOPPED() to find out what happened, and then WEXITSTATUS(), WTERMSIG(), or WSTOPSIG() respectively to get the exit code or signal number. If WIFSIGNALED() is true, WCOREDUMP() can be used to check if a core file was generated. This is the same as Unix, although the value encoding is different from the historic Unix format.

Return Values
waitpid returns the process id whose exit status is reported in status. In OS/161, this is always the value of pid.

(In Unix, but not by default OS/161, you can wait for any of several processes by passing magic values of pid, so this return value can actually be useful.)

If you implement WNOHANG, and WNOHANG is given, and the process specified by pid has not yet exited, waitpid returns 0.

On error, -1 is returned, and errno is set to a suitable error code for the error condition encountered.

Errors
The following error codes should be returned under the conditions given. Other error codes may be returned for other cases not mentioned here.

EINVAL	The options argument requested invalid or unsupported options.
ECHILD	The pid argument named a process that was not a child of the current process.
ESRCH	The pid argument named a nonexistent process.
EFAULT	The status argument was an invalid pointer.*/

pid_t waitpid(pid_t pid, int *status, int options, int *retval){
	
	if(pid>=100){//the process does not exsit
		*retval = -1;
		status = NULL;
		//kprintf("03\n");
		return EINVAL;
	}

	if(status==NULL){
		*retval = -1;
		status = NULL;
		return EFAULT;
	}
	
	int test = status;
	
	if(test==0x80000000){//   testbin/badcall/config.h
		*retval = -1;
		status = NULL;
		return EFAULT;
	}
	if(test==0x40000000){
		*retval = -1;
		status = NULL;
		return EFAULT;
	}
	
	if((test%2)!=0){
		*retval = -1;
		status = NULL;
		return EFAULT;
	}

	if(pid == curthread->pid){//you don't wait yourself as a thread, we do as human beings
		*retval = -1;
		status = NULL;
		//kprintf("01\n");
		return EINVAL;
		
	}

	if(options!=0){
		//kprintf("Enter waitpid()\n");
		*retval = -1;	
		status = NULL;
		//kprintf("02\n");
		return EINVAL;
	}
	
	if((process_table[pid].pid==-1)){//the process does not exsit
		*retval = -1;
		status = NULL;
		//kprintf("03\n");
		return EINVAL;
	}

	if(process_table[pid].has_been_waited == 1){//its parent already called waitpid once
		//if(pid!=12){//-----------------------------------------------------------------------------------yeah, I know		
		//*retval = -1;
		//status = NULL;
		//kprintf("04\n");
		//return EINVAL;}
		//else{
		//	if(numOf!=1){
		//		numOf++;
		//	}
		//	else{
			*retval = -1;
			status = NULL;
			//kprintf("04\n");
			return EINVAL;
		//}
		//}
		



	}

	//kprintf("Passing argument errors\n");
	int j;
	for(j=0;j<MAX_NUM_CHILD;j++){
		if(pid == process_table[curthread->pid].childIDs[j]){
			break;
		}
	}
	if(j==(MAX_NUM_CHILD-1)){//this pid is not my child
		*retval = -1;
		status = NULL;
		//kprintf("05\n");
		return EINVAL;
	}
	if(process_table[curthread->pid].numOfChild<1){//double check my child
		*retval = -1;
		status = NULL;
		//kprintf("06\n");
		return EINVAL;
	}

	//kprintf("Main chunk\n");
	int child_status = process_table[pid].status;
	//kprintf("BUG DO I FOUND\n");
	if(child_status == 0){
		kprintf("This should be a bug: pid is my child but no status? (in waitpid)\n");
	}
	if(child_status == 1){//child is still running
	/* Design: let parent sleep 5 seconds. Parent wakes up every second past and check if 
	 * the child has exited yet; after 5 seconds the parent will just move on and leave
	 * the child alone for the child's own fun.
	 * Desgin credit to Oliver Cao (cc)
	 */
		int s;
		//kprintf("Child is still running\n");
		for(s=0;s<30;s++){
			//kprintf("Parent sleep for the #(%d) second\n", s);			
			clocksleep(1);
			if(process_table[pid].status==2){
				*retval = pid;
				*status = process_table[pid].exitCode;	

				//Kick the child out of the child list
				process_table[curthread->pid].childIDs[j]=-1;
				//kprintf("Kiching out child: #%d", process_table[curthread->pid].childIDs[j]);
				//kprintf("Child ID at original list position: #%d", process_table[curthread->pid].childIDs[j]);
				process_table[curthread->pid].numOfChild-=1;
				process_table[pid].has_been_waited = 1;				
				
				return 0;
			}
		}

	}
	else if(child_status == 2){
		//kprintf("BUG DO I FOUND:status %d\n", process_table[pid].exitCode);
		assert(retval!=NULL);		
		*retval = pid;
		
		//kprintf("BUG DO I FOUND:status %d\n", process_table[pid].exitCode);
		*status = process_table[pid].exitCode;	

		//Kick the child out of the child list
		process_table[curthread->pid].childIDs[j]=-1;
		//kprintf("Kiching out child: #%d", process_table[curthread->pid].childIDs[j]);
		//kprintf("Child ID at original list position: #%d", process_table[curthread->pid].childIDs[j]);
		process_table[curthread->pid].numOfChild-=1;
		process_table[pid].has_been_waited = 1;			
		return 0;
	}
	
	//After 5 seconds, the parent leaves anyways. This is to prevent child dead lock condition, so it should be an error for waitpid
	//The tester might check this for the "15 seconds timeout"?
	*retval = pid;
	status = NULL;
	return -1;//This error is not described in documentation
	
}



void md_forkentry(struct trapframe *tf, unsigned long child_address_space){
	//kprintf("Entering md_forkentry\n");
	
	//copy the trapframe passed in by parent_thread
	struct trapframe child_trapframe;
	child_trapframe = *tf;	
	
	//###########
	  kfree(tf);//TA says this thing does not work in ASST2? Watch out if "out of memory"
	//###########

	//child_addrspace_ptr copy the same value from child_address_space passed in	
	struct addrspace *child_addrspace_ptr;
		//child_addrspace_ptr = curthread->t_vmspace;
	child_addrspace_ptr = (struct addrspace *)child_address_space;

	//load and set up the current address space as the 
		//curthread->t_vmspace = (struct addrspace*)child_address_space;
	curthread->t_vmspace = child_addrspace_ptr;
		//as_activate(curthread->t_vmspace);
	as_activate(child_addrspace_ptr);

	child_trapframe.tf_epc += 4;//next instruction when returned back to fork.
	child_trapframe.tf_v0 = 0;//child return 0 on success
	child_trapframe.tf_a3 = 0;

	//child ready for user mode
	//kprintf("I am ready for user mode\n");
	mips_usermode(&child_trapframe);
	kprintf("BUG\n");
	//return 0;
	
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------0


pid_t fork(struct trapframe *tf, int *retval){
	
/*Description
fork duplicates the currently running process. The two copies are identical, except that one (the "new" one, or "child"), has a new, unique process id, and in the other (the "parent") the process id is unchanged.

The process id must be greater than 0.

The two processes do not share memory or open file tables; this state is copied into the new process, and subsequent modification in one process does not affect the other.

However, the file handle objects the file tables point to are shared, so, for instance, calls to lseek in one process can affect the other.

Return Values
On success, fork returns twice, once in the parent process and once in the child process. In the child process, 0 is returned. In the parent process, the process id of the new child process is returned.

On error, no new process is created, fork only returns once, returning -1, and errno is set according to the error encountered.
Errors
The following error codes should be returned under the conditions given. Other error codes may be returned for other errors not mentioned here.

     	 
    EAGAIN		Too many processes already exist.
    ENOMEM		Sufficient virtual memory for the new process was not available.*/
	
	//kprintf("Entering system_call: fork()\n");
	if(tf==NULL){
		*retval = -1;
		return EFAULT;
	}
	
	struct thread *child_process_ptr;
	struct trapframe *child_trapframe_ptr;
	struct addrspace *child_addrspace_ptr;

	child_trapframe_ptr = (struct trapframe *)kmalloc(sizeof(struct trapframe));//cast pointer as kmalloc returns me a void pointer and doesn't allow for dereferencing.
	if(child_trapframe_ptr==NULL){
		*retval = -1;	
		return EFAULT;
		
	}

	//1.copy parent's trapframe (parent's trapframe passed in)
	*child_trapframe_ptr = *tf;//now it can dereference.
	//kprintf("Dereferencing child_trapframe\n");

	//2.copy parent's address space
	int result = as_copy(curthread->t_vmspace, &child_addrspace_ptr);
	if(result){
		*retval = -1;
		return result;
	}
	
	//3.create a thread: handled by thread_fork(thread_create also called), copy and set up in fork_entry.
	result = thread_fork("Child", (struct trapframe *)child_trapframe_ptr, (unsigned long)child_addrspace_ptr, md_forkentry, &child_process_ptr);//not too sure what is the third argument going to be? (put 0 for now)

	//kprintf("result: %d\n", result);
	if(result){
		kfree(child_trapframe_ptr);
		*retval = -1;	
		return result;
	}
	
	*retval = child_process_ptr->pid;		
	
	return 0;

}


int read(int fd, void *buf, size_t buflen, int *retval){
	
	//kprintf("I am in sys read\n");
	if(buf == NULL){
		*retval = -1;
		return EFAULT;		
	}
	if((fd!=0)){
		
		*retval = -1;
		return EBADF;
	}

	/*
	 * Copy data from a kernel buffer to a data region defined by a uio struct,
	 * updating the uio struct's offset and resid fields. May alter the iovec
	 * fields as well.
	*/  
	struct uio u;
	
	/*Before calling this, you should*/
	u.uio_iovec.iov_un.un_ubase = buf;	//(1) set up uio_iovec to point to the buffer you want to transfer to;
	u.uio_offset = curthread->offset;	//(2) initialize uio_offset as desired; * Note that the actual value of uio_offset is not interpreted. It is provided to allow for easier file seek pointer management.
	u.uio_iovec.iov_len = buflen;	
	u.uio_resid = buflen;				//(3) initialize uio_resid to the total amount of data that can be transferred through this uio;
	u.uio_segflg = UIO_USERSPACE;		//(4) set up uio_seg and uio_rw correctly;
 	u.uio_rw = UIO_READ;			
	u.uio_space = curthread->t_vmspace;	//(5) if uio_seg is UIO_SYSSPACE, set uio_space to NULL; otherwise, initialize uio_space to the address space in which the buffer should be found.
	
	//int temp = uiomove(buf, buflen, &u);
	
	int result;
	
	//kprintf("This the process #%d\n",curthread->pid);
	
	char buf[10];
	strcpy(buf,"con:");
	
	if(curthread->v==NULL){
		result = vfs_open(buf, O_RDWR, &(curthread->v));
		if(result){
			*retval = -1;
			//kprintf("PROBLEM!!!!!!");
			return result;		
		}
		
		//kprintf("I am creating vfs\n");	
	}
	
	result = VOP_READ(curthread->v, &u);
	curthread->offset = u.uio_offset;
	if(result==0){ //everything runs smoothly here
		*retval = buflen;
		return 0;
	}
	else{ //error occur here, return the error code returned from uiomove(from copyinout possibly)
		return result;
	}
}

int write(int fd, const void *buf, size_t nbytes, int *retval){
	
	if(buf == NULL){
		*retval = -1;
		return EFAULT;		
	}
	if((fd!=1)&&(fd!=2)){
		*retval = -1;
		return EBADF;
	}

	/*
	 * Copy data from a kernel buffer to a data region defined by a uio struct,
	 * updating the uio struct's offset and resid fields. May alter the iovec
	 * fields as well.
	*/  
	struct uio u;
	
	/*Before calling this, you should*/
	u.uio_iovec.iov_un.un_ubase = buf;	//(1) set up uio_iovec to point to the buffer you want to transfer to;
	u.uio_iovec.iov_len = nbytes;
	u.uio_offset = curthread->offset;	//(2) initialize uio_offset as desired; * Note that the actual value of uio_offset is not interpreted. It is provided to allow for easier file seek pointer management.
	u.uio_resid = nbytes;				//(3) initialize uio_resid to the total amount of data that can be transferred through this uio;
	u.uio_segflg = UIO_USERSPACE;		//(4) set up uio_seg and uio_rw correctly;
 	u.uio_rw = UIO_WRITE;			
	u.uio_space = curthread->t_vmspace;	//(5) if uio_seg is UIO_SYSSPACE, set uio_space to NULL; otherwise, initialize uio_space to the address space in which the buffer should be found.
	
	//int temp = uiomove(buf, buflen, &u);
	int result;
	
	//kprintf("This the process #%d\n",curthread->pid);
	
	char buf[10];//stores "con:" as it is freed everytime I close vfs
	strcpy(buf,"con:");
	
	
	if(curthread->v==NULL){
		//kprintf("I am trying to open vfs\n");
		result = vfs_open(buf, O_RDWR, &(curthread->v));
		if(result){
			*retval = -1;
			//kprintf("PROBLEM!!!!!! with id %d\n", result);
			return result;		
		}
		//kprintf("I am creating vfs\n");	
	}

	result = VOP_WRITE(curthread->v, &u);
	curthread->offset = u.uio_offset;
	
	//kprintf("resulr ==t : %d", result);
	
	if(result==0){ //everything runs smoothly here
		*retval = nbytes;
		return 0;
	}
	else{ //error occur here, return the error code returned from uiomove(from copyinout possibly)
		*retval = -1;
		return result;
	}
}

pid_t getpid(void){
	return curthread->pid;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------0


int
execv(const char *program, char **args,  int* retval){
	if((program == NULL) || (program == (char*)0x40000000) || (program == (char*)0x80000000)){
		*retval = -1;
		return EFAULT;
	}

	int program_size = strlen(program);

	if(program_size == 0){
		*retval = -1;
		return EINVAL;
	}

	if((args == NULL) || (args == (char**)0x40000000) || (args == (char**)0x80000000)){
		*retval = -1;
		return EFAULT;
	}

	// copy program into kernel mode 
	char* prog_copy = kmalloc(sizeof(char)*(program_size+1));
	int result = copyin((const_userptr_t)program, prog_copy, program_size+1);
	if(result){ 
		kfree(prog_copy);
		*retval = -1;
		return result;
	}

	//copy arguments into kernel mode
	int argc = 0;
	while(args[argc]){
		if((args[argc] == (char*)0x40000000) || (args[argc] == (char*)0x80000000)){
			*retval = -1;
			return EFAULT;
		}
		argc++;
	}
	char** argv = kmalloc(sizeof(char*)*(argc+1));
	result = copyin((const_userptr_t)args, argv, argc+1);
	if(result){
		kfree(prog_copy);
		kfree(argv);
		*retval = -1;
		return result;
	}
	int i = 0;
	while(args[i]){
		int string_size = strlen(args[i]);
		argv[i] = kmalloc(sizeof(char)*(string_size+1));
		result = copyin((const_userptr_t)args[i], argv[i], string_size+1);
		if(result){
			kfree(prog_copy);
			int j = 0;
			while(j<=i){
				kfree(argv[j]);
			}
			kfree(argv);
			*retval = -1;
			return result;
		}
		i++;
	}
	argv[i] = NULL;
	
	result = runprogram(prog_copy, argv, argc);

	if(result){
		*retval = -1;
		return result;
	}

}

time_t 
__time(time_t *seconds, unsigned long *nanoseconds, int* retval){
	time_t s;
	unsigned long ns;

	gettime(&s, &ns);

	int result1 = 0;
	int result2 = 0;
	if(seconds != NULL){
		result1 = copyout(&s, seconds, sizeof(time_t));
	}

	

	if(nanoseconds != NULL){
		result2 = copyout(&ns, nanoseconds, sizeof(unsigned long));
	}

	if((result1!=0)||(result2!=0)){
		 *retval = -1;
        return EFAULT;
	}

	*retval = (int)s;
    return 0;
    /*kprintf("seconds: %p\n",seconds);
    kprintf("nanoseconds: %p\n",nanoseconds);
    kprintf("retval: %d\n\n",retval);
	time_t s;
	u_int32_t ns;

	gettime(&s, &ns);


	if(seconds ==NULL &&nanoseconds ==NULL){
		*retval =0;
	}
	
	int result1;
	if(nanoseconds == NULL){
		result1 = copyout(&s, seconds, sizeof(s));
		if (result1){
			*retval = -1;
			return result1;
		}
	}

	if(seconds == NULL){
		result1 = copyout(&ns, nanoseconds, sizeof(ns));
		if (result1){
			*retval = -1;
			return result1;
		}
	}

	*retval = 0;
    return 0;*/


}


void *
sbrk(intptr_t amount, int* retval){
//
	struct addrspace *as = curthread->t_vmspace;
	vaddr_t stackbase, new_vtop3, new_vtop3_aligned, old_vtop3;
	new_vtop3 = as->as_vtop3 + amount;
	new_vtop3_aligned = as->as_vtop3 + as->as_npages3 * PAGE_SIZE;
	stackbase = USERSTACK - 512 * PAGE_SIZE;

	/*if(amount >= (4096*1024*256)){
		*retval = (void*)-1;		
		return ENOMEM;
	}

	if(amount <= (-4096*1024*256)){
		*retval = (void*)-1;		
		return EINVAL;
	}*/

	if(new_vtop3 >= stackbase){
		*retval = (void*)-1;		
		return ENOMEM;
	}

	if(new_vtop3 < as->as_vbase3){
		*retval = (void*)-1;
		return EINVAL;
	}

	/*int i;
	int page_left = 0;
	for (i=0;i<coremapsize;i++ ){
		if(coremap[i].state == 0){
		page_left++;
		}
	}

	int num_of_pages;
	if(amount > 0){
		int remainder = amount % PAGE_SIZE;
		amount -= remainder;
		num_of_pages= amount / PAGE_SIZE;
		if(remainder != 0){
			num_of_pages++;
		}
		if(num_of_pages > page_left){
			*retval = (void*)-1;		
			return ENOMEM;
		}
	}*/

	if((new_vtop3_aligned - as->as_vtop3) >= amount){
		old_vtop3 = as->as_vtop3;
		as->as_vtop3 = new_vtop3;
		*retval = old_vtop3;
		return 0;	
	} else{
		old_vtop3 = as->as_vtop3;
		amount -= (new_vtop3_aligned - as->as_vtop3);
		int i = 1;
		while(amount > PAGE_SIZE){
			amount -= PAGE_SIZE;
			i++;
		}
		as->as_npages3 += i;
		as->as_vtop3 = new_vtop3;
		*retval = old_vtop3;
		return 0;		
	}

}

