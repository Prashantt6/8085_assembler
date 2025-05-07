START:          ; Label for the start
MVI A, 00H      ; Initialize A to 00H
LOOP1:          ; Label for first loop
    INR A       ; Increment A
    JNZ LOOP1   ; Jump back to LOOP1 if A is not zero
LOOP2:          ; Label for second loop
    DCR A       ; Decrement A
    JNZ LOOP2   ; Jump back to LOOP2 if A is not zero
HLT              ; Halt the program
