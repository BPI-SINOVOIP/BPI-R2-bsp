/* ------------
 *   Type definition.
 */

#ifndef _MTK_DVC_TEST_TYPEDEFS_H
#define _MTK_DVC_TEST_TYPEDEFS_H


/*==== CONSTANTS ==================================================*/

#define IMPORT  EXTERN
#ifndef __cplusplus
  #define EXTERN  extern
#else
  #define EXTERN  extern "C"
#endif
#define LOCAL     static
#define GLOBAL
#define EXPORT    GLOBAL


#define EQ        ==
#define NEQ       !=
#define AND       &&
#define OR        ||
#define XOR(A,B)  ((!(A) AND (B)) OR ((A) AND !(B)))

#ifndef FALSE
  #define FALSE   0
#endif

#ifndef TRUE
  #define TRUE    1
#endif

#ifndef NULL
  #define NULL    0
#endif

#ifndef BOOL
typedef unsigned char  BOOL;
#endif

// Iverson add
#ifndef bool
typedef unsigned char  bool;
#endif

typedef volatile unsigned char  *UINT8P;
typedef volatile unsigned short *UINT16P;
typedef volatile unsigned int   *UINT32P;


typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned int    UINT32;
typedef unsigned short  USHORT;
typedef signed char     INT8;
typedef signed short    INT16;
typedef signed int      INT32;
typedef signed int      DWORD;
typedef void            VOID;
typedef unsigned char   BYTE;
typedef float           FLOAT;


typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned long long u64;

typedef unsigned long long  U64;
typedef unsigned int        U32;
typedef unsigned short      U16;
typedef unsigned char       U8;

typedef signed char         S8;
typedef signed short        S16;
typedef signed int          S32;
typedef signed long long    S64;

typedef unsigned int    kal_uint32;
typedef unsigned short  kal_uint16;
typedef unsigned char   kal_uint8;

typedef signed int      kal_int32;
typedef signed short    kal_int16;
typedef signed char     kal_int8;

typedef enum {
   KAL_FALSE = 0,
   KAL_TRUE  = 1,
} kal_bool;

/*==== EXPORT =====================================================*/

#define MAXIMUM(A,B)       (((A)>(B))?(A):(B))
#define MINIMUM(A,B)       (((A)<(B))?(A):(B))

#define READ_REGISTER_UINT32(reg) \
    (*(volatile unsigned int * const)(reg))

#define WRITE_REGISTER_UINT32(reg, val) \
    (*(volatile unsigned int * const)(reg)) = (val)

#define READ_REGISTER_UINT16(reg) \
    (*(volatile unsigned short * const)(reg))

#define WRITE_REGISTER_UINT16(reg, val) \
    (*(volatile unsigned short * const)(reg)) = (val)

#define READ_REGISTER_UINT8(reg) \
    (*(volatile unsigned char * const)(reg))

#define WRITE_REGISTER_UINT8(reg, val) \
    (*(volatile unsigned char * const)(reg)) = (val)

#define INREG8(x)           READ_REGISTER_UINT8((unsigned char *)(x))
#define OUTREG8(x, y)       WRITE_REGISTER_UINT8((unsigned char *)(x), (unsigned char)(y))
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))
#define MASKREG8(x, y, z)   OUTREG8(x, (INREG8(x)&~(y))|(z))

#define INREG16(x)          READ_REGISTER_UINT16((unsigned short *)(x))
#define OUTREG16(x, y)      WRITE_REGISTER_UINT16((unsigned short *)(x),(unsigned short)(y))
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))
#define MASKREG16(x, y, z)  OUTREG16(x, (INREG16(x)&~(y))|(z))

#define INREG32(x)          READ_REGISTER_UINT32((unsigned int *)(x))
#define OUTREG32(x, y)      WRITE_REGISTER_UINT32((unsigned int *)(x), (unsigned int )(y))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))
#define MASKREG32(x, y, z)  OUTREG32(x, (INREG32(x)&~(y))|(z))


#define DRV_Reg8(addr)              INREG8(addr)
#define DRV_WriteReg8(addr, data)   OUTREG8(addr, data)
#define DRV_SetReg8(addr, data)     SETREG8(addr, data)
#define DRV_ClrReg8(addr, data)     CLRREG8(addr, data)

#define DRV_Reg16(addr)             INREG16(addr)
#define DRV_WriteReg16(addr, data)  OUTREG16(addr, data)
#define DRV_SetReg16(addr, data)    SETREG16(addr, data)
#define DRV_ClrReg16(addr, data)    CLRREG16(addr, data)

#define DRV_Reg32(addr)             INREG32(addr)
#define DRV_WriteReg32(addr, data)  OUTREG32(addr, data)
#define DRV_SetReg32(addr, data)    SETREG32(addr, data)
#define DRV_ClrReg32(addr, data)    CLRREG32(addr, data)

// !!! DEPRECATED, WILL BE REMOVED LATER !!!
#define DRV_Reg(addr)               DRV_Reg16(addr)
#define DRV_WriteReg(addr, data)    DRV_WriteReg16(addr, data)
#define DRV_SetReg(addr, data)      DRV_SetReg16(addr, data)
#define DRV_ClrReg(addr, data)      DRV_ClrReg16(addr, data)

#endif

