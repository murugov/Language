        in
        pop r0
        in
        pop r1
        in
        pop r2

        call discr

        push 0
        push r0
        je a_is_zero

        push 0
        push r3
        jb no_roots

        push 0
        push r3
        je one_root

        jmp two_roots

discr:
        push r1
        push r1
        mul

        push 4
        push r0
        mul
        push r2
        mul

        sub

        pop r3

        ret

a_is_zero:
        push 0
        push r1
        je b_is_zero

        push 1
        out 

        push 0
        push r2
        sub
        push r1
        div


        out
        hlt

b_is_zero:
        push 0
        push r2
        je inf_roots

        jmp no_roots

inf_roots:
        push -1
        out
        hlt

no_roots:
        push 0
        out
        hlt

one_root:
        push 1
        out

        push 0
        push r1
        sub
        push 2
        push r0
        mul
        div

        out         ; print root_1
        hlt


two_roots:
        push 2
        out

        push 0
        push r1
        sub
        push r3
        sqrt
        add
        push 2
        push r0
        mul
        div

        out         ; print root_1

        push 0
        push r1
        sub
        push r3
        sqrt
        sub
        push 2
        push r0
        mul
        div

        out         ; print root_1
        hlt