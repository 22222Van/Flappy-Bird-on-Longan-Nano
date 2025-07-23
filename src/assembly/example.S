.section .text
.globl draw
.type draw, @function
draw:
#	li a0,120
#	li a1,15
#	li a2,8
#	li a3,65535

# 	li a0,60
# 	li a1,20
# 	li a2,
# 	li a3,45
# 	addi sp,sp,-4
# 	sw ra,0(sp)
# 	jal LCD_DrawCircle
#	jal LCD_DrawRectangle
# 	lw ra,0(sp)
# 	addi sp,sp,4

# copy string ptrs
mv a6,a0

    li a0,00000
    addi sp,sp,-4
	sw ra,0(sp)
	jal LCD_Clear
    lw ra,0(sp)
	addi sp,sp,4

loop_:
    # ShowString
    li a0,0
    li a1,20
    mv a2,a6
    li a3,63519 # MAGENTA
    addi sp,sp,-4
	sw ra,0(sp)
	jal LCD_ShowString
    lw ra,0(sp)
	addi sp,sp,4

    li a0,1
    li a1,19
    li a2,79
    li a3,126
    li a4,42577
    addi sp,sp,-4
	sw ra,0(sp)
	jal LCD_DrawRectangle
    lw ra,0(sp)
	addi sp,sp,4

    # button check
    li a0,5
	addi sp,sp,-4
	sw ra,0(sp)
	jal Get_Button
    lw ra,0(sp)
	addi sp,sp,4
    bnez a0, otto
    li a0,6
	addi sp,sp,-4
	sw ra,0(sp)
	jal Get_Button
    lw ra,0(sp)
	addi sp,sp,4
    bnez a0, otto
    beqz a0,loop_
otto:






