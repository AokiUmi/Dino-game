.data
    hard: .string "Hard"
    easy: .string "Easy"
    settings: .string "Settings"

.section .text
.globl settingsmenu
.type settingsmenu, @function
settingsmenu:
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

    
    li a0, 2        
    li a1, 2        
    li a2, 157      
    li a3, 77       
    li a4, 0xFFFE   
    jal LCD_DrawRectangle

    li a0, 6        
    li a1, 6        
    li a2, 153      
    li a3, 73       
    li a4, 0xF800  
    jal LCD_DrawRectangle
    #title
    la s1, settings
    li a0, 47 #x
    li a1, 15 #y
    mv a2, s1
    li a3, 0x3333FF
    jal LCD_ShowString

    #easy
    la s1, easy
    li a0, 30 #x
    li a1, 40 #y
    mv a2, s1
    li a3, 0x4F0D5C 
    jal LCD_ShowString

    #hard
    la s1, hard
    li a0, 90 #x
    li a1, 40 #y
    mv a2, s1
    li a3, 0xFFFE  
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
