.data
    retry: .string "Retry"
    quit: .string "Quit"
    score: .string "Score: "

.section .text
.globl endmenu
.type endmenu, @function
endmenu:
    addi sp, sp, -52
    # Save S-reg
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
    sw s4, 16(sp)
    sw s5, 20(sp)
    sw s6, 24(sp)
    sw s7, 28(sp)
    sw s8, 32(sp)
    sw s9, 36(sp)
    sw s10, 40(sp)
    sw s11, 44(sp)
    sw ra, 48(sp)
    ########################################
    #set blackgound
    li a0, 0
    jal LCD_Clear

    mv s0, a0 # store your score number
    mv s1, a1 # the bit of num


    li a0, 2        
    li a1, 2        
    li a2, 157      
    li a3, 77       
    li a4, 0x07FF 
    jal LCD_DrawRectangle

    li a0, 6        
    li a1, 6        
    li a2, 153      
    li a3, 73       
    li a4, 0x07FF   
    jal LCD_DrawRectangle
    #title score
    la s1, score
    li a0, 27 #x
    li a1, 15 #y
    mv a2, s1
    li a3, 0x3333FF
    jal LCD_ShowString

    #number of score
    li a0, 38 #x
    li a1, 15 #y
    mv a2, s0 # the exact num
    mv a3, s1 # the bit of num
    li a4, 0x3333FF
    jal LCD_ShowNum

    #retry
    la s1, retry
    li a0, 30 #x
    li a1, 40 #y
    mv a2, s1
    li a3, 0x00FFFF
    jal LCD_ShowString

    #quit
    la s1, quit
    li a0, 90 #x
    li a1, 40 #y
    mv a2, s1
    li a3, 0x00FFFF
    jal LCD_ShowString

    # #######################################
    # Load S-reg
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    lw s4, 16(sp)
    lw s5, 20(sp)
    lw s6, 24(sp)
    lw s7, 28(sp)
    lw s8, 32(sp)
    lw s9, 36(sp)
    lw s10, 40(sp)
    lw s11, 44(sp)
    lw ra, 48(sp)
    addi sp, sp, 52
    jr ra