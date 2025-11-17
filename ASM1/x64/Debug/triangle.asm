.word 256 2600   # Vertex A (top-left), B (right-angle), C (bottom-right)
.word 257 28200
.word 258 28400

# Load triangle vertices
lw $s0, $zero, $imm, 2600   # Load the x-coordinate of vertex A into $s0
lw $s1, $zero, $imm, 28200  # Load the x-coordinate of vertex B into $s1
lw $s2, $zero, $imm, 28400  # Load the x-coordinate of vertex C into $s2

# Compute triangle dimensions
sub $t0, $s1, $s0, 0       # $t0 = width of the triangle (BC)
sub $t1, $s2, $s0, 0       # $t1 = height of the triangle (AB)

# Initialize variables for drawing
add $t2, $zero, $zero, 0   # Initialize $t2 (current row)
add $a0, $zero, $imm, 1    # Set color to white (assuming 1 represents white)

# Loop through each row
draw_row:
    ble $t2, $t1, $imm, draw_pixels # If $t2 <= height (AB), continue drawing pixels
    beq $imm, $zero, $zero, end # If $t2 > height (AB), jump to end

draw_pixels:
    # Compute the x-coordinate of the right edge of the triangle for the current row
    mul $t1, $t2, $t0, 0    # $t1 = $t2 * width (BC)
    srl $t1, $t1, $t1, 0    # $t1 = $t1 / height (AB)
    add $t1, $t1, $s0, 0    # $t1 = $t1 + x-coordinate of A (left edge)

    # Draw pixels for the current row
    draw_pixel:
        ble $t2, $t1, $imm, set_pixel # If $t2 <= $t1, continue drawing pixel
        beq $imm, $zero, $zero, next_row # If $t2 > $t1, move to the next row

    set_pixel:
        # Output white pixel at ($t2, $t2)
        out $t2, $t2, $imm, 20 # Assuming output function with x, y coordinates
        add $t2, $t2, $imm, 1  # Move to the next column
        beq $imm, $zero, $zero, draw_pixel # Loop back if $imm == 0

    next_row:
        add $t2, $t2, $imm, 1  # Move to the next row
        beq $imm, $zero, $zero, draw_row # Loop back if $imm == 0

end:
    halt $zero, $zero, $zero, 0 # Stop execution
