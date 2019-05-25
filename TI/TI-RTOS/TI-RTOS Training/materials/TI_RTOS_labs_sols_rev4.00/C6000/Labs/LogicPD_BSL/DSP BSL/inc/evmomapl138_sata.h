//-----------------------------------------------------------------------------
// \file    evmomapl138_usb.h
// \brief   OMAP-L138 usb host and otg registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_SATA_H
#define EVMOMAPL138_SATA_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------



#define NUMOFPORTS (1) // Freon Supports A Single HBA Port. However it can support up to
// 16 additional Ports with the use of an external Port Multiplier.
// So keep this value to 1.
#define LISTLENGTH (2) // Max Command Header Per Port is 32
#define WRITE_CMD_SLOT (0) // Value used here should be < = LISTLENGTH-1
#define READ_CMD_SLOT (1) // Value used here should be < = LISTLENGTH-1

// WARNING. PRDLENGTH can not be greater than 8 for this program.
// See Note captured by the area when memory has been reserved for
// within sata_utilities.c for Command Table "CmdTable" for
// more information.
#if 1
	#define _MAX_DATA_TRANSFER_ // Define this in project file when needed.
#endif

#ifndef _MAX_DATA_TRANSFER_ // 512 Bytes Data Size within 2 PRD Descriptors.
	#define PRDLENGTH (2) // Max PRD Length is 65535 per port.
	#define DATABUFFERLEN (256) // DMA Data Buffer Length
#else // Max Data Size Transfer 8K Bytes within 2 PRD Descriptors
	#define PRDLENGTH (2) // Max PRD Length is 65535 per port.
	#define DATABUFFERLEN (2*4096) // DMA Data Buffer Length
#endif

#if ((PRDLENGTH > 8) | (WRITE_CMD_SLOT > LISTLENGTH-1) | (READ_CMD_SLT > LISTLENGTH-1))
	#error PRDLENGTH ENTRY ERROR - PROGRAM HARD CODED FOR MAX VALUE OF 8 - CMD SLOT ENTRY ERROR
#endif

#define DESIRED_SPEED (GEN1) // GOASFASTASDEVICE, GEN1, GEN2
#define DEVICE_LBA_ADDRESS (0x00000002) // Dev28bitLbaAddress = 28-Bit LBA Address
#define WAIT_500_MILLISECONDS (50) // This should be set to 500 once the ONE_MS_VALUE is programmed correctly.
#define WAIT_1_MILLISECOND (1)
#define ONE_MS_VALUE (1) // Number of CPU Cycles needed to generate a millisecond wait time.
#define DMA_BURST_LENGTH (0x9) // [0x0 - 0x9] Burst=2 ^(-1) i.e., 0x8= > 2 ^(9-1)=256
#define DMA_TRANSACTION_SIZE (0x4) // [0x0 - 0xA] TransSize=2 ^n i.e., 0xA= > 2 ^10=1024

/////////////////////////////////////////////////////////////////////////////////////////////
// Maximum of 32 commands slots per port exist where each command occupies 8 DWs (64 Bytes).
// The structure 'CmdListHeader' defines a single command header definition.
// The start of the first Command List & CmdListHeader[0] needs to be programmed onto P0CLB.
//
// Command List Base Address should be 1K Byte Aligned.
typedef struct {
	uint32_t CmdLen:5;		//bits[4:0]
	uint32_t Atapi:1;		//bit[5]
	uint32_t Write:1;		//bit[6]
	uint32_t Prefetch:1;	//bit[7]
	uint32_t Reset:1;		//bit[8]
	uint32_t Bist:1;		//bit[9]
	uint32_t Rok:1;			//bit[10]
	uint32_t Rsv:1;			//bit[11]
	uint32_t Pmp:4;			//bits[15:12]
	uint32_t Prdtl:16;		//bits[31:16]
}CmdListHeaderW0;

typedef struct {
	uint32_t PrdByteCnt; //bits[31:0]
}CmdListHeaderW1;

typedef struct {
// 	uint32_t CmdTableAddLowRsv:7; //bit[6:0]
//	uint32_t CmdTableAddLow:25; //bits[31:7]
	uint32_t CmdTableAddLow; //bits[31:7]
}CmdListHeaderW2;

typedef struct {
	uint32_t CmdTableAddHigh; //bits[31:0]
}CmdListHeaderW3;

typedef struct {
	CmdListHeaderW0 DW0;
	CmdListHeaderW1 DW1;
	CmdListHeaderW2 DW2;
	CmdListHeaderW3 DW3;
	uint32_t DW4;
	uint32_t DW5;
	uint32_t DW6;
	uint32_t DW7;
} CmdListHeader;

typedef struct {
	uint32_t B0FisType:8;	//bits[7:0]
	uint32_t BYTE1:8;		//bits[15:8]
	uint32_t B2Cmd:8;		//bits[23:16]
	uint32_t B3Feature:8;	//bits[31:24]
}CmdFisWord0;

typedef struct {
	uint32_t B0LbaLow:8;	//bits[7:0]
	uint32_t B1LbaMid:8;	//bits[15:8]
	uint32_t B2LbaHigh:8;	//bits[23:16]
	uint32_t B3Device:8;	//bits[31:24]
}CmdFisWord1;

typedef struct {
	uint32_t B0LbaLowExp:8;		//bits[7:0]
	uint32_t B1LbaMidExp:8;		//bits[15:8]
	uint32_t B2LbaHighExp:8;	//bits[23:16]
	uint32_t B3FeatureExp:8;	//bits[31:24]
}CmdFisWord2;

typedef struct {
	uint32_t B0SecCnt:8;		//bits[7:0]
	uint32_t B1SecCntExp:8;		//bits[15:8]
	uint32_t B2Rsv:8;			//bits[23:16]
	uint32_t B3Control:8;		//bits[31:24]
}CmdFisWord3;

typedef struct {
	uint32_t DWResv;			//bits[31:0]
}CmdFisWord4;

typedef struct {
	CmdFisWord0 DW0;
	CmdFisWord1 DW1;
	CmdFisWord2 DW2;
	CmdFisWord3 DW3;
	CmdFisWord4 DW4;
	uint32_t DW5;
	uint32_t DW6;
	uint32_t DW7;
	uint32_t DW8;
	uint32_t DW9;
	uint32_t DW10;
	uint32_t DW11;
	uint32_t DW12;
	uint32_t DW13;
	uint32_t DW14;
	uint32_t DW15;
}CommandFIS;

//-----------Command FIS end ATAPI Command ------

// ATAPI Command Data Structure
typedef struct {
	uint32_t ATAPI[4];
}Atapi;

//-----------ATAPI Command end PRDT -------------
// Physical Region Descriptor Table Data Structure
typedef struct {
	uint32_t DbaLow;		//bits[31:0]
}DbaAddressLow;

typedef struct {
	uint32_t DbaHigh;		//bits[31:0]
}DbaAddressHigh;

typedef struct {
	uint32_t DW2Reserved;	//bits[31:0]
}PrdtRsv;

typedef struct {
	uint32_t DataBC:22;		//bits[21:0]
}DataByteCnt;

typedef struct {
	DbaAddressLow DW0;
	DbaAddressHigh DW1;
	PrdtRsv DW2;
	DataByteCnt DW3;
}PRDT;

//-----------PRDT end ---------------------------

//-----------Command Table Data Structure -------
// Since Command Table has to be 128 bytes = 0x80 bytes aligned if supporting more
// than a single Command Header, then need to make sure that the Array you are
// creating for all associated Command Tables match the 128 bytes alignment.
// In order to do so, the number of PRD Table length you are allocating should be
// multiples of 8.
typedef struct {
	CommandFIS cfis;
	Atapi atapi;
	uint32_t Rsv[12];
	PRDT prdTable[16];	// Have forced this size to 8 in order to meet the minimum
	// required size for Command Table.
}CommandTable;

//-----------Command Table Data Structure end ---



/////////////////////////////////////////////////////////////////////////////////////////////
// Receive FIS requires the Receive FIS to be 256 byte aligned. P0FB should be programmed
// with this restriction.
//
// RECEIVE FIS Data Structure
// Members: DMA Setup FIS (DSFIS)
// PIO Setup FIS (PSFIS)
// D2H Register FIS (RFIS)
// Set Device Bits FIS (SDBFIS)
// Unknown FIS (UFIS)
//-----------DMA Setup FIS-----------------------
typedef struct {
	uint32_t B0FisType:8;	//bits[7:0]
	uint32_t BYTE1:8;		//bits[15:8]
	uint32_t B2Rsv:8;		//bits[23:16]
	uint32_t B3Rsv:8;		//bits[31:24]
}DsfisW0;

typedef struct {
	DsfisW0 DW0;
	uint32_t DW1DmaBuffLow;
	uint32_t DW2DmaBuffHigh;
	uint32_t DW3Rsv;
	uint32_t DW4DmaBuffOffset;
	uint32_t DW5DmaXfrCnt;
	uint32_t DW6Rsv;
}DMASetupFis;

//-----------DMA Setup FIS end PIO Setup FIS ----
typedef struct {
	uint32_t B0FisType:8;	//bits[7:0]
	uint32_t BYTE1:8;		//bits[15:8]
	uint32_t B2Status:8;	//bits[23:16]
	uint32_t B3Errror:8;	//bits[31:24]
}PioSetupDW0;

typedef struct {
	uint32_t B0LbaLow:8;	//bits[7:0]
	uint32_t B1LbaMid:8;	//bits[15:8]
	uint32_t B2LbaHigh:8;	//bits[23:16]
	uint32_t B3Device:8;	//bits[31:24]
}PioSetupDW1;

typedef struct {
	uint32_t B0LbaLowExp:8;		//bits[7:0]
	uint32_t B1LbaMidExp:8;		//bits[15:8]
	uint32_t B2LbaHighExp:8;	//bits[23:16]
	uint32_t B3Rsv:8;			//bits[31:24]
}PioSetupDW2;

typedef struct {
	uint32_t B0SecCnt:8;		//bits[7:0]
	uint32_t B1SecCntExp:8;		//bits[15:8]
	uint32_t B2Rsv:8;			//bits[23:16]
	uint32_t B3Estatus:8;		//bits[31:24]
}PioSetupDW3;

typedef struct {
	uint32_t HW0XferCnt:16;		//bits[15:0]
	uint32_t HW1Rsv:16;			//bits[31:16]
}PioSetupDW4;

typedef struct {
	PioSetupDW0 DW0;
	PioSetupDW1 DW1;
	PioSetupDW2 DW2;
	PioSetupDW3 DW3;
	PioSetupDW4 DW4;
}PIOSetupFis;

//-----------PIO Setup FIS end D2H Reg FIS-------
typedef struct {
	uint32_t B0FisType:8;		//bits[7:0]
	uint32_t BYTE1:8;			//bits[15:8]
	uint32_t B2Status:8;		//bits[23:16]
	uint32_t B3Errror:8;		//bits[31:24]
}D2HRegDW0;

typedef struct {
	uint32_t B0LbaLow:8;		//bits[7:0]
	uint32_t B1LbaMid:8;		//bits[15:8]
	uint32_t B2LbaHigh:8;		//bits[23:16]
	uint32_t B3Device:8;		//bits[31:24]
}D2HRegDW1;

typedef struct {
	uint32_t B0LbaLowExp:8;		//bits[7:0]
	uint32_t B1LbaMidExp:8;		//bits[15:8]
	uint32_t B2LbaHighExp:8;	//bits[23:16]
	uint32_t B3Rsv:8;			//bits[31:24]
}D2HRegDW2;

typedef struct {
	uint32_t B0SecCnt:8;		//bits[7:0]
	uint32_t B1SecCntExp:8;		//bits[15:8]
	uint32_t HW1Rsv:16;			//bits[31:16]
}D2HRegDW3;

typedef struct {
	uint32_t W0Rsv;				//bits[31:0]
}D2HRegDW4;

typedef struct {
	D2HRegDW0 DW0;
	D2HRegDW1 DW1;
	D2HRegDW2 DW2;
	D2HRegDW3 DW3;
	D2HRegDW4 DW4;
}D2HRegFis;


//-----------D2H Reg FIS end Set Device Bits FIS-
// The Set Device Bit FIS definition does not contain the 2nd Word required
// for Native Command Queueing. This second word is the SACTVE register and
// the AHCI takes care of updating SACTIVE register at its location.
typedef struct {
	uint32_t B0FisType:8;		//bits[7:0]
	uint32_t BYTE1:8;			//bits[15:8]
	uint32_t B2Status:8;		//bits[23:16]
	uint32_t B3Errror:8;		//bits[31:24]
}SetDevBitsDW0;


typedef struct {
	uint32_t W1Rsv;				//bits[31:0]
}SetDevBitsDW1;

typedef struct {
	SetDevBitsDW0 DW0;
	SetDevBitsDW1 DW1;
}SetDevBitsFis;

//-----------Set Device Bits FIS end Unkonwn FIS-
typedef struct {
	uint32_t UserDefined;		//bits[31:0]
}UnknownDWx;

typedef struct {
	UnknownDWx DW[16];			// 16 Words (Max 64 Bytes allowed)
}UnknownFis;

//-----------Unkonw FIS end----------------------
//-----------Receive Register FIS Structure------
typedef struct {
	DMASetupFis DSFIS;
	uint32_t Rsv1;
	PIOSetupFis PSFIS;
	uint32_t Rsv2[3];
	D2HRegFis RFIS;
	uint32_t Rsv3;
	SetDevBitsFis SDBFIS;
	UnknownFis UFIS;
}ReceiveFis;


typedef struct {
	uint8_t cfisType;
	uint8_t cfisByte1;
	uint8_t cfisCmd;
	uint8_t cfisFeature;
	uint8_t cfisDw1SecNumLbaLow;
	uint8_t cfisDw1CylLowLbaMid;
	uint8_t cfisDw1CylHighLbahigh;
	uint8_t cfisDw1Dev;
	uint8_t cfisDw2SecNumLbaLowExp;
	uint8_t cfisDw2CylLowLbaMidExp;
	uint8_t cfisDw2CylHighLbahighExp;
	uint8_t cfisDw2FeatureExp;
	uint8_t cfisDw3SecCnt;
	uint8_t cfisDw3SecCntExp;
	uint8_t cfisDw3Ctrl;
}cmdFis;

typedef struct {
	uint8_t dsfisType;
	uint8_t dsfisByte1;
	uint32_t dsfisDw1DmaBuffLow;
	uint32_t dsfisDw2DmaBuffHigh;
	uint32_t dsfisDw4DmaBuffOffset;
	uint32_t dsfisDw5DmaXferCnt;
}dsFis;


typedef struct {
	uint8_t psfisType;
	uint8_t psfisByte1;
	uint8_t psfisStatus;
	uint8_t psfisError;
	uint8_t psfisDw1SecNumLbaLow;
	uint8_t psfisDw1CylLowLbaMid;
	uint8_t psfisDw1CylHighLbahigh;
	uint8_t psfisDw1Dev;
	uint8_t psfisDw3SecCnt;
	uint8_t psfisDw3Estatus;
	uint16_t psfisDw4XferCnt;
}piosFis;

typedef struct {
	uint8_t regfisType;
	uint8_t regfisByte1;
	uint8_t regfisStatus;
	uint8_t regfisError;
	uint8_t regfisDw1SecNumLbaLow;
	uint8_t regfisDw1CylLowLbaMid;
	uint8_t regfisDw1CylHighLbahigh;
	uint8_t regfisDw1Dev;
	uint8_t regfisDw3SecCnt;
}regFis;

typedef struct {
	uint8_t sdbfisType;
	uint8_t sdbfisByte1;
	uint8_t sdbfisStatus;
	uint8_t sdbfisError;
}sdbFis;

typedef struct {
	uint32_t ufisWord[16];
}uFis;

typedef struct {
	uint32_t capSMPS:1;
	uint32_t capSSS:1;
	uint32_t piPi:2;
	uint32_t p0cmdCpd:1;
	uint32_t p0cmdEsp:1;
	uint32_t p0cmdMpsp:1;
	uint32_t p0cmdHpcp:1;
	uint32_t rsv:24;
}FirmwareCtrlFeatures;



//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
#endif
