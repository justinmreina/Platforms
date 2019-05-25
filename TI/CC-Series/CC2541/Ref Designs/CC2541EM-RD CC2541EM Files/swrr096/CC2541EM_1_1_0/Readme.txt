XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
X   	Artwork and documentation done by: 					X
X									X
X	TEXAS INSTRUMENTS NORWAY LPRF           				X
X  									X
X	Address: Gaustadalléen 21    0349 OSLO                                       	X
X	Phone  : (+47) 22 95 85 44   Fax :  (+47) 22 95 89 05                   	X
X   	web: www.ti.com                         	 				X
X                                                                           	 		X
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

PCB NAME : CC2541EM	
REVISION: 1.1.0
DATE: 2011-08-25

PCB DESCRIPTION:4 LAYER PCB 1.6 MM
      Copper        1   35 um
      Dielectric  1-2   0.35 mm (e.g. 2x Prepreg 7628 AT05 47% Resin)
      Copper        2   18 um
      Dielectric  2-3   0.76 mm (4 x 7628M 43% Resin)
      Copper        3   18 um
      Dielectric  3-4   0.35 mm (e.g. 2x Prepreg 7628 AT05 47% Resin)
      Copper        4   35 um

DE104ML/FR4 or equivalent substrate (Resin contents around 45%, which gives Er=4.42@2.4GHz, TanD=0.016)
  Dimensions in mil (0.001 inch)
  DOUBLE SIDE SOLDER MASK,
  DOUBLE SIDE SILKSCREEN,
  8 MIL MIN TRACE WIDTH AND 6 MIL MIN ISOLATION.

Manufacturers marking: Two letter + year + week
The two letter code shall identify the manufaturer and is decided by the manufacturer. 
No logos or other identifiers are allowed.
The marking shall be in silk screen print at secondary side of the PCB, or in solder 
resist if no silk print at secondary side.
	
ASSEMBLY: 	LABEL CARD WITH "CC2541EM_R1.1.0"

FILE: CC2541EM_1_1_0.ZIP PACKED WITH WinZIP containing 
                 
FILE NAME            				DESCRIPTION                               			FILE TYPE

PCB MANUFACTURING FILES:
L1.SPL               				LAYER 1 COMPONENT SIDE/POSITIVE            	            EXT. GERBER
L2.SPL	              			LAYER 2 POSITIVE               		            EXT. GERBER
L3.SPL               				LAYER 3 POSITIVE            		                         EXT. GERBER
L4.SPL	              			LAYER 4 SOLDER SIDE/POSITIVE               	            EXT. GERBER
STOPCOMP.SPL         			SOLDER MASK COMPONENT SIDE/NEGATIVE               EXT. GERBER
STOPSOLD.SPL         			SOLDER MASK SOLDER SIDE/NEGATIVE                       EXT. GERBER
SILKCOMP.SPL         			SILKSCREEN COMPONENT SIDE/POSITIVE                   EXT. GERBER
SILKSOLD.SPL				SILKSCREEN SOLDER SIDE/POSITIVE		            EXT. GERBER
GERBER.REP				DRILL and NC DRILL REPORT		            ASCII
DRILL.SPL	            			DRILL/MECHANICAL DRAWING                  	            EXT. GERBER
NCDRILL.SPL				NC DRILL THROUGH HOLE                     	            EXCELLON			
EXT_GERBER.USR	     			PCB PHOTOPLOTTER DEFINITION FILE	            ASCII
CNC.USR		     			NC DRILL DEVICE FILE			            ASCII

PCB ASSEMBLY FILES:
CC2541EM_PARTLIST.XLS			PART LIST					ASCII
P_P_COMP.REP			              PICK AND PLACE COORDINATES, COMPONENT SIDE	ASCII
P_P_SOLD.REP			              PICK AND PLACE COORDINATES, SOLDER SIDE	ASCII
PASTCOMP.SPL         			SOLDER PASTE COMPONENT SIDE               		EXT. GERBER
PASTSOLD.SPL				SOLDER PASTE SOLDER SIDE			EXT. GERBER
ASSYCOMP.SPL        			ASSEMBLY DRAWING COMPONENT SIDE           	EXT. GERBER
ASSYSOLD.SPL				ASSEMBLY DRAWING SOLDER SIDE			EXT. GERBER

PDF FILES:
CC2541EM_SCHEMATIC.PDF			CIRCUIT DIAGRAM
CC2541EM_LAYOUT.PDF			LAYOUT DIAGRAM

CADSTAR FILES:
CC2541EM.SCM				CADSTAR SCHEMATIC FILE
CC2541EM.PCB				CADSTAR LAYOUT FILE
CC2541EM.CSA				CADSTAR SCHEMATIC ARCHIVE
CC2541EM.CPA				CADSTAR LAYOUT ARCHIVE

README.TXT           			THIS FILE                                 			ASCII

END.

Release history
--------------------------------------------------------------------------------------------------------------
0.1.0: 	Initial release.
	Based on CC2540EM R1.5.1. 
	New crystal 2x2.5 mm
	I2C pullup resistors added.
1.0.0:     Added TI and WEEE logo.
1.1.0:     Removed WEEE logo.