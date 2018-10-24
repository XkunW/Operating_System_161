#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <vm.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>
#include <machine/tlb.h>
#include <array.h>
#include <vnode.h>

/*
 * Note! If OPT_DUMBVM is set, as is the case until you start the VM
 * assignment, this file is not compiled or linked or in any way
 * used. The cheesy hack versions in dumbvm.c are used instead.
 */
#define DUMBVM_STACKPAGES    12

/*
paddr_t
getppages(unsigned long npages)
{

	size_t sz = (npages*PAGE_SIZE) - PAGE_SIZE + 1;
	
	return (vaddr_t)KVADDR_TO_PADDR(kmalloc(sz));//kmalloc returns va, the original test case need pa

}*/
struct addrspace *
as_create(void)
{
	struct addrspace *as = kmalloc(sizeof(struct addrspace));
	if (as==NULL) {
		return NULL;
	}
//------------------------------------------------------------------------------------------------------------------------------------0
	int i;
	for(i=0;i<1024;i++){
		as->PT1[i]=NULL;
	}

	
	as->as_vbase1 = 0;
	as->as_pbase1 = 0;
	as->as_npages1 = 0;

	as->as_vbase2 = 0;
	as->as_pbase2 = 0;
	as->as_npages2 = 0;

	as->as_vbase3 = 0;
	as->as_vtop3 = 0;
	as->as_pbase3 = 0;
	as->as_npages3 = 0;
	
	as->as_stackvbase = 0;
	as->as_stackpbase = 0;

	as->as_v = NULL;

	return as;
}

void
as_destroy(struct addrspace *as)
{
	int i;
	for (i=0;i<coremapsize;i++ ){

		if((coremap[i].york =='u') && (coremap[i].thread_as == as)){
			//kprintf("u");
			free_kpages(PADDR_TO_KVADDR(coremap[i].pa));
		}

	}

	for (i=0;i<coremapsize;i++ ){

		if((coremap[i].york =='k') && (coremap[i].thread_as == as)){
			//kprintf("k");
			kfree(PADDR_TO_KVADDR(coremap[i].pa));
		}

	}
	
	int z = 0;
	for(z=0;z<1024;z++){
		as->PT1[z] = NULL;
	}

	kfree(as);

}

void
as_activate(struct addrspace *as)
{
	int i, spl;

	(void)as;

	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		TLB_Write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t sz, off_t offset, size_t filesize, struct vnode *v,
		 int readable, int writeable, int executable)
{
	size_t npages; 

	/* Align the region. First, the base... */
	sz += vaddr & ~(vaddr_t)PAGE_FRAME;
	vaddr &= PAGE_FRAME;

	/* ...and now the length. */
	sz = (sz + PAGE_SIZE - 1) & PAGE_FRAME;

	npages = sz / PAGE_SIZE;

	/* We don't use these - all pages are read-write */
	(void)readable;
	(void)writeable;
	//(void)executable;

	//--------------------------------------------------------------------------------------0

	if (as->as_v == NULL){
		as->as_v = v;
	}	

	if (as->as_vbase1 == 0) {
		as->as_vbase1 = vaddr;
		as->as_npages1 = npages;
		as->as_offset1 = offset;	
		as->is_executable1 = executable;
		as->as_filesize1 = filesize;
		return 0;
	}

	if (as->as_vbase2 == 0) {
		as->as_vbase2 = vaddr;
		as->as_npages2 = npages;
		as->as_offset2 = offset;
		as->is_executable2 = executable;
		as->as_filesize2 = filesize;
		return 0;
	}
	
	/*
	 * Support for more than two regions is not available.
	 */
	kprintf("dumbvm: Warning: too many regions\n");
	return EUNIMP;
}

int
as_prepare_load(struct addrspace *as)
{
	/*assert(as->as_pbase1 == 0);
	assert(as->as_pbase2 == 0);
	assert(as->as_stackpbase == 0);

	as->as_pbase1 = getppages(as->as_npages1);
	if (as->as_pbase1 == 0) {
		return ENOMEM;
	}

	as->as_pbase2 = getppages(as->as_npages2);
	if (as->as_pbase2 == 0) {
		return ENOMEM;
	}

	as->as_stackpbase = getppages(DUMBVM_STACKPAGES);
	if (as->as_stackpbase == 0) {
		return ENOMEM;
	}*/

	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	as->as_vbase3 = as->as_vbase2 + as->as_npages2 * PAGE_SIZE;
	as->as_vtop3 = as->as_vbase3;

	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	//assert(as->as_stackpbase != 0);
	
	*stackptr = USERSTACK;
	return 0;
}

int
as_copy(struct addrspace *old, struct addrspace **ret)
{
	struct addrspace *new = as_create();

	if (new==NULL) {
		return ENOMEM;
	}

	new->as_vbase1 = old->as_vbase1;
	new->as_npages1 = old->as_npages1;
	new->as_offset1 = old->as_offset1;
	new->is_executable1 = old->is_executable1;
	new->as_vbase2 = old->as_vbase2;
	new->as_npages2 = old->as_npages2;
	new->as_offset2 = old->as_offset2;
	new->is_executable2 = old->is_executable2;
	new->as_vbase3 = old->as_vbase3;
	new->as_npages3 = old->as_npages3;
	new->as_v = old->as_v;


	int i;

	for(i=0;i<1024;i++){

		if(old->PT1[i]!=NULL){
			vaddr_t *PT2 = kmalloc(sizeof(paddr_t)*1024);
			
			int k;
			for(k=0;k<1024;k++){
				PT2[k] = NULL;
			}

			new->PT1[i] = PT2;

			int j=0;
			for(j=0;j<1024;j++){
				if(old->PT1[i][j] != NULL){
					new->PT1[i][j] = pmalloc(PAGE_SIZE);
					memcpy((void *)(new->PT1[i][j]),
						(const void *)(old->PT1[i][j]),
						PAGE_SIZE);
				}
			}			
		}
	}

	if (as_prepare_load(new)) {
		as_destroy(new);
		return ENOMEM;
	}

	VOP_INCREF(old->as_v);
	VOP_INCOPEN(old->as_v);

	
	*ret = new;

	return 0;
}
