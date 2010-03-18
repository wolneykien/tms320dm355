
/* PLL and Reset Controller Module Instances */

/* PLLC1 base address */
#define PLLC1_BASE 0x1c40800

/* PLLC2 base address */
#define PLLC2_BASE 0x1c40c00

/* Offset definitions for the PLL registers */

/*Contains peripheral ID and revision information */
#define PID 0x00
/*Controls PLL operations */
#define PLLCTL 0x100
/*PLL Multiplier Control */
#define PLLM 0x110
/*Pre-divider control */
#define PREDIV 0x114
/*Divider 1 control-divider for SYSCLK1 */
#define PLLDIV1 0x118
/*Divider 2 control-divider for SYSCLK2 */
#define PLLDIV2 0x11c
/*Divider 3 control-divider for SYSCLK3 */
#define PLLDIV3 0x120
/*Post-divider control */
#define POSTDIV 0x128
/*Bypass divider control */
#define BPDIV 0x12c
/*PLL controller command register */
#define PLLCMD 0x138
/*PLL controller status register */
#define PLLSTAT 0x13c
/*SYSCLKn divider ratio change and align control register */
#define ALNCTL 0x140
/*PLL divider ratio change status register */
#define DCHANGE 0x144
/*Clock enable control AUXCLK */
#define CKEN 0x148
/*Clock status for SYSCLKBP and AUXCLK */
#define CKSTAT 0x14c
/*Clock status for SYSCLKn clocks */
#define SYSTAT 0x150
/* Divider 4 control-divider for SYSCLK4 */
#define PLLDIV4 0x160
