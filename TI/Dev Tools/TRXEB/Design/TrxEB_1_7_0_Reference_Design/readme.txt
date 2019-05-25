XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
X                                                                               X
X   Artwork and documentation done by: Texas Instruments Norway AS              X
X   Company: Texas Instruments Norway AS                                        X
X   Address: Gaustadalleen 21, 0349 OSLO, Norway                                X
X                                                                               X
X                                                                               X
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

PROJECT....: CC112x
PCB NAME...: TRXEB
REVISION...: 1.7.0
DATE.......: 2011-07-20 (removed WEEE logo 2011-08-24)
QUANTITY...: See order (in panel)

Manufacturers marking: Two letter + year + week
The two letter code shall identify the manufaturer and is decided by the manufacturer. 
No company logos or other identifyers are allowed.
The marking shall be in silk screen print at secondary side of the PCB, or in solder 
resist if no silk print at secondary side.
The PCB shall be RoHS and REACH compliant.


FILE: TRXEB_Reference_Design_1_7_0.ZIP PACKED WITH WinZIP 

PCB DESCRIPTION: 4 LAYER PCB 1.6 MM
      Copper        1   35 um
      Dielectric  1-2   0.35 mm (e.g. 2x Prepreg 7628 AT05 47% Resin)
      Copper        2   18 um
      Dielectric  2-3   0.76 mm (4 x 7628M 43% Resin)
      Copper        3   18 um
      Dielectric  3-4   0.35 mm (e.g. 2x Prepreg 7628 AT05 47% Resin)
      Copper        4   35 um

DE104iML or equivalent substrate (Resin contents around 45%, which gives Er=4.42@2.4GHz, TanD=0.016
               
               
FILE NAME                   DESCRIPTION                             FILE TYPE
-------------------------------------------------------------------------------------------
***PCB MANUFACTURING FILES:
l1.SPL                  LAYER 1 COMPONENT SIDE/POSITIVE             EXT. GERBER
l2.SPL                  LAYER 2 Inner layer/POSITIVE                EXT. GERBER
l3.SPL                  LAYER 3 Inner layer/POSITIVE                EXT. GERBER
l4.SPL                  LAYER 4 SOLDER SIDE/POSITIVE                EXT. GERBER

STOPCOMP.SPL            SOLDER MASK COMPONENT SIDE/NEGATIVE         EXT. GERBER
STOPSOLD.SPL            SOLDER MASK SOLDER SIDE/NEGATIVE            EXT. GERBER

SILKCOMP.SPL            SILKSCREEN COMPONENT SIDE/POSITIVE          EXT. GERBER
SILKSOLD.SPL            SILKSCREEN SOLDER SIDE/POSITIVE             EXT. GERBER

PASTCOMP.SPL            SOLDER PAST COMPONENT SIDE/POSITIVE         EXT. GERBER
PATSOLD.SPL             SOLDER PAST SOLDER SIDE/POSITIVE            EXT. GERBER

ASSYCOMP.SPL            ASSEMBLY DRAWING COMPONENT SIDE             EXT. GERBER
ASSYSOLD.SPL            ASSEMBLY DRAWING SOLDER SIDE                EXT. GERBER

NCDRILL.SPL             NC DRILL THROUGH HOLE                       EXCELLON
DRILL.SPL               DRILL/MECHANICAL DRAWING                    EXT. GERBER

GERBER.REP              DRILL/NC DRILL REPORT                       ASCII

P&PCOMP.REP             PICK AND PLACE FILE COMPONENT SIDE          ASCII
P&PSOLD.REP             PICK AND PLACE FILE SOLDER SIDE             ASCII

EXT_GERBER.USR          EXTENDED GERBER APERTURE TABLE              ASCII
CNC.USR                 NC DRILL DEVICE FILE                        ASCII

TRXEB_1_7_0_PARTLIST.xls     PART LIST                              ASCII

***PDF FILES:
TRXEB_1_7_0_SCHEMATIC.PDF    Circuit Diagram
TRXEB_1_7_0_LAYOUT.PDF       Layout Diagram

***CADSTAR FILES:
TRXEB_1_7_0.SCM              Cadstar Schematic file
TRXEB_1_7_0.CSA              Cadstar Shematic archive
TRXEB_1_7_0.PCB              Cadstar Layout file
TRXEB_1_7_0.CPA              Cadstar PCB archive

README.TXT                   THIS FILE                              ASCII


***REVISION HISTORY
Rev 1.5.0   Changes to silk screen. 
            Changed some components to 0603 size to ease manual soldering.
            Added RC filter to CC2511 reset line.
            Pullup resistors on LCD SPI lines connected to the LCD power source.

Rev 1.6.0   Added switch (TS3A44159) for improved control of combo (CC119x/CC259x) boards
            Improved decoupling of power to accelerometer.
            Improved silk print (marking of ON/OFF switch etc.)
            Moved fiducial marks to avoid exposure of copper.
            Added GND pad.
            P2.0 on CC2511 connected to test pad.
            Connected RF2, pin 20 to GND.
            Connected P5, pin 2 to GND.
            Replaced external power connector with pin header.
            Updated footprint for MSP430 (distance between pins on each side of chip).
            New connector type for the LCD.

Rev 1.7.0   Replaced debug header for CC2511 on bottom side with testpins.
            Fixed solder paste pattern for TPS63000
            Minor update to the silkprint
            Update 2011-08-24: Removed WEEE logo (not applicable)
            



END.          