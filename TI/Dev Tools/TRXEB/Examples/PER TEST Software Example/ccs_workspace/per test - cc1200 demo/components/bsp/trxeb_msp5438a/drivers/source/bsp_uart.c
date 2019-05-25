//*****************************************************************************
//! @file       bsp_uart.c
//! @brief      UART implementation to TrxEB's USB MCU.
//!
//!             Configuration options:
//!             If \b BSP_UART_ISR is defined, UART communication is CPU ISR
//!             driven.
//!
//!             If \b BSP_UART_DMA is defined, UART communication is DMA
//!             driven. DMA channels may be configured by defines
//!             \b BSP_UART_DMA_RX = x and and \b BSP_UART_DMA_TX = y where
//!             x=0,1,2 and y=0,1,2.
//!             The default channel configuration is RX=0 and TX=1
//!
//!
//!             If \b BSP_UART_ALL_OR_NOTHING is defined, functions
//!             bspUartDataGet() and bspUartDataPut() will only read(write)
//!             from(to) the UART buffer if the specified number of bytes
//!             are available (can fit) in the UART buffer. Eor example, if 3
//!             bytes are present in the UART RX buffer bspUartDataGet(myBuf, 4)
//!             will not read any bytes whereas bspUartDataGet(myBuf, 3) will
//!             read 3 bytes.
//!
//! Revised     $Date: 2013-04-11 20:13:51 +0200 (to, 11 apr 2013) $
//! Revision    $Revision: 9716 $
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************/
#ifndef BSP_UART_EXCLUDE

/**************************************************************************//**
* @addtogroup   bsp_uart_api
* @{
******************************************************************************/


/******************************************************************************
* INCLUDES
*/
#include "bsp.h"
#include "bsp_uart.h"


/******************************************************************************
* DEFINES
*/
//
// Handle different data models and compilers.
//
#if defined(__ICC430__)
#define BU_REG_TYPE             __ACCESS_20BIT_REG__
#elif defined(__TI_COMPILER_VERSION__) || defined(ccs)
#define BU_REG_TYPE             __SFR_FARPTR)(uint32_t
#endif

//
// Configuration handling:
// If neither are specified, use CPU ISR
//
#if (!defined(BSP_UART_ISR) && !defined(BSP_UART_DMA))
#define BSP_UART_ISR
#endif
//
// If both are defined, issue error
//
#if (defined(BSP_UART_ISR) && defined(BSP_UART_DMA))
#error BSP_UART_DMA and BSP_UART_ISR defined. Only one can be defined.
#endif

// DMA related defines
#if defined(BSP_UART_DMA)
// Sanity checking
#   if((defined(BSP_UART_DMA_RX) && !defined(BSP_UART_DMA_TX) &&              \
       (BSP_UART_DMA_RX ==1))    || (defined(BSP_UART_DMA_TX) &&              \
       !defined(BSP_UART_DMA_RX) && (BSP_UART_DMA_TX ==0)) ||                 \
       (defined(BSP_UART_DMA_TX) && defined(BSP_UART_DMA_RX) &&               \
       (BSP_UART_DMA_RX == BSP_UART_DMA_TX)) || ((BSP_UART_DMA_TX > 2) ||     \
       (BSP_UART_DMA_RX > 2)))
#       error DMA channel config. error! RX and TX DMA ch. cannot be same.    \
See defines BSP_UART_DMA_RX and BSP_UART_DMA_TX(0, 1, 2 is valid).
#   endif

#   ifdef BSP_UART_ALLOCATE_ISR
#       define BU_DMA_HANDLER()    _Pragma("vector=DMA_VECTOR") \
                                    __interrupt static void bspUartDmaIsr(void)
#   else
#       define BU_DMA_HANDLER()    void bspUartDmaHandler(void)
#   endif

#   define BU_DMARXBUF              UCA1RXBUF           // RX DMA source
#   if   (BSP_UART_DMA_RX == 2)     // RX DMA = ch. 2
#       define BU_RX_DMACTL         DMACTL1             // DMA control register
#       define BU_RX_DMATRIG        DMA2TSEL_20         // RX DMA trigger (USCIA1 RX)
#       define BU_RX_DMAxCTL        DMA2CTL             // Ch. control register
#       define BU_RX_DMAxSA         DMA2SA              // Src address register
#       define BU_RX_DMAxDA         DMA2DA              // Dst address register
#       define BU_RX_DMAxSZ         DMA2SZ              // Size register
#   elif (BSP_UART_DMA_RX == 1)     // RX DMA = ch. 1
#       define BU_RX_DMACTL         DMACTL0             // DMA control register
#       define BU_RX_DMATRIG        DMA1TSEL_20         // RX DMA trigger (USCIA1 RX)
#       define BU_RX_DMAxCTL        DMA1CTL             // Ch. control register
#       define BU_RX_DMAxSA         DMA1SA              // Src address register
#       define BU_RX_DMAxDA         DMA1DA              // Dst address register
#       define BU_RX_DMAxSZ         DMA1SZ              // Size register
#   else                            // RX DMA = ch. 0 (default)
#       define BU_RX_DMACTL         DMACTL0             // DMA control register
#       define BU_RX_DMATRIG        DMA0TSEL_20         // RX DMA trigger (USCIA1 RX)
#       define BU_RX_DMAxCTL        DMA0CTL             // Ch. control register
#       define BU_RX_DMAxSA         DMA0SA              // Src address register
#       define BU_RX_DMAxDA         DMA0DA              // Dst address register
#       define BU_RX_DMAxSZ         DMA0SZ              // Size register
#   endif

#   define BU_DMATXBUF              UCA1TXBUF           // RX DMA destination
#   if   (BSP_UART_DMA_TX == 2)     // RX DMA = ch. 2
#       define BU_TX_DMACTL         DMACTL1             // DMA control register
#       define BU_TX_DMATRIG        DMA2TSEL_21         // TX DMA trigger (USCIA1 transmit)
#       define BU_TX_DMAxCTL        DMA2CTL             // Ch. control register
#       define BU_TX_DMAxSA         DMA2SA              // Src address register
#       define BU_TX_DMAxDA         DMA2DA              // Dst address register
#       define BU_TX_DMAxSZ         DMA2SZ              // Size register
#   elif(defined(BSP_UART_DMA_TX)&&(BSP_UART_DMA_TX==0))// RX DMA = ch. 0
#       define BU_TX_DMACTL         DMACTL0             // DMA control register
#       define BU_TX_DMATRIG        DMA0TSEL_21         // TX DMA trigger
#       define BU_TX_DMAxCTL        DMA0CTL             // Ch. control register
#       define BU_TX_DMAxSA         DMA0SA              // Src address register
#       define BU_TX_DMAxDA         DMA0DA              // Dst address register
#       define BU_TX_DMAxSZ         DMA0SZ              // Size register
#   else                            // RX DMA = ch. 1 (default)
#       define BU_TX_DMACTL         DMACTL0             // DMA control register
#       define BU_TX_DMATRIG        DMA1TSEL_21         // TX DMA trigger
#       define BU_TX_DMAxCTL        DMA1CTL             // Ch. control register
#       define BU_TX_DMAxSA         DMA1SA              // Src address register
#       define BU_TX_DMAxDA         DMA1DA              // Dst address register
#       define BU_TX_DMAxSZ         DMA1SZ              // Size register
#   endif

// Enable/disable DMA channels
#define BU_TX_DMA_ENABLE()          { BU_TX_DMAxCTL |= DMAEN;}
#define BU_TX_DMA_DISABLE()         { BU_TX_DMAxCTL &= ~DMAEN;}
#define BU_RX_DMA_ENABLE()          { BU_RX_DMAxCTL |= DMAEN;}
#define BU_RX_DMA_DISABLE()         { BU_RX_DMAxCTL &= ~DMAEN;}

// Clear interrupt flags
#define BU_CLR_TX_DMA_STATUS()      { BU_RX_DMAxCTL &= ~DMAIFG; }
#define BU_CLR_RX_DMA_STATUS()      { BU_RX_DMAxCTL &= ~DMAIFG; }
#endif // defined(BSP_UART_DMA)

// CPU ISR related defines
#ifdef BSP_UART_ISR
#   ifdef BSP_UART_ALLOCATE_ISR
#       define BU_UART_HANDLER()    _Pragma("vector=USCI_A1_VECTOR") \
                                    __interrupt static void bspUartIsr(void)
#   else
#       define BU_UART_HANDLER()    void bspUartIsrHandler(void)
#   endif
#endif

// IO register defines
#define BU_RXTX_DIR                 P5DIR
#define BU_RXTX_SEL                 P5SEL
#define BU_RXTX_OUT                 P5OUT
#define BU_RXTX_REN                 P5REN

// Get 1 byte from UART
#define BU_GETBYTE()                UCA1RXBUF

// Put 1 byte into the UART
#define BU_PUTBYTE(x)               { UCA1TXBUF = (x); }

// Set Source Clock
#define BU_SET_SRC_CLK()            { UCA1CTL1 |= UCSSEL__SMCLK; }

// Setup TXD and RXD Port
#define BU_PORT_CONFIG()            { BU_RXTX_SEL |= (BSP_UART_TXD |          \
                                                      BSP_UART_RXD);          \
                                      BU_RXTX_DIR |=  BSP_UART_TXD;           \
                                      BU_RXTX_DIR &= ~BSP_UART_RXD; }
#define BU_PORT_UNCONFIG()          { BU_RXTX_SEL &= ~(BSP_UART_TXD |         \
                                                       BSP_UART_RXD);         \
                                      BU_RXTX_REN &= ~(BSP_UART_TXD |         \
                                                       BSP_UART_RXD);         \
                                      BU_RXTX_DIR &= ~(BSP_UART_TXD |         \
                                                       BSP_UART_RXD); }

//
// Setup format frame (disable parity, lsb first, 8bit data, 1 stop bit, uart
// mode, asynchronous mode).
//
#define BU_FRAME_CONFIG()           { UCA1CTL0 = 0; }

//
// Enable/Disable UART TX interrupts
//
#define BU_TX_INT_ENABLE()          { UCA1IE |= UCTXIE; }
#define BU_TX_INT_DISABLE()         { UCA1IE &= ~UCTXIE; }

//
// Enable/Disable UART RX interrupts
//
#define BU_RX_INT_ENABLE()          { UCA1IE |= UCRXIE; }
#define BU_RX_INT_DISABLE()         { UCA1IE &= ~UCRXIE; }

//
// Enable/Disable UART module (SWRST)
//
#define BU_SWRST_ENABLE()           { UCA1CTL1 |= UCSWRST; }
#define BU_SWRST_DISABLE()          { UCA1CTL1 &= ~UCSWRST; }

//
// Get Rx/Tx status bit
//
#define BU_GET_RX_STATUS()          (UCA1IFG & UCRXIFG)
#define BU_GET_TX_STATUS()          (UCA1IFG & UCTXIFG)
#define BU_CLR_TX_STATUS()          (UCA1IFG &= ~UCTXIFG)
#define BU_CLR_RX_STATUS()          (UCA1IFG &= ~UCRXIFG)
#define BU_SET_TX_STATUS()          (UCA1IFG |= UCTXIFG)

//
// Condition for UART TX FIFO to be ready for new data
//
#define BU_TX_BUF_READY()           BU_GET_TX_STATUS()

//
// Ringbuffer macros
//
#define BU_GET_BUF_SIZE(pRbc)       ((uint32_t)((pRbc)->end) -                \
                                     (uint32_t)((pRbc)->start))
#define BU_GET_USED_SPACE(pRbc)     ((pRbc)->numStoredBytes)
#define BU_GET_FREE_SPACE(pRbc)     (BU_GET_BUF_SIZE(pRbc) -                  \
                                     ((pRbc)->numStoredBytes))
#define BU_IS_BUF_EMPTY(pRbc)       ((pRbc)->numStoredBytes == 0)
#define BU_IS_BUF_FULL(pRbc)        (BU_GET_USED_SPACE(pRbc) ==               \
                                     BU_GET_BUF_SIZE(pRbc))
#define BU_BYTES_TO_TAIL_WRAP(pRbc) ((uint32_t)((pRbc)->end) -                \
                                     (uint32_t)((pRbc)->tail))


/******************************************************************************
* TYPEDEFS
*/

//
//! Ringbuffer implementation control structure
//
typedef struct
{
    //
    // Lowest address in buffer
    //
    uint8_t *start;

    //
    // Highest address in buffer + 1
    //
    uint8_t *end;

    //
    // Buffer data input pointer (push)
    //
    volatile uint8_t *head;

    //
    // Buffer data output pointer (pop)
    //
    volatile uint8_t *tail;

    //
    // Number of stored bytes
    //
    volatile uint16_t numStoredBytes;
}
buBufCtl_t;

#if defined(BSP_UART_DMA)
//
//! DMA TX control structure
//
typedef struct
{
    //
    // TX transfer is ongoing
    //
    volatile uint_fast8_t txActive;

    //
    // Number of bytes transferred in last burst
    //
    volatile uint_fast16_t burstSize;
}
buDmaCtl_t;
#endif // BSP_UART_DMA


/******************************************************************************
* MACROS
*/


/******************************************************************************
* LOCAL FUNCTIONS AND VARIABLES
*/
//
// Holds currently configured baud rate
//
static uint32_t ui32BuBaudRate;

//
// RX buffer control structure
//
static buBufCtl_t uuRxBuf;

//
// TX buffer control structure
//
static buBufCtl_t uuTxBuf;

#if defined(BSP_UART_DMA)
//
// DMA control structure
//
static buDmaCtl_t uuDmaCtl;
#endif

static uint8_t bspUartBaudRateSet(uint32_t ui32BaudRate);
static void bspUartBufFlush(buBufCtl_t *pBuf);
static void uuBufPopByte(void);
#if defined(BSP_UART_ISR)
static void uuBufPushByte(void);
#endif
#if defined(BSP_UART_DMA)
static void bspUartDmaInit(void);
#endif


/******************************************************************************
* FUNCTIONS
*/
/**************************************************************************//**
* @brief    Initialize buffers used by USB UART module.
*
* @return   Returns BSP_UART_SUCCESS on success.
* @return   Returns BSP_UART_ERROR on configuration error.
******************************************************************************/
uint8_t
bspUartBufInit(uint8_t *pui8TxBuf, uint16_t ui16TxByteAlloc,
               uint8_t *pui8RxBuf, uint16_t ui16RxByteAlloc)
{
    //
    // Check input arguments
    //
    if(((pui8RxBuf == 0) || (ui16RxByteAlloc == 0) || (pui8TxBuf == 0) ||      \
            (ui16TxByteAlloc == 0)))
    {
        return (BSP_UART_ERROR);
    }

    //
    // TBD: Add compile flag to use internal buffers
    //

    //
    // 1. Store pointers
    //
    uuTxBuf.start = pui8TxBuf;
    uuTxBuf.end = pui8TxBuf + ui16TxByteAlloc;
    uuRxBuf.start = pui8RxBuf;
    uuRxBuf.end = pui8RxBuf + ui16RxByteAlloc;

    //
    // 2. Reset control structure
    //
    bspUartBufFlush(&uuTxBuf);
    bspUartBufFlush(&uuRxBuf);

    //
    // Return
    //
    return (BSP_UART_SUCCESS);
}


/**************************************************************************//**
* @brief    Initializes UART communication to the TrxEB USB MCU at the baud
*           rate specified by \e ui32BaudRate. This function must be called
*           after initializing the UART buffers using bspUartBufInit().
*           The UART format between the USB MCU is 8-N-1, i.e. 8 data bits, no
*           parity bit and 1 stop bit.
*
*           The UART module is configured to use SMCLK as a clock source.
*           The applied divider is based on the SMCLK speed set by bspInit().
*
* @param    ui32BaudRate  is the UART baud rate; it must be one of the
*			following values:
*           \li \b BSP_UART_BR_9600
*           \li \b BSP_UART_BR_38400
*           \li \b BSP_UART_BR_57600
*           \li \b BSP_UART_BR_115200
*
* @return   Returns BSP_UART_SUCCESS on success
* @return   Returns BSP_UART_UNCONFIGURED if buffers are not configured.
* @return   Returns BSP_UART_BAUDRATE_ERROR if the baud rate is not supported.
******************************************************************************/
uint8_t
bspUartOpen(uint32_t ui32BaudRate)
{
    if((BU_GET_BUF_SIZE(&uuTxBuf) == 0) || (BU_GET_BUF_SIZE(&uuRxBuf) == 0))
    {
        //
        // One or more buffer size is zero -> no buffers configured
        //
        return (BSP_UART_UNCONFIGURED);
    }

    //
    // Disable module before configuring it
    //
    BU_SWRST_ENABLE();

    //
    // Set correct frame format (8-N-1)
    //
    BU_FRAME_CONFIG();

    //
    // Set clock source as SMCLK
    //
    BU_SET_SRC_CLK();

    //
    // Set baud rate based on current SMCLK frequency
    //
    if(BSP_UART_SUCCESS != bspUartBaudRateSet(ui32BaudRate))
    {
        return (BSP_UART_BAUDRATE_ERROR);
    }

    //
    // Store baudrate
    //
    ui32BuBaudRate = ui32BaudRate;

    //
    // Setup io ports
    //
    BU_PORT_CONFIG();

    //
    // Enable UART module
    //
    BU_SWRST_DISABLE();

#ifdef BSP_UART_DMA
    //
    // Configure DMA channels (set UART IFG high), and set UART TX interrupt
    // flags to a known state.
    //
    bspUartDmaInit();
    BU_SET_TX_STATUS();

    //
    // Enable RX DMA channel
    //
    BU_RX_DMAxCTL |= DMAEN;
#endif

#ifdef BSP_UART_ISR
    //
    // Clear RX and TX interrupt flags
    //
    BU_CLR_TX_STATUS();
    BU_CLR_RX_STATUS();

    //
    // Enable UART module TX and RX interrupts.
    //
    BU_TX_INT_ENABLE();
    BU_RX_INT_ENABLE();
#endif

    //
    // Report success
    //
    return (BSP_UART_SUCCESS);
}


/**************************************************************************//**
* @brief    Stops UART communication to the TrxEB USB MCU.
*
* @return   Returns BSP_UART_SUCCESS on success
******************************************************************************/
uint8_t
bspUartClose(void)
{
#ifdef BSP_UART_DMA
    //
    // Disable DMAs and clear interrupt flags
    //
    BU_RX_DMA_DISABLE();
    BU_TX_DMA_DISABLE();
    BU_CLR_RX_DMA_STATUS();
    BU_CLR_TX_DMA_STATUS();
#endif
    //
    // Disable UART module
    //
    BU_SWRST_ENABLE();

    //
    // Set pins back as gpio
    //
    BU_PORT_UNCONFIG();

    //
    // Clear baud rate variable and return status
    //
    ui32BuBaudRate = 0;
    return (BSP_UART_SUCCESS);
}


/**************************************************************************//**
* @brief    Reads up to \e ui16Length bytes from the USB UART RX buffer into
*           the buffer specified by \e pui8Data.
*
*           If \b BSP_UART_ALL_OR_NOTHING is defined, data is only read to
*           \e pui8Data if \e ui16Length or more bytes are available in the USB
*           UART RX buffer.
*
* @return   Returns the number of bytes read from the USB UART RX buffer.
******************************************************************************/
uint16_t
bspUartDataGet(uint8_t *pui8Data, uint16_t ui16Length)
{
    uint16_t ui16IntState;
    register uint16_t ui16Count, ui16N;

    //
    // Data available in RX buffer?
    //
    ui16Count = bspUartRxCharsAvail();

    //
    // No data available, return early
    //
    if(!ui16Count)
    {
        return (0);
    }
    if(ui16Length <= ui16Count)
    {
        //
        // Limited by input argument
        ui16Count = ui16Length;
    }
    else
    {
#ifdef BSP_UART_ALL_OR_NOTHING
        //
        // All or nothing
        //
        return (0);
#else
        //
        // Limited by data available
        //
        ui16Length = ui16Count;
#endif
    }

    for(ui16N = 0; ui16N < ui16Count; ui16N++)
    {
        pui8Data[ui16N] = *uuRxBuf.tail++;

        //
        // Handle ringbuffer wrap-around
        //
        if(uuRxBuf.tail == uuRxBuf.end)
        {
            uuRxBuf.tail = uuRxBuf.start;
        }
    }

    //
    // Update ringbuffer storage counter
    //
    ui16IntState = __get_interrupt_state();
    __disable_interrupt();
    uuRxBuf.numStoredBytes -= ui16N;
    __set_interrupt_state(ui16IntState);

    //
    // Return number of bytes read
    //
    return (ui16N);
}


/**************************************************************************//**
* @brief    Puts \e ui16Length bytes (or as many bytes free in USB UART TX buffer)
*           into the USB UART TX buffer and starts transferring data over UART.
*
*           If \e BSP_UART_ALL_OR_NOTHING is defined, data is only
*           put into the TX buffer if there is room for all \e ui16Length bytes.
*
* @return   Returns the number of bytes copied to the USB UART RX buffer.
******************************************************************************/
uint16_t
bspUartDataPut(uint8_t *pui8Data, uint16_t ui16Length)
{
    uint16_t ui16IntState;
    register uint16_t ui16Bytes, ui16N;
#ifdef BSP_UART_DMA
    uint16_t ui16BytesToTailWrap;
#endif

    //
    // Get free space in the TX buffer
    //
    ui16Bytes = bspUartTxSpaceAvail();

    //
    // Return if no bytes are to be put or there is no space available
    //
    if((!ui16Length) || (!ui16Bytes))
    {
        return (0);
    }

    //
    // Limit transfer count
    //
    if(ui16Length <= ui16Bytes)
    {
        //
        // Limited by input argument
        //
        ui16Bytes = ui16Length;
    }
#ifdef BSP_UART_ALL_OR_NOTHING
    else
    {
        //
        // All or nothing
        //
        return (0);
    }
#endif

    //
    // Copy data to TX buffer
    //
    for(ui16N = 0; ui16N < ui16Bytes; ui16N++)
    {
        *uuTxBuf.head++ = pui8Data[ui16N];

        //
        // Manage wrap-around
        //
        if(uuTxBuf.head == uuTxBuf.end)
        {
            uuTxBuf.head = uuTxBuf.start;
        }
    }

    //
    // Critical section start
    //
    ui16IntState = __get_interrupt_state();
    __disable_interrupt();

    //
    // Store current byte count before updating it
    //
    ui16Bytes = uuTxBuf.numStoredBytes;
    uuTxBuf.numStoredBytes += ui16N;

    //
    // If no bytes were initially in buffer, trigger TX by pushing byte to FIFO
    //
    if(!ui16Bytes)
    {
        //
        // Clear UART TX interrupt flag
        //
        BU_CLR_TX_STATUS();

#ifdef BSP_UART_DMA
        //
        // Only configure DMA if we're transferring more than 1 byte.
        //
        if((ui16N > 1))
        {
            //
            // End critical section
            //
            __set_interrupt_state(ui16IntState);

            //
            // Wait for any ongoing DMA transfers to complete
            //
            while(uuDmaCtl.txActive)
            {
            }

            //
            // Start critical section
            //
            ui16IntState = __get_interrupt_state();
            __disable_interrupt();

            //
            // Configure DMA
            // 1. Set transfer size
            //
            ui16BytesToTailWrap = BU_BYTES_TO_TAIL_WRAP(&uuTxBuf);
            if((ui16N > ui16BytesToTailWrap) && (ui16BytesToTailWrap > 1))
            {
                ui16N = ui16BytesToTailWrap;
            }

            //
            // Substract trigger byte
            //
            BU_TX_DMAxSZ = ui16N - 1;

            //
            // 2. Set source address
            //
            if(ui16BytesToTailWrap > 1)
            {
                BU_TX_DMAxSA = (BU_REG_TYPE)((uuTxBuf.tail) + 1);
            }
            else
            {
                //
                // DMA starts transferring at lowest ringbuffer position
                //
                BU_TX_DMAxSA = (BU_REG_TYPE)(uuTxBuf.start);
            }

            //
            // 3. Update control structure
            //
            uuDmaCtl.txActive = 1;
            uuDmaCtl.burstSize = ui16N;

            //
            // 4. Enable TX DMA
            //
            BU_TX_DMAxCTL  |= DMAEN;
        }
#endif // BSP_UART_DMA

        //
        // Pop byte to TX fifo
        //
        uuBufPopByte();
    }

    //
    // End of critical section
    //
    __set_interrupt_state(ui16IntState);

    //
    // Return number of bytes actually transferred
    //
    return (ui16N);
}


/**************************************************************************//**
* @brief    Returns the number of unused bytes in the USB UART TX buffer.
*
* @return   Returns the number of unused bytes in the USB UART TX buffer.
******************************************************************************/
uint16_t
bspUartTxSpaceAvail(void)
{
    uint16_t ui32Space;

    //
    // Enter critical section (reading volatile variables)
    //
    uint16_t ui16IntState = __get_interrupt_state();
    __disable_interrupt();
    ui32Space = BU_GET_FREE_SPACE(&uuTxBuf);
    __set_interrupt_state(ui16IntState);

    //
    // End of critical section. Return space available
    //
    return (ui32Space);
}


/**************************************************************************//**
* @brief    Flushes the USB UART RX buffer. Function resets the buffer control
*           structure.
*
* @return   None
******************************************************************************/
void bspUartRxFlush(void)
{
#ifdef BSP_UART_DMA
    //
    // Stop DMA channel and reset it
    //
    BU_RX_DMA_DISABLE();
    BU_RX_DMAxDA = (BU_REG_TYPE)uuRxBuf.start;
#endif

    //
    // Update buffer pointers
    //
    bspUartBufFlush(&uuRxBuf);

#ifdef BSP_UART_DMA
    //
    // Enable DMA channel
    //
    BU_CLR_RX_DMA_STATUS();
    BU_RX_DMA_ENABLE();
#endif
}


/**************************************************************************//**
* @brief    Flushes the USB UART TX buffer. Function resets the buffer control
*           structure.
*
* @return   None
******************************************************************************/
void
bspUartTxFlush(void)
{
#ifdef BSP_UART_DMA
    //
    // Disable DMA channel and reset DMA destination address
    //
    BU_TX_DMA_DISABLE();
    BU_TX_DMAxDA = (BU_REG_TYPE)uuRxBuf.start;
#endif

    //
    // Update buffer pointers
    //
    bspUartBufFlush(&uuTxBuf);

#ifdef BSP_UART_DMA
    //
    // Enable DMA channel
    //
    BU_CLR_TX_DMA_STATUS();
    BU_TX_DMA_ENABLE();
#endif
}


/**************************************************************************//**
* @brief    Returns the number bytes available in the USB UART RX buffer.
*
* @brief    Returns the number bytes available in the USB UART RX buffer.
******************************************************************************/
uint16_t bspUartRxCharsAvail(void)
{
    uint16_t ui32Space;

    //
    // Enter critical section
    //
    uint16_t ui16IntState = __get_interrupt_state();
    __disable_interrupt();
    ui32Space = BU_GET_USED_SPACE(&uuRxBuf);
    __set_interrupt_state(ui16IntState);

    //
    // end of critical section. Return byte count.
    //
    return (ui32Space);
}


/**************************************************************************//**
* @brief    Get the current USB UART baud rate in baud. Return zero if no
*           UART connection is open.
*
* @brief    Returns the current USB UART baud rate in baud.
******************************************************************************/
uint32_t
bspUartBaudRateGet(void)
{
    return (ui32BuBaudRate);
}


/******************************************************************************
* LOCAL FUNCTIONS
*/
/**************************************************************************//**
* @brief    Pops a byte from the UART TX buffer to the UART TX FIFO. Handles
*           TX buffer tail wrap-around. Does not handle buffer underrun.
*           Function modifies volatile variables and should only be called when
*           interrupts are disabled.
*
* @brief    None
******************************************************************************/
static void
uuBufPopByte(void)
{
    //
    // Update byte count
    //
    uuTxBuf.numStoredBytes--;

    //
    // Send byte to TX FIFO
    //
    BU_PUTBYTE((*uuTxBuf.tail++));

    //
    // Manage ringbuffer wrap-around
    //
    if(uuTxBuf.tail == uuTxBuf.end)
    {
        uuTxBuf.tail = uuTxBuf.start;
    }
}


#ifdef BSP_UART_ISR
/**************************************************************************//**
* @brief    Pushes a byte from the UART RX FIFO to the USB UART RX buffer.
*           Handles RX buffer wrap-around. Does not handle RX buffer overflow!
*           Function modifies volatile variables and should only be called when
*           interrupts are disabled.
*
* @brief    None
******************************************************************************/
static void
uuBufPushByte(void)
{
    //
    // Push byte from RX FIFO to buffer
    //
    *uuRxBuf.head++ = BU_GETBYTE();

    //
    // Update byte count
    //
    uuRxBuf.numStoredBytes++;

    //
    // Manage wrap-around
    //
    if(uuRxBuf.head == uuRxBuf.end)
    {
        uuRxBuf.head = uuRxBuf.start;
    }
}
#endif // BSP_UART_ISR


/**************************************************************************//**
* @brief    Sets the USB UART baudrate according to the baud rate specified by
*           \e ui32BaudRate. Function uses the value returned by
*           bspSysClockSpeedGet() to configure the UART module for correct
*           baudrate, i.e. it is assumed that bspInit() has previously been
*           called.
*
* @brief    Returns BSP_UART_SUCCESS, ...
******************************************************************************/
static uint8_t
bspUartBaudRateSet(uint32_t ui32BaudRate)
{
    uint32_t ui32Smclk = bspSysClockSpeedGet();

    if(ui32Smclk == BSP_SYS_CLK_25MHZ)
    {
        switch(ui32BaudRate)
        {
        case BSP_UART_BR_9600:
            UCA1BRW = 2604;
            UCA1MCTL = UCBRS_1;
            break;
        case BSP_UART_BR_38400:
            UCA1BRW = 651;
            UCA1MCTL = UCBRS_0;
            break;
        case BSP_UART_BR_57600:
            UCA1BRW = 434;
            UCA1MCTL = UCBRS_0;
            break;
        case BSP_UART_BR_115200:
            UCA1BRW = 217;
            UCA1MCTL = UCBRS_0;
            break;
        }
        return (BSP_UART_SUCCESS);
    }
    else if(ui32Smclk == BSP_SYS_CLK_20MHZ)
    {
        //
        // Values taken from MSP430F5438A user's guide
        //
        switch(ui32BaudRate)
        {
        case BSP_UART_BR_9600:
            UCA1BRW = 2083;
            UCA1MCTL = UCBRS_2;
            break;
        case BSP_UART_BR_38400:
            UCA1BRW = 520;
            UCA1MCTL = UCBRS_7;
            break;
        case BSP_UART_BR_57600:
            UCA1BRW = 347;
            UCA1MCTL = UCBRS_2;
            break;
        case BSP_UART_BR_115200:
            UCA1BRW = 173;
            UCA1MCTL = UCBRS_5;
            break;
        }
        return (BSP_UART_SUCCESS);
    }
    else if(ui32Smclk == BSP_SYS_CLK_16MHZ)
    {
        //
        // Values taken from MSP430F5438A user's guide
        //
        switch(ui32BaudRate)
        {
        case BSP_UART_BR_9600:
            UCA1BRW = 1666;
            UCA1MCTL = UCBRS_6;
            break;
        case BSP_UART_BR_38400:
            UCA1BRW = 416;
            UCA1MCTL = UCBRS_6;
            break;
        case BSP_UART_BR_57600:
            UCA1BRW = 277;
            UCA1MCTL = UCBRS_7;
            break;
        case BSP_UART_BR_115200:
            UCA1BRW = 138;
            UCA1MCTL = UCBRS_7;
            break;
        }
        return (BSP_UART_SUCCESS);
    }
    else if(ui32Smclk == BSP_SYS_CLK_12MHZ)
    {
        //
        // Values taken from MSP430F5438A user's guide
        //
        switch(ui32BaudRate)
        {
        case BSP_UART_BR_9600:
            UCA1BRW = 1250;
            UCA1MCTL = UCBRS_0;
            break;
        case BSP_UART_BR_38400:
            UCA1BRW = 312;
            UCA1MCTL = UCBRS_4;
            break;
        case BSP_UART_BR_57600:
            UCA1BRW = 208;
            UCA1MCTL = UCBRS_2;
            break;
        case BSP_UART_BR_115200:
            UCA1BRW = 104;
            UCA1MCTL = UCBRS_1;
            break;
        }
        return (BSP_UART_SUCCESS);
    }
    else if(ui32Smclk == BSP_SYS_CLK_8MHZ)
    {
        //
        // Values taken from MSP430F5438A user's guide
        //
        switch(ui32BaudRate)
        {
        case BSP_UART_BR_9600:
            UCA1BRW = 833;
            UCA1MCTL = UCBRS_2;
            break;
        case BSP_UART_BR_38400:
            UCA1BRW = 208;
            UCA1MCTL = UCBRS_3;
            break;
        case BSP_UART_BR_57600:
            UCA1BRW = 138;
            UCA1MCTL = UCBRS_7;
            break;
        case BSP_UART_BR_115200:
            UCA1BRW = 69;
            UCA1MCTL = UCBRS_4;
            break;
        }
        return (BSP_UART_SUCCESS);
    }
    else if(ui32Smclk == BSP_SYS_CLK_4MHZ)
    {
        //
        // Values taken from MSP430F5438A user's guide
        //
        switch(ui32BaudRate)
        {
        case BSP_UART_BR_9600:
            UCA1BRW = 416;
            UCA1MCTL = UCBRS_6;
            break;
        case BSP_UART_BR_38400:
            UCA1BRW = 104;
            UCA1MCTL = UCBRS_1;
            break;
        case BSP_UART_BR_57600:
            UCA1BRW = 69;
            UCA1MCTL = UCBRS_4;
            break;
        case BSP_UART_BR_115200:
            UCA1BRW = 34;
            UCA1MCTL = UCBRS_6;
            break;
        }
        return (BSP_UART_SUCCESS);
    }
    else if(ui32Smclk == BSP_SYS_CLK_1MHZ)
    {
        //
        // Values taken from MSP430F5438A user's guide
        //
        switch(ui32BaudRate)
        {
        case BSP_UART_BR_9600:
            UCA1BRW = 104;
            UCA1MCTL = UCBRS_1;
            break;
        case BSP_UART_BR_38400:
            UCA1BRW = 26;
            UCA1MCTL = UCBRS_0;
            break;
        case BSP_UART_BR_57600:
            UCA1BRW = 17;
            UCA1MCTL = UCBRS_3;
            break;
        case BSP_UART_BR_115200:
            UCA1BRW = 8;
            UCA1MCTL = UCBRS_6;
            break;
        }
        return (BSP_UART_SUCCESS);
    }

    return (BSP_UART_BAUDRATE_ERROR);
}


/**************************************************************************//**
* @brief    Flushes the rinbuffer control structure specified by \e pBuf.
*
* @return   None
******************************************************************************/
static void
bspUartBufFlush(buBufCtl_t *pBuf)
{
    //
    // Start of critical section
    //
    uint16_t ui16IntState = __get_interrupt_state();
    __disable_interrupt();

    //
    // Reset control structure
    //
    (pBuf)->head = (pBuf)->start;
    (pBuf)->tail = (pBuf)->start;
    (pBuf)->numStoredBytes = 0;

    //
    // Return to previous interrupt state
    //
    __set_interrupt_state(ui16IntState);
}

#if defined(BSP_UART_DMA)
/**************************************************************************//**
* @brief    Function initializes the USB UART DMA channels. The channels
*           used are defined by global defines \b BSP_UART_DMA_RX = x and
*           \b BSP_UART_DMA_TX = y, where x and y can be 0, 1 or 2.
*
* @return   None
******************************************************************************/
static void
bspUartDmaInit(void)
{
    //
    // DMA Channel for RX:
    // Trigger select: USCIA1 receive
    //
    BU_RX_DMACTL = BU_RX_DMATRIG;

    //
    // Repeated single transfer, destination address is incremented,
    // source address is unchanged, DMA destination byte == byte size,
    // DMA source byte == byte size, // DMA interrupt enabled
    //
    BU_RX_DMAxCTL = (DMADT_0 | DMADSTINCR_3 | DMASRCINCR_0 | DMADSTBYTE |     \
                     DMASRCBYTE | DMAIE);

    //
    // DMA Source address register = uart rx
    //
    BU_RX_DMAxSA = (BU_REG_TYPE)&BU_DMARXBUF;

    //
    // DMA destination address register = SW Rx data buffer
    //
    BU_RX_DMAxDA = (BU_REG_TYPE)uuRxBuf.head;

    //
    // DMA size address register = 1
    //
    BU_RX_DMAxSZ = 1;

    //
    // TX DMA channel
    //
#if (BU_RX_DMACTL == BU_TX_DMACTL)
    //
    // Trigger is set in the same control register
    //
    BU_TX_DMACTL |= BU_TX_DMATRIG;
#else
    //
    // Trigger is set in different control register
    //
    BU_TX_DMACTL = BU_TX_DMATRIG;
#endif

    //
    // Burst block transfer, destination addr does not increment, source
    // address is incremented, DMA destination byte == byte,
    // DMA source byte == byte, DMA interrupt enabled.
    /
    BU_TX_DMAxCTL = (DMADT_0 | DMADSTINCR_0 | DMASRCINCR_3 | DMADSTBYTE |     \
                     DMASRCBYTE | DMAIE);

    //
    // DMA destination address register = UART TX
    //
    BU_TX_DMAxDA = (BU_REG_TYPE)&BU_DMATXBUF;

    //
    // DMA Size address register = 1
    //
    BU_TX_DMAxSZ = 1;

    //
    // DMA4 Erratum fix - disallow DMA interrupt of CPU read-modify-write
    // instructions.
    //
    DMACTL4 = DMARMWDIS_L;
}


/**************************************************************************//**
* @brief    USB UART DMA interrupt service routine.
*
* @return   None
******************************************************************************/
BU_DMA_HANDLER()
{
    register uint16_t ui16TransferSize;

    //
    // RX DMA interrupt
    //
    if(BU_RX_DMAxCTL & DMAIFG)
    {
        //
        // Clear RX DMA interrupt flag
        //
        BU_CLR_RX_DMA_STATUS();

        //
        // Only increment head, DMA config and numStoredBytes if there's
        // room for another byte. Check is on > 1 because we've already written
        // the byte to the buffer, but not updated the pointers.
        //
        if(BU_GET_FREE_SPACE(&uuRxBuf) > 1)
        {
            //
            // Increment header and handle wrap-around
            //
            if(++uuRxBuf.head == uuRxBuf.end)
            {
                uuRxBuf.head = uuRxBuf.start;
            }
            //
            // Update DMA destination address
            //
            BU_RX_DMAxDA = (BU_REG_TYPE)(uuRxBuf.head);

            //
            // Update bytecount
            //
            uuRxBuf.numStoredBytes++;
        }

        //
        // Re-enable RX DMA
        //
        BU_RX_DMA_ENABLE();
    }

    //
    // TX DMA interrupt
    //
    if(BU_TX_DMAxCTL & DMAIFG)
    {
        //
        // Store value for quick access. -1 because of trigger byte.
        //
        ui16TransferSize = uuDmaCtl.burstSize - 1;

        //
        // Update numStoredbytes (based on completed DMA transfer)
        //
        uuTxBuf.numStoredBytes -= ui16TransferSize;

        //
        // Update tail and handle wrap-around
        //
        uuTxBuf.tail += ui16TransferSize;
        if(uuTxBuf.tail == uuTxBuf.end)
        {
            uuTxBuf.tail = uuTxBuf.start;
        }

        //
        // Wait until TX FIFO is ready for more data
        //
        while(!BU_TX_BUF_READY())
        {
        }

        //
        // Clear interrupt flags (UART and DMA)
        //
        BU_CLR_TX_STATUS();
        BU_CLR_TX_DMA_STATUS();

        //
        // Start new DMA transfer?
        //
        ui16TransferSize = uuTxBuf.numStoredBytes;
        if(ui16TransferSize > 1)
        {
            //
            // DMA will transfer data, what comes first - wrap or bytecount?
            //
            if(ui16TransferSize > (BU_BYTES_TO_TAIL_WRAP(&uuTxBuf)))
            {
                ui16TransferSize = BU_BYTES_TO_TAIL_WRAP(&uuTxBuf);
            }
            uuDmaCtl.burstSize = ui16TransferSize;

            //
            // Update DMA configuration (source address, size), enable TX DMA.
            //
            BU_TX_DMAxSA = (BU_REG_TYPE)((uuTxBuf.tail) + 1);
            BU_TX_DMAxSZ = ui16TransferSize - 1;
            BU_TX_DMA_ENABLE();

            //
            // Send trigger byte to TX FIFO
            //
            uuBufPopByte();
        }
        else
        {
            if(ui16TransferSize)
            {
                //
                // Only 1 byte to transfer: the trigger byte
                //
                uuBufPopByte();

                //
                // TBD: Wait for byte to be transferred?
                //
            }
            //
            // Set TX state
            //
            uuDmaCtl.txActive = 0;
            uuDmaCtl.burstSize = 0;
        }
    }
}
#endif // BSP_UART_DMA


#ifdef BSP_UART_ISR
/**************************************************************************//**
* @brief    USB UART interrupt service routine.
*
* @return   None
******************************************************************************/
BU_UART_HANDLER()
{
    //
    // RX
    //
    if(UCA1IFG & UCRXIFG)
    {
        //
        // Clear RX flag
        //
        BU_CLR_RX_STATUS();

#ifdef BSP_UART_ISR
        //
        // Push byte to buffer if there is room
        //
        if(!BU_IS_BUF_FULL(&uuRxBuf))
        {
            uuBufPushByte();
        }
#endif // BSP_UART_ISR
    }

    //
    // TX
    //
    if(UCA1IFG & UCTXIFG)
    {
        //
        // Clear  TX flag
        //
        BU_CLR_TX_STATUS();

#ifdef BSP_UART_ISR
        if(!BU_IS_BUF_EMPTY(&uuTxBuf))
        {
            uuBufPopByte();
        }
#endif // BSP_UART_ISR
    }
}
#endif // BSP_UART_ISR


/**************************************************************************//**
* Close the Doxygen group.
* @}
******************************************************************************/
#endif // #ifndef BSP_UART_EXCLUDE
