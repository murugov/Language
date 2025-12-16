        in          ; enter number
        pop r0

        push r0
        push 0
        jne no_zero

        push 1
        out
        hlt

        no_zero:
                push 1
                pop r1

                call fact
                
                push r1
                out
                hlt


fact:
        push 1
        push r0
        je end

        push r0
        push r1
        mul
        pop r1
        
        push r0
        push 1
        sub
        pop r0

        call fact

        end:
                ret