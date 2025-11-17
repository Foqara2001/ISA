    .word 0x100 0x6060   # point A (top-left)
    .word 0x101 0xE014   # point B (unused)
    .word 0x102 0xA0A0   # point C (bottom-right)
    .word 0x103 0x14E0   # point D (unused) 


rectangle:
    lw $t0, $zero, $imm, 0x100       # load A
    and $a0, $imm, $t0, 0xff         # x1 of A  
    srl $a1, $t0, $imm, 8            # y1 of A
    lw $t0, $zero, $imm, 0x102       # load C
    and $a2, $imm, $t0, 0xff         # x2 of C
    srl $a3, $t0, $imm, 8            # y2 of C
    add $t1, $imm, $zero, 0          # pixel = 0

drawpixel:
    out $zero, $imm, $zero, 21       # black 0
    out $t1, $imm, $zero, 20         # address
    srl $t2, $t1, $imm, 8            # current y
    and $s0, $imm, $t1, 0xff         # current x
    bgt $imm, $a1, $t2, next         # y < y1 go to next
    blt $imm, $a3, $t2, next         # y > y2 go to next
    bgt $imm, $a0, $s0, next         # x < x1 go to next
    blt $imm, $a2, $s0, next         # x > x2 go to next
    add $s1, $imm, $zero, 0xff       # white 255
    out $s1, $imm, $zero, 21         # set white

next:
    add $s1, $imm, $zero, 1
    out $s1, $imm, $zero, 22         # draw
    add $t1, $imm, $t1, 1            # pixel++
    add $s2, $imm, $zero, 65536     # end (256*256 pixels)
    bgt $imm, $s2, $t1, drawpixel    # if we dont reach 65536 pixels go to draw pixel
 
halt:
    halt $zero, $zero, $zero, 0

 
