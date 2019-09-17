; list directive to define processor
		list      p=12f629            
		; processor specific variable definitions
		#include <p12f629.inc>        

		__CONFIG   	_CP_OFF & _MCLRE_OFF & _WDT_ON & _INTRC_OSC_NOCLKOUT & _BODEN_OFF & _CPD_OFF &_PWRTE_OFF
	
#DEFINE	PIN0	GPIO,0	
#DEFINE	PIN1	GPIO,1	
#DEFINE	PIN2	GPIO,2	
#DEFINE	PIN3	GPIO,3	
#DEFINE	PIN4	GPIO,4	
#DEFINE	PIN5	GPIO,5	
#DEFINE PIN0_MODE	TRISIO,0 
#DEFINE PIN1_MODE	TRISIO,1 
#DEFINE PIN2_MODE	TRISIO,2 
#DEFINE PIN3_MODE	TRISIO,3 
#DEFINE PIN4_MODE	TRISIO,4 
#DEFINE PIN5_MODE	TRISIO,5 

CNT0	EQU		20H
CNT1	EQU		21H
CNT2	EQU		22H
CNT3	EQU		23H
K1		EQU		24H
K2		EQU		25H
;---------------------------------
		ORG		0000H
			
		MOVLW 	07H	
		MOVWF 	CMCON		; TURN-OFF analog comparator 
		
		BSF		STATUS,RP0	; select bank 1
		CALL	3FFH		; This will load the factory pre-calibrated OSCCAL calibration value into the W register
		MOVWF	OSCCAL		; in W so we can load it into the OSCCAL register
		BCF		PIN0_MODE	; set as output}
		BCF		PIN1_MODE
		BCF		PIN2_MODE
		BCF		PIN3_MODE
		BCF		PIN4_MODE
		BSF		PIN5_MODE
		MOVLW 	b'00001111' ;Prescaler = 000 => 1:128 prescaler,2.3 seg
		MOVWF 	OPTION_REG  ; PSA=1 => WDT will use the prescaler
		BCF		STATUS,RP0	; select bank 0

		CLRF	GPIO
		BTFSS 	STATUS,4 ;Check if the reset is caused by WDT Time out
		BSF 	PIN2 ;WDT Time out has occured, set PORTB. 1 
	
	 
LOOP1	MOVLW 	d'30'
		CLRWDT 
		MOVWF	CNT3 		
LOOP2:	BSF		PIN0;	pin 7 in micro on led and clock out signal
		CALL	DELAY
		BTFSS 	GPIO,5;	pin 2 on micro input
		CALL	RESETUC;RESETEAR PIN 3 GP4 ->1	10ML
		CALL	DELAY
		CLRWDT
		BCF		PIN0;	pin 7 in micro off
		CALL	DELAY
		BTFSC 	GPIO,5;	pin 2 on micro
		CALL	RESETUC;RESETEAR PIN 3 GP4 ->1	 10ML
		CALL	DELAY		
		CLRWDT
		DECFSZ	CNT3, f
		GOTO	LOOP2		
		NOP
		GOTO 	LOOP1
;---------------------------------
; DELAY 1 SECONDS ROUTINE   
;--------------------------------- 
DELAY
			;999990 cycles
	movlw	0x07
	movwf	CNT0
	movlw	0x2F
	movwf	CNT1
	movlw	0x03
	movwf	CNT2
DELAY_0
	decfsz	CNT0, f
	goto	$+2
	decfsz	CNT1, f
	goto	$+2
	decfsz	CNT2, f
	goto	DELAY_0

			;6 cycles
	goto	$+1
	goto	$+1
	goto	$+1

			;4 cycles (including call)
	return
;-------------------------------------
; Actual delay = 0.01 seconds = 10000 cycles
; Error = 0 %,9993 cycles
RESETUC
	BSF 	PIN4	
	movlw	0xCE
	movwf	K1
	movlw	0x08
	movwf	K2
RESETUC_0
	decfsz	K1, f
	goto	$+2
	decfsz	K2, f
	goto	RESETUC_0

			;3 cycles
	goto	$+1
	nop
	BCF 	PIN4
			;4 cycles (including call)
	return		



		END