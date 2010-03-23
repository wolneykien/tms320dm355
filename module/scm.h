
/* The system control module base address */
#define SCM_BASE 0x01c40000

/* SCM resugter offsets */

/* PINMUX0 - Pin Mux 0 (Video In) Pin Mux Register */
#define PINMUX0 0x0
/* PINMUX1 - Pin Mux 1 (Video Out) Pin Mux Register*/
#define PINMUX1 0x4
/* PINMUX2 - Pin Mux 2 (AEMIF) Pin Mux Register */
#define PINMUX2 0x8
/* PINMUX3 - Pin Mux 3 (GIO/Misc) Pin Mux Register */
#define PINMUX3 0xC
/* PINMUX4 - Pin Mux 4 (Misc) Pin Mux Register */
#define PINMUX4 0x10
/* Boot Configuration */
#define BOOTCFG 0x14
/* Multiplexing Control for Interrupts */
#define ARM_INTMUX 0x18
/* Multiplexing Control for EDMA Events */
#define EDMA_EVTMUX 0x1C
/* DDR Slew Rate */
#define DDR_SLEW 0x20
/* CLKOUT div/out Control */
#define CLKOUT 0x24
/* Device ID */
#define DEVICE_ID 0x28
/* Video DAC Configuration */
#define VDAC_CONFIG 0x2C
/* TIMER64_CTL - Timer64+ Input Control */
#define TIMER64_CTL 0x30
/* USB PHY Control */
#define USB_PHY_CTRL 0x34
/* Miscellaneous Control */
#define MISC 0x38
/* Master Priorities Reg0 */
#define MSTPRI0 0x3C
/* Master Priorities Reg1 */
#define MSTPRI1 0x40
/* VPSS Clock Mux Control */
#define VPSS_CLK_CTRL 0x44
/* DEEPSLEEP Configuration */
#define DEEPSLEEP 0x48
/* DEBOUNCE - Debounce for GIO0 Input */
#define DEBOUNCE0 0x50
/* DEBOUNCE - Debounce for GIO1 Input */
#define DEBOUNCE1 0x54
/* DEBOUNCE - Debounce for GIO2 Input */
#define DEBOUNCE2 0x58
/* DEBOUNCE - Debounce for GIO3 Input */
#define DEBOUNCE3 0x5C
/* DEBOUNCE - Debounce for GIO4 Input */
#define DEBOUNCE4 0x60
/* DEBOUNCE - Debounce for GIO5 Input */
#define DEBOUNCE5 0x64
/* DEBOUNCE - Debounce for GIO6 Input */
#define DEBOUNCE6 0x68
/* DEBOUNCE - Debounce for GIO7 Input */
#define DEBOUNCE7 0x6C
/* VTP IO Control Register */
#define VTPIOCR 0x70


/* USB-PHY bit-map */
#define USB_PHYPDWN 0x01


/* SDRAM controller base address */
#define SZ_32K 0x00008000
#define DDR_CTL_BASE 0x20000000

/* SDRCR register */
#define SDRCR 0x0c

#define SDRCR_SR_PD   BIT(23)
#define SDRCR_LPMODEN BIT(31)
