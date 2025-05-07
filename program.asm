START:      MVI A, 32H
            MVI B, 15H
LOOP:       ADD B
            DCR B
            JNZ LOOP
            HLT
