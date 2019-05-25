this appears to be the same firmware shipped with product. See SWRU296b 
- note that this is in IAR, ewww.... CCS port in progress!
- TrxEB RF PER Test Software Example User’s Guide for reference.

Use 'SmartRF Flash Programmer 1' w/file:
	D:\Documents\Ref\Platforms\TI\Dev Tools\TRXEB\Examples\PER TEST Software Example\swrc219c\ide\iar\Debug\Exe\per_test.hex
	
	*connect MSP-FETUIF, S2:Enable, S1:UART to detect
	
	*then press 'Refresh', and 'MSP430F5438A will appear as 'AVAILABLE' over a COM port (e.g. 'COM19')
	
	*'Erase, program and verify', then 'Perform actions'

	*Rx Sniff Test->868 (start Slave first)
		Press 'Right' on Tx board to send packets. "Rcv'd Pkt nr" increments on Slave(Rx) for each Master(Tx) press of "Right"
	
-jmr 6.10.17

