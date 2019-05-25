#include <cslr.h>
#include <cslr_syscfg0_OMAPL138.h>
#include <soc_OMAPL138.h>
#define SYS_BASE           0x01C14000
#define KICK0Ra             *(unsigned int*)(SYS_BASE + 0x038)
#define KICK1Ra             *(unsigned int*)(SYS_BASE + 0x03c)
#define ARM_PRIV_MODE_KEY 327680

CSL_SyscfgRegsOvly SYS_REGS = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;
CSL_PscRegsOvly psc0Regs = (CSL_PscRegsOvly) CSL_PSC_0_REGS;

int main (void)
{
  // Intrinsic Function; Gain 'supervisor' privileges (Not required for PG2.0 silicon and above)
  _call_swi(ARM_PRIV_MODE_KEY);

  // Open Permissions to SYSCFG Registers (Not required for PG2.0 silicon and above)
  KICK0Ra = 0x83e70b13;
  KICK1Ra = 0x95A4F1E0;

  /* Set DSP boot address vector to entry point of DSP program
  This must be aligned to 1KB boundaries */
  SYS_REGS->HOST1CFG = 0x11830000;

  /* Wake up the DSP */
  CSL_FINST(psc0Regs->MDCTL[CSL_PSC_DSP], PSC_MDCTL_NEXT, ENABLE);
  CSL_FINST(psc0Regs->PTCMD, PSC_PTCMD_GO1, SET);
  while(CSL_FEXT(psc0Regs->PTSTAT, PSC_PTSTAT_GOSTAT1)==CSL_PSC_PTSTAT_GOSTAT1_IN_TRANSITION);
  CSL_FINST(psc0Regs->MDCTL[CSL_PSC_DSP], PSC_MDCTL_LRST, DEASSERT);

  while(1);
}

