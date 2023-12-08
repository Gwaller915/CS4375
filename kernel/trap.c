#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include <stddef.h>

struct spinlock tickslock;
uint ticks;

extern char trampoline[], uservec[], userret[];

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}


void
usertrap(void)
{
  int which_dev = 0;
 
  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");
 
  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec); 
 
  struct proc *p = myproc();
 
  // save user program counter.
 
  p->trapframe->epc = r_sepc();
  if(r_scause() == 8){
    // system call
 
    if(p->killed)
      exit(-1);
 
    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;
 
    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    intr_on();
 
    syscall();
  } else if((which_dev = devintr()) != 0){
    // ok
 
  } else if(r_scause() == 13 || r_scause() == 15){
    uint64 faultingAddress = PGROUNDDOWN(r_stval());
    
 
    if ( faultingAddress >= p->sz) {
     for(int i = 0; i < MAX_MMR; i++){ 
       
       // Check if the MMR is valid and within range
        printf("___starting page region %p\n", p->mmr[0].addr);
       
      if(p->mmr[i].valid && 
         faultingAddress >= p->mmr[i].addr && 
         faultingAddress < p->mmr[i].addr + p->mmr[i].length){
	
	int valid_permission = 0;
  	if(r_scause()==13 && (p->mmr[i].prot & PTE_R)){
		valid_permission = 1;}
	else if(r_scause() == 15 && (p->mmr[i].prot & PTE_W)){
		valid_permission = 1;}
	if(!valid_permission){
          p->killed = 1;
          exit(-1);
       }
      }
     }
    }
    
    //Allocates memory frame
    char *mem = kalloc(); 
    if(mem == NULL){
       printf("Kalloc failed\n");
       p->killed = 1;
       exit(-1);
    }
    
    // Frame is added to page table after rounding
    if (mappages(p->pagetable, faultingAddress, PGSIZE, (uint64)mem, PTE_W | PTE_X | PTE_R | PTE_U) < 0) {
      printf("___Page mapped.\n");
      printf("___p size after: %p\n", p->sz);
      printf("___Faulting address after rounding: %p\n", faultingAddress);
      kfree(mem);
      p->killed = 1;
      exit(-1);
    }
     
     // New mapping for page in page table
      for(int i = 0; i < MAX_MMR; i++) {
        if(p->mmr[i].valid && faultingAddress >= p->mmr[i].addr && faultingAddress < p->mmr[i].addr + p->mmr[i].length) {
	  struct mmr_list *mmr_list = get_mmr_list(p->mmr->mmr_family.listid);
	  acquire(&mmr_list[p->mmr[i].mmr_family.listid].lock);
	  struct mmr_node *nf = p->mmr[i].mmr_family.next;
          while (nf != &(p->mmr[i].mmr_family) ) {
            struct proc *family_proc = nf->proc;
            if (family_proc != p) {
              
              if (mappages(family_proc->pagetable, PGROUNDDOWN(faultingAddress), PGSIZE, (uint64)mem, PTE_W | PTE_X | PTE_R | PTE_U) < 0) {
                printf("__Page mapped for new mapping.\n");
                printf("__p size after: %p\n", p->sz);
                printf("__Faulting address after rounding: %p\n", faultingAddress);
               
                kfree(mem);
                p->killed = 1;
                exit(-1);
              }
            }
            nf = nf->next;
          }
	  release(&mmr_list[p->mmr[i].mmr_family.listid].lock);
        }
      }
  }
  
      //Test print statements
      //printf("User trap error.\n");
      //printf("p size: %p\n", p->sz);
      //printf("In trap.c Faulting address: %p\n", faulting_address);
      //Psuedo code from class.  Call to kalloc if faulting address is less than sz
  
  
    else {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }
 
  if(p->killed)
    exit(-1);
 
  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();
 
  usertrapret();
}


//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//

/* VERSION 1
void usertrap(void) {
  int which_dev = 0;

  if ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // Send interrupts and exceptions to kerneltrap()
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();

  // Save user program counter.
  p->trapframe->epc = r_sepc();

  if (r_scause() == 8) {
    // System call
    if (p->killed)
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // An interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    intr_on();

    syscall();
  } else if ((which_dev = devintr()) != 0) {
    // Ok
  } else if (r_scause() == 13 || r_scause() == 15) { // Check for load or store fault
    uint64 faulting_address = PGROUNDDOWN(r_stval());

    if (faulting_address >= p->sz) {
      ///p->killed = 1;
    
      for (int i = 0; i < MAX_MMR; i++) {
        // Check if the MMR is valid and within range
        if (p->mmr[i].valid == 1 &&
            faulting_address >= p->mmr[i].addr &&
            faulting_address < p->mmr[i].addr + p->mmr[i].length) {

          int valid_permission = 0;
          if (r_scause() == 13 && (p->mmr[i].prot & PTE_R)) {
            valid_permission = 1;
          } else if (r_scause() == 15 && (p->mmr[i].prot & PTE_W)) {
            valid_permission = 1;
          }

          if (!valid_permission) {
            p->killed = 1;
            exit(-1);
          }
        }

        break; // Exiting the loop since we've found a match
      }
    
    }

    if (faulting_address < p->sz) {
      char *memory = kalloc();
      int pg_round_down = PGROUNDDOWN(faulting_address);
      memset(memory, 0, PGSIZE);

      if (memory == NULL) {
        printf("Kalloc failed\n");
        p->killed = 1;
      } else {
        if (mappages(p->pagetable, pg_round_down, PGSIZE, (uint64)memory, PTE_W | PTE_X | PTE_R | PTE_U) != 0) {
          printf("Page not mapped.\n");
          kfree(memory);
          p->killed = 1;
        } else {
          printf("Page mapped.\n");
          printf("p size after: %p\n", p->sz);
          printf("Faulting address after rounding: %p\n", pg_round_down);
        }
      }
    }
  }

  // Increment cputime if interrupt happens
  if (p) {
    if (r_scause() == 9) {
      p->cputime++;
    }
  }

  if (p->killed)
    exit(-1);

  // Give up the CPU if this is a timer interrupt.
  if (which_dev == 2)
    yield();

  usertrapret();
}
*/



void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to trampoline.S
  w_stvec(TRAMPOLINE + (uservec - trampoline));

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 fn = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64,uint64))fn)(TRAPFRAME, satp);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    printf("scause %p\n", scause);
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING)
    yield();

  //user implemented cputime increment increase when interupt happens
  //pointer to myproc
    struct proc *curr_proc = myproc();
    if (curr_proc){
      curr_proc->cputime++;
    }


  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
  acquire(&tickslock);
  ticks++;
  wakeup(&ticks);
  release(&tickslock);
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000001L){
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    if(cpuid() == 0){
      clockintr();
    }
    
    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2);

    return 2;
  } else {
    return 0;
  }
}

