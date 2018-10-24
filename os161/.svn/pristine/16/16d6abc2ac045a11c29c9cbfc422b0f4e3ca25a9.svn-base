#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <addrspace.h>
#include <vm.h>
#include <machine/spl.h>
#include <machine/tlb.h>
#include <uio.h>
#include <vnode.h>


/*
 * Dumb MIPS-only "VM system" that is intended to only be just barely
 * enough to struggle off the ground. You should replace all of this
 * code while doing the VM assignment. In fact, starting in that
 * assignment, this file is not included in your kernel!
 */
extern int numthreads;
//struct page *coremap;
#define DUMBVM_STACKPAGES    12
int counter = 0;
void
vm_bootstrap(void)
{
	/*initialize coremap*/
	boott=0;
	vaddr_t va;

	int i=0;
	coremap_ram_getsize(&begin,&end);
	
	coremap = kmalloc(PAGE_SIZE);
	va = alloc_kpages(1);	
	while(va !=0){
		coremap[i].pa = KVADDR_TO_PADDR(va);
		coremap[i].state = 0;
		coremap[i].follower = 0;
		coremap[i].york='n';
		coremap[i].thread_as =NULL;
		i++;
		va = alloc_kpages(1);

	}
	coremapsize = i;
	
	coremap_ram_getsize(&begin,&end);
	
	boott=1;
		
	vaddr_t v = kmalloc(PAGE_SIZE);

}

int calculate_page(size_t sz){
	return (sz+PAGE_SIZE-1)/PAGE_SIZE;
}

void printbitmap(){ //helper function for debugging
	int i;	
	kprintf("\n--->Core Map<--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	for(i=0;i<coremapsize;i++){
		if(i<10)
			kprintf("0");
		kprintf("%d :: %d :: 0x%x   ",i,coremap[i].state,coremap[i].pa);
		if((i+1)%10 == 0){
			kprintf("\n");
		}
	}
	kprintf("\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

static
paddr_t
getppages(unsigned long npages)
{
	int spl;
	paddr_t addr;

	spl = splhigh();

	addr = ram_stealmem(npages);
	
	splx(spl);
	return addr;//pa
}



/* Allocate/free some kernel-space virtual pages */
vaddr_t 
alloc_kpages(int npages)
{	

	//kprintf("\n!!!!%d!!!!!\n",npages);
	if(boott ==0){
		paddr_t pa;
		pa = getppages(npages);
		if (pa==0) {
			return 0;
		}
		return PADDR_TO_KVADDR(pa);
	}else if(boott == 1){
		//stick into coremap
		if(npages ==1){
			int i;
			// critical section
			int spl;
			spl = splhigh();
			char* temp;
			for(i=0;i<coremapsize;i++){
				if(coremap[i].state == 0 ){ //found empty slot
					coremap[i].state =1;
					coremap[i].york ='k';
					coremap[i].thread_as =curthread->t_vmspace;
					paddr_t addrs = coremap[i].pa;
					int k;
					temp = PADDR_TO_KVADDR(addrs);                    
					for(k=0;k<4096;k++){						
						temp[k] = 0;					
					}
					splx(spl);

					return PADDR_TO_KVADDR(addrs);
				}
			}	
			splx(spl);
			//no empty
			return 0;

		}else if(npages>1){
			int i;
			int spl;
			spl = splhigh();			
			//look for empty slot in coremap. if found, check the next  N-1 slots, if N slots are available, return pa of 1 slot.
			// else, return 0
			for(i=0;i<coremapsize;i++){ 
				if(coremap[i].state ==0){ //found empty slot, need consecutive 0's
					int not_enough_space =0;
					int j;
					for(j=1;j<npages;j++){ //check if there are enough space
						if(coremap[i+j].state == 1){//occupied
							not_enough_space =1;
						}		
					}
					if(not_enough_space ==0){ //enough space for npages
						int k;		
						for(k=0;k<npages;k++){// mark i, i+1,i+2,..... to occupied
							coremap[i+k].state = 1;
							coremap[i+k].york ='k';
							coremap[i].thread_as =curthread->t_vmspace;
							int m;
							char* temp;
							temp = PADDR_TO_KVADDR(coremap[i+k].pa);                    
							for(m=0;m<4096;m++){						
								temp[m] = 0;					
							}
						}
						coremap[i].follower = npages-1;
						paddr_t addrs = coremap[i].pa;
						splx(spl);
						return PADDR_TO_KVADDR(addrs);
					}
				}			
			}
			splx(spl);
			//end of critical section
			return 0; //iterated entire coremap, not enough space.
			
		}
	}
}

void 
free_kpages(vaddr_t addr)
{
	/* if free single page, change coremap state, then return, */
	/* if free multiple page, change states, then return, */
	/* page not found? */
	int spl;
	spl = splhigh();
	
	paddr_t pa = KVADDR_TO_PADDR(addr);
	int i ;
	for(i=0;i<coremapsize;i++){
		if(coremap[i].pa == pa && coremap[i].follower ==0){ //no follower, single page deallocation
			coremap[i].state = 0;//mark state, space freed
			coremap[i].york ='n';
			coremap[i].thread_as =NULL;
			break;
		}else if (coremap[i].pa ==pa && coremap[i].follower !=0){ //yes follower, multi page deallocation
			int j;	
			char* temp;		
			for(j=0;j<=coremap[i].follower;j++){
				coremap[i+j].state = 0;
				coremap[i].york ='n';
				coremap[i].thread_as =NULL;
				int k;
				temp = PADDR_TO_KVADDR(coremap[i].pa);                    
				for(k=0;k<4096;k++){
					temp[k] = 0;						
				}
			}
			coremap[i].follower = 0;
			break;
		}

	}

	splx(spl);
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{	

	counter++;
	vaddr_t vbase1, vtop1, vbase2, vtop2, stackbase, stacktop, vbase3, vtop3, faultaddress_dummy;
	paddr_t paddr;
	int i;

	u_int32_t ehi, elo;
	struct addrspace *as;
	int spl, region;


	spl = splhigh();

	faultaddress_dummy = faultaddress;
	faultaddress &= PAGE_FRAME;

	switch (faulttype) {
	    case VM_FAULT_READONLY:
			splx(spl);
			return EFAULT;
	    case VM_FAULT_READ:
			break;
	    case VM_FAULT_WRITE:
			break;
	    default:
		splx(spl);
		return EINVAL;
	}

	as = curthread->t_vmspace;
	if (as == NULL) {
		/*
		 * No address space set up. This is probably a kernel
		 * fault early in boot. Return EFAULT so as to panic
		 * instead of getting into an infinite faulting loop.
		 */
		splx(spl);
		return EFAULT;
	}

	vbase1 = as->as_vbase1;
	vtop1 = vbase1 + as->as_npages1 * PAGE_SIZE;
	vbase2 = as->as_vbase2;
	vtop2 = vbase2 + as->as_npages2 * PAGE_SIZE;
	stackbase = USERSTACK - 512 * PAGE_SIZE;//-------------------------------------------------------------------------------------?
	stacktop = USERSTACK;
	vbase3 = as->as_vbase3;
	vtop3 = as->as_vtop3;

	int indicator = 0;
	int out_of_memory = 0;

	if (faultaddress >= vbase1 && faultaddress < vtop1) {
		paddr = fault_handler(faultaddress, &indicator, &out_of_memory);
		region = 1;
		if(out_of_memory == 1){
			return ENOMEM;
		}
		if(indicator == 1){
			/*Load Segment*/
	
			off_t offset = 0;
			size_t filesize;
			int is_executable = 0;
			vaddr_t vbase;

			switch (region) {
				case 1:
					vbase = vbase1;	
					is_executable = as->is_executable1;
					offset = as->as_offset1 + faultaddress - (vbase & PAGE_FRAME);		
					filesize = as->as_filesize1;	
				break;
				case 2:
					vbase = vbase2;	
					is_executable = as->is_executable2;
					offset = as->as_offset2 + faultaddress - (vbase & PAGE_FRAME);		
					filesize = as->as_filesize2;		
				break;
				default:
					kprintf("BOOM\n");
					splx(spl);
					return 1;
			}
	
			int remainder = filesize % PAGE_SIZE;
			int num_of_pages = filesize / PAGE_SIZE;	
			if(remainder){
				num_of_pages++;
			}
	
			int num_of_pages_fault = ((faultaddress - vbase) / PAGE_SIZE) + 1;
	
			if(num_of_pages_fault < num_of_pages){
				filesize = PAGE_SIZE;
			} else if(num_of_pages_fault == num_of_pages) {
				filesize = remainder;
			}

			load_page(as->as_v, offset, faultaddress, PAGE_SIZE, filesize, is_executable);

			splx(spl);
			return 0;
		}
	}
	else if (faultaddress >= vbase2 && faultaddress < vtop2) {
		paddr = fault_handler(faultaddress, &indicator, &out_of_memory);
		region = 2;
		if(out_of_memory == 1){
			return ENOMEM;
		}
		if(indicator == 1){
			/*Load Segment*/
	
			off_t offset = 0;
			size_t filesize;
			int is_executable = 0;
			vaddr_t vbase;

			switch (region) {
				case 1:
					vbase = vbase1;	
					is_executable = as->is_executable1;
					offset = as->as_offset1 + faultaddress - (vbase & PAGE_FRAME);		
					filesize = as->as_filesize1;	
				break;
				case 2:
					vbase = vbase2;	
					is_executable = as->is_executable2;
					offset = as->as_offset2 + faultaddress - (vbase & PAGE_FRAME);		
					filesize = as->as_filesize2;		
				break;
				default:
					kprintf("BOOM\n");
					splx(spl);
					return 1;
			}
	
			int remainder = filesize % PAGE_SIZE;
			//kprintf("remainder = %d\n", remainder);
			int num_of_pages = (filesize - remainder) / PAGE_SIZE;	
			if(remainder){
				num_of_pages++;
			}
	
			int num_of_pages_fault = (faultaddress / PAGE_SIZE) + 1;
	
			if(num_of_pages_fault < num_of_pages){
				filesize = PAGE_SIZE;
			} else if(num_of_pages_fault == num_of_pages) {
				filesize = remainder;
			}

			load_page(as->as_v, offset, faultaddress, PAGE_SIZE, filesize, is_executable);

			splx(spl);
			return 0;
		}
	}
	else if (faultaddress > stackbase && faultaddress < stacktop) {
		paddr = fault_handler(faultaddress, &indicator, &out_of_memory);
		if(out_of_memory == 1){
			return ENOMEM;
		}
		region = 4;
	}
	else if (faultaddress >= vbase3 && faultaddress < vtop3) {
		paddr = fault_handler(faultaddress, &indicator, &out_of_memory);
		if(out_of_memory == 1){
			return ENOMEM;
		}
		region = 3;
	} else {		
		//splx(spl);
		//return 0;		
		/*kprintf("faultaddress: 0x%x\n", faultaddress);
		kprintf("vbase1: 0x%x\n", vbase1);
		kprintf("vtop1: 0x%x\n", vtop1);
		kprintf("vbase2: 0x%x\n", vbase2);
		kprintf("vtop2: 0x%x\n", vtop2);
		kprintf("stacktop: 0x%x\n", stacktop);
		kprintf("vbase3: 0x%x\n", vbase3);
		kprintf("vtop3: 0x%x\n", vtop3);*/
		
		//kprintf("%d, HERE!\n", counter);
		splx(spl);
		return EFAULT;
	}
	/* make sure it's page-aligned */
	assert((paddr & PAGE_FRAME)==paddr);

	for (i=0; i<NUM_TLB; i++) {
		TLB_Read(&ehi, &elo, i);
		if (elo & TLBLO_VALID) {
			continue;
		}
		ehi = faultaddress;
		elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
		DEBUG(DB_VM, "dumbvm: 0x%x -> 0x%x\n", faultaddress, paddr);
		TLB_Write(ehi, elo, i);
		break;
	}
	
	if(i == NUM_TLB){
		ehi = faultaddress;
		elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
		TLB_Random(ehi, elo);
	}
	
	splx(spl);
	return 0;
}

paddr_t 
fault_handler(vaddr_t faultaddress, int *indicator, int *out_of_memory){
	vaddr_t pageTableOneIndex = (faultaddress & 0xffc00000)>>22;
	vaddr_t pageTableTwoIndex = (faultaddress & 0x003ff000)>>12; 
	struct addrspace *as;
	as = curthread->t_vmspace;
	
	if(as->PT1[pageTableOneIndex]==NULL)//Table 2 not exist
	{
		*indicator = 1;
		vaddr_t *PT2 = kmalloc(sizeof(paddr_t)*1024);
		int i;
		for(i=0;i<1024;i++){
			PT2[i] = NULL;
		}
		PT2[pageTableTwoIndex] = pmalloc(PAGE_SIZE);
		as->PT1[pageTableOneIndex] = PT2;
	}
	else{
		if(as->PT1[pageTableOneIndex][pageTableTwoIndex]==NULL){//Table 2 exist, but mapping not found
			
			*indicator = 1;
			as->PT1[pageTableOneIndex][pageTableTwoIndex] = pmalloc(PAGE_SIZE);
		}
		else{//mapping found in the PT2
		
			return KVADDR_TO_PADDR(as->PT1[pageTableOneIndex][pageTableTwoIndex]);
		}
	}
	
	if(as->PT1[pageTableOneIndex][pageTableTwoIndex] == 0){
		*out_of_memory = 1;
	}	

	return KVADDR_TO_PADDR(as->PT1[pageTableOneIndex][pageTableTwoIndex]);
}

vaddr_t 
pmalloc(int npages)
{	
	int i;
	// critical section
	int spl;
	spl = splhigh();

	for(i=0;i<coremapsize;i++){
		if(coremap[i].state == 0 ){ //found empty slot
			coremap[i].state =1;
			coremap[i].york ='u';
			paddr_t addrs = coremap[i].pa;
			coremap[i].thread_as =curthread->t_vmspace;
			int k;
			char* temp;
			temp = PADDR_TO_KVADDR(coremap[i].pa);                    
			for(k=0;k<4096;k++){						
				temp[k] = 0;					
			}
			splx(spl);

			return PADDR_TO_KVADDR(addrs);
		}
		
	}	
	splx(spl);

	//no empty
	return 0;
}

static int
load_page(struct vnode *v, off_t offset, vaddr_t vaddr, 
	     size_t memsize, size_t filesize,
	     int is_executable)
{
	struct uio u;
	int result;
	size_t fillamt;

	if (filesize > memsize) {
		kprintf("ELF: warning: segment filesize > segment memsize\n");
		kprintf("filesize: %d    memsize: %d\n\n", filesize, memsize);
		filesize = memsize;
	}

	DEBUG(DB_EXEC, "ELF: Loading %lu bytes to 0x%lx\n", 
	      (unsigned long) filesize, (unsigned long) vaddr);

	u.uio_iovec.iov_ubase = (userptr_t)vaddr;
	u.uio_iovec.iov_len = memsize;   // length of the memory space
	u.uio_resid = filesize;          // amount to actually read
	u.uio_offset = offset;
	u.uio_segflg = is_executable ? UIO_USERISPACE : UIO_USERSPACE;
	u.uio_rw = UIO_READ;
	u.uio_space = curthread->t_vmspace;

	result = VOP_READ(v, &u);
	if (result) {
		return result;
	}

	if (u.uio_resid != 0) {
		// short read; problem with executable? 
		kprintf("ELF: short read on segment - file truncated?\n");
		return ENOEXEC;
	}

	/* Fill the rest of the memory space (if any) with zeros */
	fillamt = memsize - filesize;
	if (fillamt > 0) {
		DEBUG(DB_EXEC, "ELF: Zero-filling %lu more bytes\n", 
		      (unsigned long) fillamt);
		u.uio_resid += fillamt;
		result = uiomovezeros(fillamt, &u);
	}
	
	return result;
}
