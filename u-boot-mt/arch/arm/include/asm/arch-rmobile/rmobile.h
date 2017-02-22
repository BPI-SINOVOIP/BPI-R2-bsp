#ifndef __ASM_ARCH_RMOBILE_H
#define __ASM_ARCH_RMOBILE_H

#if defined(CONFIG_RMOBILE)
#if defined(CONFIG_SH73A0)
#include <asm/arch/sh73a0.h>
#elif defined(CONFIG_R8A7740)
#include <asm/arch/r8a7740.h>
#elif defined(CONFIG_R8A7790)
#include <asm/arch/r8a7790.h>
#elif defined(CONFIG_R8A7791)
#include <asm/arch/r8a7791.h>
#else
#error "SOC Name not defined"
#endif
#endif /* CONFIG_RMOBILE */

#endif /* __ASM_ARCH_RMOBILE_H */
