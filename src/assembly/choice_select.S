
.data
    eat: .string "Eat" # Only to fill the space
.section .text
.globl choice_select
.type choice_select, @function
# main:
choice_select:
    addi sp, sp, -52
    # Save S-reg
    sw s0, 0(sp) #save reg
    sw s1, 4(sp) #save reg
    sw s2, 8(sp) #save reg
    sw s3, 12(sp) #save reg
    sw s4, 16(sp) #save reg #save reg
    sw s5, 20(sp) #save reg #save reg
    sw s6, 24(sp) #save reg
    sw s7, 28(sp) #save reg #save reg
    sw s8, 32(sp) #save reg
    sw s9, 36(sp) #save reg
    sw s10, 40(sp) #save reg
    sw s11, 44(sp) #save reg
    sw ra, 48(sp) #save reg
    # #######################################
    
    mv s0, a0 # s0 left or right
    mv s1, a1 # s1 set or clear

    #########################
    # li s0,1
    # li s1,1
    #########################
    li s5, 0 # constant
    li s6, 1 # constant
    
    li t1, 60 # y1
    li t3, 60 # y2
    beq s0, s5, choose_l # judge left box or right box
    beq s0, s6, choose_r # judge left box or right box
set_or_clean:
    beq s1, s5, set # judge set or clear
    beq s1, s6, clean # judge set or clear
draw:
    mv a0, t0 # set arg
    mv a1, t1 # set arg
    mv a2, t2 # set arg
    mv a3, t3 # set arg
    mv a4, t4 # set arg
    jal LCD_DrawLine # call func

end:
    # #######################################
    # Load S-reg
    lw s0, 0(sp) # load reg
    lw s1, 4(sp) # load reg
    lw s2, 8(sp) # load reg
    lw s3, 12(sp) # load reg
    lw s4, 16(sp) # load reg # load reg
    lw s5, 20(sp) # load reg
    lw s6, 24(sp) # load reg
    lw s7, 28(sp) # load reg
    lw s8, 32(sp) # load reg
    lw s9, 36(sp) # load reg
    lw s10, 40(sp) # load reg
    lw s11, 44(sp) # load reg
    lw ra, 48(sp) # load reg
    addi sp, sp, 52 
    jr ra


choose_l:
    li t0, 30
    li t2, 65
    j set_or_clean
choose_r:
    li t0, 90
    li t2, 123
    j set_or_clean
set:
    li t4, 0x0000
    j draw
clean:
    li t4, 0xFFFFFF
    j draw
