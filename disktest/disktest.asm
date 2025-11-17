disk_read_loop:
    add $s0, $zero, $imm, 0x3E8       # $s0 = disk buffer at 0x3E8 (1000)
    add $s1, $zero, $imm, 0           # $s1 = start with sector 0

read_next_sector:
    add $t2, $zero, $imm, 1          # $t2 = read command (1)
    out $s1, $zero, $imm, 15         # Set sector number (reg 15)
    out $s0, $zero, $imm, 16         # Set buffer address (reg 16)
    out $t2, $zero, $imm, 14         # Execute read (reg 14)

wait_for_read:
    in $t0, $zero, $imm, 17               # Check disk status (reg 17)
    bne $imm, $zero, $t0, wait_for_read   # Wait while busy
    add $s2, $zero, $zero, 0                # Reset word index
    add $s1, $s1, $imm, 1                 # Increment sector number
    jal $ra, $imm, $zero, accumulate_data # Process sector data
    add $t0, $imm, $zero, 4               # We want 4 sectors (0-3)
    bne $imm, $t0, $s1, read_next_sector  # Continue if more sectors

write_final_result:
    in $t0, $zero, $imm, 17                  # Check disk status
    bne $imm, $zero, $t0, write_final_result # Wait while busy

set_disk:
    add $s0, $zero, $imm, 0x468          # Source at 0x468 (1128)
    add $s1, $zero, $imm, 4              # Target sector 4
    add $t2, $zero, $imm, 2              # $t2 = write command (2)
    out $s1, $zero, $imm, 15             # Set sector number
    out $s0, $zero, $imm, 16             # Set buffer address
    out $t2, $zero, $imm, 14             # Execute write
    jal $ra, $imm, $zero, cleanup_memory # Clear memory buffers

accumulate_data:
    lw $t0, $imm, $s2, 0x468            # Load current sum (0x468)
    lw $t1, $imm, $s2, 0x3E8            # Load sector data (0x3E8)
    add $t1, $t1, $t0, 0                  # Accumulate sum
    sw $t1, $imm, $s2, 0x468            # Store updated sum
    add $s2, $s2, $imm, 1               # Next word
    add $t0, $s0, $imm, 0x80            # Sector size (128 words)
    bne $imm, $s2, $s0, accumulate_data # Continue until sector done
    out $ra, $zero, $imm, 7             # Original signal 
    reti $ra, $zero, $zero, 0              # Return

halt_program:
    in $t0, $zero, $imm, 17          # Check disk status
    bne $imm, $zero, $t0, halt_program # Wait while busy
    halt $zero, $zero, $zero, 0           # halt instruction

cleanup_memory:
    add $s0, $zero, $imm, 0x3E8       # Start at disk buffer

clear_memory_loop:
    sw $zero, $imm, $s0, 0            # Clear current word
    add $s0, $s0, $imm, 1               # Next address
    add $t0, $zero, $imm, 0x850           # End address (0x850 = 2128)
    bne $imm, $s0, $t0, clear_memory_loop # Continue until clean
    jal $ra, $imm, $zero, halt_program    # Jump to halt_program