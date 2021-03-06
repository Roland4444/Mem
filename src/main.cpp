#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <unistd.h>

#include <iostream>
#include <sys/param.h> /* for HZ */
using namespace std;

extern "C"
{
    int summ(int a, int b)
    {
        return a + b;
    }

typedef struct MEMPACKED // Define a mem occupied structure
{
  char name1[20]; // Define an array name of type char with 20 elements
  unsigned long MemTotal;
  char name2[20];
  unsigned long MemFree;
  char name3[20];
  unsigned long Buffers;
  char name4[20];
  unsigned long Cached;
  char name5[20];
  unsigned long SwapCached;
} MEM_OCCUPY;

// proc/stat file structure
// cpu  633666 46912 249878 176813696 782884 2859 19625 0
// cpu0 633666 46912 249878 176813696 782884 2859 19625 0
// intr 5812844
// ctxt 265816063
// btime 1455203832
// processes 596625
// procs_running 1
// procs_blocked 0

typedef struct CPUPACKED // Define a cpu occupied structure
{
  char name[20];       // Define an array name of type char with 20 elements
  unsigned int user;   // Define an unsigned int user
  unsigned int nice;   // Define an unsigned int type nice
  unsigned int system; // Define an unsigned int system
  unsigned int idle;   // Define an unsigned int type idle
  unsigned int lowait;
  unsigned int irq;
  unsigned int softirq;
} CPU_OCCUPY;

void
get_memoccupy (MEM_OCCUPY *mem) // For untyped get functions, there is a pointer
                                // to a formal parameter structure class O
{
  FILE *fd;
  char buff[256];
  MEM_OCCUPY *m;
  m = mem;

  fd = fopen ("/proc/meminfo", "r");
  // MemTotal: 515164 kB
  // MemFree: 7348 kB
  // Buffers: 7892 kB
  // Cached: 241852  kB
  // SwapCached: 0 kB
  // Read the string with the length of buff from the fd file and save it to the
  // space where the starting address is buff
  fgets (buff, sizeof (buff), fd);
  sscanf (buff, "%s %lu ", m->name1, &m->MemTotal);
  fgets (buff, sizeof (buff), fd);
  sscanf (buff, "%s %lu ", m->name2, &m->MemFree);
  fgets (buff, sizeof (buff), fd);
  sscanf (buff, "%s %lu ", m->name3, &m->Buffers);
  fgets (buff, sizeof (buff), fd);
  sscanf (buff, "%s %lu ", m->name4, &m->Cached);
  fgets (buff, sizeof (buff), fd);
  sscanf (buff, "%s %lu", m->name5, &m->SwapCached);

  fclose (fd); // Close the file fd
}

int
get_cpuoccupy (
  CPU_OCCUPY *cpust) // For the untyped get function contains a pointer to a
                     // formal parameter structure class O
{
  FILE *fd;
  char buff[256];
  CPU_OCCUPY *cpu_occupy;
  cpu_occupy = cpust;

  fd = fopen ("/proc/stat", "r");
  fgets (buff, sizeof (buff), fd);

  sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user,
          &cpu_occupy->nice, &cpu_occupy->system, &cpu_occupy->idle,
          &cpu_occupy->lowait, &cpu_occupy->irq, &cpu_occupy->softirq);

  fclose (fd);

  return 0;
}

int
cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n)
{
  unsigned long od, nd;
  double cpu_use = 0;

  od = (unsigned long)(o->user + o->nice + o->system + o->idle + o->lowait
                       + o->irq
                       + o->softirq); // First time (user +priority+system+idle)
                                      // time is assigned to od
  nd = (unsigned long)(n->user + n->nice + n->system + n->idle + n->lowait
                       + n->irq + n->softirq); // The second time (user
                                               // +priority+system+idle) time is
                                               // assigned to od
  double sum = nd - od;
  double idle = n->idle - o->idle;
  cpu_use = idle / sum;
 // printf ("cpu_use1(idle) = %f\r\n", cpu_use);
  idle = n->user + n->system + n->nice - o->user - o->system - o->nice;
  cpu_use = idle / sum;
//  printf ("cpu_use2(user+system+nice) = %f\r\n", cpu_use);
  return 0;
}


    long
    getfreeMem ()
    {
      long result = 0;
      MEM_OCCUPY mem_stat;
      CPU_OCCUPY cpu_stat1;
      CPU_OCCUPY cpu_stat2;

      // CAfcLowerTools m_afcLowerTools;

      get_memoccupy ((MEM_OCCUPY *)&mem_stat);
      result = mem_stat.MemFree;

      get_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1);

      get_cpuoccupy ((CPU_OCCUPY *)&cpu_stat2);

      cal_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);
      return result;
};
}

