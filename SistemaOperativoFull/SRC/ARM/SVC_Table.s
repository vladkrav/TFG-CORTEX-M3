;/*----------------------------------------------------------------------------
; *      RL-ARM - RTX
; *----------------------------------------------------------------------------
; *      Name:    SVC_TABLE.S
; *      Purpose: Pre-defined SVC Table for Cortex-M
; *      Rev.:    V4.70
; *----------------------
;------------------------------------------------------
; *
; * Copyright (c) 1999-2009 KEIL, 2009-2013 ARM Germany GmbH
; * All rights reserved.
; * Redistribution and use in source and binary forms, with or without
; * modification, are permitted provided that the following conditions are met:
; *  - Redistributions of source code must retain the above copyright
; *    notice, this list of conditions and the following disclaimer.
; *  - Redistributions in binary form must reproduce the above copyright
; *    notice, this list of conditions and the following disclaimer in the
; *    documentation and/or other materials provided with the distribution.
; *  - Neither the name of ARM  nor the names of its contributors may be used 
; *    to endorse or promote products derived from this software without 
; *    specific prior written permission.
; *
; * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
; * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
; * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
; * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
; * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
; * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
; * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
; * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; * POSSIBILITY OF SUCH DAMAGE.
; *---------------------------------------------------------------------------*/


                AREA    SVC_TABLE, CODE, READONLY

                EXPORT  SVC_Count

SVC_Cnt         EQU    (SVC_End-SVC_Table)/4
SVC_Count       DCD     SVC_Cnt

; Import user SVC functions here.
                IMPORT  __SVC_1
				IMPORT  __SVC_2
				IMPORT  __SVC_3
				IMPORT  __SVC_4
				IMPORT  __SVC_5
				IMPORT  __SVC_6
				IMPORT  __SVC_7
				IMPORT  __SVC_8
				IMPORT  __SVC_9
				IMPORT  __SVC_10
				IMPORT  __SVC_11
				IMPORT  __SVC_12
				IMPORT  __SVC_13
				IMPORT  __SVC_14
				IMPORT  __SVC_15
				IMPORT  __SVC_16
				IMPORT  __SVC_17
				IMPORT  __SVC_18
				IMPORT  __SVC_19
					
                EXPORT  SVC_Table
SVC_Table
; Insert user SVC functions here. SVC 0 used by RTL Kernel.
                DCD     __SVC_1                 ; user SVC function
				DCD     __SVC_2                 ; user SVC function
				DCD     __SVC_3                 ; user SVC function
				DCD     __SVC_4                 ; user SVC function
				DCD     __SVC_5                 ; user SVC function
				DCD     __SVC_6                 ; user SVC function
				DCD     __SVC_7                 ; user SVC function
				DCD     __SVC_8                 ; user SVC function
				DCD     __SVC_9                 ; user SVC function
				DCD     __SVC_10                ; user SVC function
				DCD     __SVC_11                ; user SVC function
				DCD     __SVC_12                ; user SVC function	
				DCD     __SVC_13                ; user SVC function
				DCD     __SVC_14                ; user SVC function	
				DCD     __SVC_15                ; user SVC function	
				DCD     __SVC_16                ; user SVC function
				DCD     __SVC_17                ; user SVC function
				DCD     __SVC_18                ; user SVC function
				DCD     __SVC_19                ; user SVC function
SVC_End

                END

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
