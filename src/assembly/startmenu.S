.data
    dino: .string "Dino Game"
    play: .string "Play"
    settings: .string "Settings"

.section .text
.globl startmenu
.type startmenu, @function
startmenu:
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

    #title
    la s1, dino
    li a0, 38 #x
    li a1, 15 #y
    mv a2, s1
    li a3, 0x3700FF  
    jal LCD_ShowString

    #play
    la s1, play
    li a0, 33 #x
    li a1, 40 #y
    mv a2, s1
    li a3, 0x3700FF  
    jal LCD_ShowString

    #settings
    la s1, settings
    li a0, 98 #x
    li a1, 40 #y
    mv a2, s1
    li a3, 0x3700FF  
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