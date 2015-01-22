/*
 *  Collin's Binary Instrumentation Tool/Framework for Android
 *  Collin Mulliner <collin[at]mulliner.org>
 *  http://www.mulliner.org/android/
 *
 *  (c) 2012
 *
 *  License: GPL v2
 *
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>
#include <termios.h>

#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#include "libt.h"

#include "uthash.h"

// contains macro of my_init()
// autogenerated
#include "hijacks.h"


#define DEBUG 0

struct mm {
	char name[256];
	unsigned long start, end;
};


static int log_baseaddress = 0;

// communication socket, a pty :)
int coms;

void __attribute__ ((constructor)) my_init(void);

void my_cacheflush(unsigned int begin, unsigned int end)
{	
	const int syscall = 0xf0002;
	__asm __volatile (
		"mov	 r0, %0\n"			
		"mov	 r1, %1\n"
		"mov	 r7, %2\n"
		//"mov     r2, #0x0\n"
		"mov     r2, #0x0\n"
		"svc     0x00000000\n"
		:
		:	"r" (begin), "r" (end), "r" (syscall)
		:	"r0", "r1", "r7"
		);
}


/*
  
  hook_t holds hook metadata 
  hookf is the hook callback
  
 */
int hook(struct hook_t *hook_hash, int pid, char *libname, char *funcname, void *hookf, int by_name, unsigned int raw_address)
{
	unsigned long int addr = 0;
	int i;
	int find_name_result;
	
	struct hook_t *hook = (struct hook_t*) malloc(sizeof(struct hook_t));

	
	if( DEBUG)
	  log("hooking - hash table 0x%x\n", hook_hash1)

	if( by_name ) {

	  // 1 - fetch function address and put it into addr
	  find_name_result = find_name(pid, funcname, libname, &addr);
	  if ( find_name_result < 0) {
	    if( DEBUG)
	      log("can't find: %s - error code %d\n", funcname, find_name_result)
	    return 0;
	  }

	} else {

	  if( DEBUG)
	    log("specified raw address: %x\n", raw_address)
  
         funcname = "raw address";



	  /* // find lib where raw address is contained in */

	  struct mm mm[450];
	  struct mm *m;
	  int nmm;
	  int load_memmap_return, mprotect_return;
	  char *slash;


	  load_memmap_return = load_memmap(pid, mm, &nmm);
	  
	  if( load_memmap_return < 0 ) {
	    log("failed memmap\n")
	      exit(1);
	    
	  }

	  if( DEBUG)
	    log("[*] searching vaddr for function %s - target relative address 0x%x\n", libname, raw_address)

	  for( i = 0, m = mm; i < nmm; i++, m++){

	    // awful code, just to print dbg infos

	    slash = strstr(m->name, ".so");
	    if (!slash)
	      continue;
	    
	    //log("[*] %s start: 0x%8x end: 0x%08x\n", m->name, m->start, m->end);
	    
	    if( strstr(m->name, libname) ) {
	      addr = m->start + raw_address;
	      
	      if( !log_baseaddress) {
		log("[*] Base address 0x%x - 0x%x\n", m->start, m->end)
	        log_baseaddress = 1;
		base_address = m->start;
	      }

	      if( DEBUG)
		log("[*]\tFound address: 0x%8x\n", addr)

		mprotect_return = mprotect((void*)m->start, m->end - m->start, PROT_READ|PROT_WRITE|PROT_EXEC);

	      if( mprotect_return == 0 ) {
	
		if( DEBUG)
	    	  log("[*]\t+rwx  memory region ok\n")
	      }
	      else {
		log("[*]\t mprotect failed\n")
		exit(1);
	      }
	    	
	      break;
	    }
	    
	  }
	  
	}
	
	if( addr == 0) {
	  log("[*] Could not find specified library %s\n", libname)
	    exit(1);
	    }
		       
	if( DEBUG)
	  log("[*] Hooking %s = %x  hook = %x  target:", funcname, addr, hookf)
	
        strncpy(hook->name, funcname, sizeof(hook->name)-1);
		  
	// 2 - hook either ARM or THUMB code
	if (addr % 4 == 0) {
	
	  if( DEBUG)
	    log("ARM\n")

		hook->thumb = 0;
		hook->patch = (unsigned int)hookf;
		hook->orig = addr;

		hook->jump[0] = 0xe59ff000; // LDR pc, [pc, #0]
		hook->jump[1] = hook->patch;
		hook->jump[2] = hook->patch;


		for (i = 0; i < 3; i++)
			hook->store[i] = ( (int*)hook->orig )[i];

		for (i = 0; i < 3; i++)
			((int*)hook->orig)[i] = hook->jump[i];
	}
	else {
		hook->thumb = 1;
		
		if( DEBUG)
		  log("THUMB\n")
		  
		  //h->jumpt[13] = 0xdf;
		  //h->jumpt[12] = 0xfe; // bp


		hook->patch = (unsigned int) hookf;
		hook->orig = addr;	

		/* patch without sub address in r12  */

		/* h->jumpt[1] = 0x46; */
		/* h->jumpt[0] = 0xac; */
		
		/* h->jumpt[3] = 0xa5; */
		/* h->jumpt[2] = 0x02; */
		  
		/* h->jumpt[5] = 0x68; */
		/* h->jumpt[4] = 0x2d; */

		/* h->jumpt[7] = 0xb4; */
		/* h->jumpt[6] = 0x20; */
		  
		/* h->jumpt[9] = 0x46; */
		/* h->jumpt[8] = 0x65; */

		/* h->jumpt[11] = 0xbd; */
		/* h->jumpt[10] = 0x00; */


		/* patch with sub address + 0xd  in r12 */
		hook->jumpt[1] = 0x46;
		hook->jumpt[0] = 0xac;
		
		hook->jumpt[3] = 0xa5;
		hook->jumpt[2] = 0x03;
		  
		hook->jumpt[5] = 0x68;
		hook->jumpt[4] = 0x2d;

		hook->jumpt[7] = 0xb4;
		hook->jumpt[6] = 0x20;
		  
		hook->jumpt[9] = 0x46;
		hook->jumpt[8] = 0x65;

		
		hook->jumpt[11] = 0x46;
		hook->jumpt[10] = 0xfc;

		hook->jumpt[13] = 0x46; // nop
		hook->jumpt[12] = 0xe4;

		hook->jumpt[15] = 0xbd;
		hook->jumpt[14] = 0x00;


		 
		memcpy(&hook->jumpt[16], (unsigned char*)&hook->patch, sizeof(unsigned int));


		unsigned int orig = addr - 1; 


		//char* cmd_disas_format = "arm-none-eabi-objdump -D --target binary -Mforce-thumb -marm %s";

		//char cmd_disas[256];
		unsigned char dump_buffer[20];

		//remove("tmp_stolen.bin");
		//FILE *stolen_bytes_dump = fopen("tmp_stolen.bin", "w");

		// Save stolen bytes
		
		if( DEBUG)
		  log("[*] Saving stolen bytes\n\t")

		for (i = 0; i < 20; i++) {
		  //log("%d:", i)
			hook->storet[i] = ((unsigned char*)orig)[i];
			dump_buffer[i] = ((unsigned char*)orig)[i];
			
			if( DEBUG)
			  log("%0.2x ", hook->storet[i])
			
		}

		if( DEBUG)
		  log("\n")
		  
	
		  //fprintf(stolen_bytes_dump, dump_buffer);
		  //fflush(stolen_bytes_dump);
		  //fclose(stolen_bytes_dump);
		
		  //remove("tmp_patch.bin");
		  //FILE *patch_bytes_dump = fopen("tmp_patch.bin", "w");


		// write the patch to mem
		if( DEBUG)
		  log("[*] Patching target address\n\t")
		    
		for (i = 0; i < 20; i++) {
		 
		    ( (unsigned char*) orig)[i] = hook->jumpt[i];
		    dump_buffer[i] = hook->jumpt[i];

		    if( DEBUG)
		      log("%0.2x ", ((unsigned char*)orig)[i])

		}
		  //fprintf(patch_bytes_dump, dump_buffer);
		  //fflush(patch_bytes_dump);
		  //fclose(patch_bytes_dump);

		if( DEBUG)
		  log("\n")



		/* char out_buffer[200]; */
		/* snprintf(cmd_disas, 100, cmd_disas_format, "tmp_stolen.bin"); */
		/* log("[*] Launching command %s\n", cmd_disas) */
		/* FILE *objdump = popen(cmd_disas, "r"); */
		/* while(fgets(out_buffer,200,objdump) != NULL) { */
		/*   log("asdf\n") */
		/*   log(out_buffer) */
		/* } */
		/* pclose(objdump); */




	}

	if( DEBUG)
	  log("[*]Adding 0x%x to hash table 0x%x\n", hook->orig, hook_hash)
	    
	HASH_ADD_INT(hook_hash1, orig, hook);


	my_cacheflush((unsigned int)hook->orig, (unsigned int)hook->orig+20);
	
	if( DEBUG)
	  log("hooking finished\n")
	return 1;
}

void hook_precall(struct hook_t *h)
{
  //  log("[*]\t Precall restoring original code %x\n", h->orig)

	int i;
	
	if (h->thumb) {
		unsigned int orig = h->orig - 1;
		//log("\t\t")
		for (i = 0; i < 20; i++) {
			((unsigned char*)orig)[i] = h->storet[i];
			//	log("%0.2x ", ((unsigned char*)orig)[i] )
		}
		//log("\n")
	}
	else {
		for (i = 0; i < 3; i++)
			( (int*) h->orig)[i] = h->store[i];
	}	
	//	log("[*]\t Precall original code restored %x\n", h->orig);
	
  my_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+20);
	
}

void hook_postcall(struct hook_t *h)
{
	int i;

	//	log("[*]\t Postcall - restoring hook %x:\n", h->orig)	
	if (h->thumb) {
		unsigned int orig = h->orig - 1;
		//	log("\t\t")
		for (i = 0; i < 20; i++) {
		  ((unsigned char*)orig)[i] = h->jumpt[i];
		  //log("%0.2x ", ((unsigned char*)orig)[i] )
		  }
		//		log("\n")
	}

	else {
		for (i = 0; i < 3; i++)
			((int*)h->orig)[i] = h->jump[i];
	}

	//	log("[*]\t Postcall - hook restored %x:\n", h->orig)
	my_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+20);	
	
}

void unhook(struct hook_t *h)
{
	log("unhooking %s = %x  hook = %x ", h->name, h->orig, h->patch)
	hook_precall(h);
}

/*
 *  workaround for blocked socket API when process does not have network
 *  permissions
 *
 *  this code simply opens a pseudo terminal (pty) which gives us a
 *  file descriptor. the pty then can be used by another process to
 *  communicate with our instrumentation code. an example program
 *  would be a simple socket-to-pty-bridge
 *  
 *  this function just creates and configures the pty
 *  communication (read, write, poll/select) has to be implemented by hand
 *
 */
void start_coms()
{
	// coms is a global
	coms = open("/dev/ptmx", O_RDWR|O_NOCTTY);
	if (coms <= 0)
		log("posix_openpt failed\n")
	//else
	//	log("pty created\n")
	if (unlockpt(coms) < 0)
		log("unlockpt failed\n")
	log("pty created, file name: %s\n", ptsname(coms))
	
	struct termios  ios;
	tcgetattr(coms, &ios);
	ios.c_lflag = 0;  // disable ECHO, ICANON, etc...
	tcsetattr(coms, TCSANOW, &ios);
}


void my_init()
{
  log("[*]\tStarting hooking\n")
	
    //start_coms();
    
  int pid = getpid();

HOOK_coverage_62
HOOK_coverage_305

  log("[*]\tHooking finished\n")
}