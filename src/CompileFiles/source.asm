jmp main

slot_mashina:

push r0
push r0
pop r1
pop [r0]
push r1
pop r0


pop r1
push r0
push 1
add
pop r0
push r1
pop [r0]
pop r0

push 8

push r0
pop r1
push [r1]
pop r0
ret

push r0
pop r1
push [r1]
pop r0
ret


main:

push r0
pop [0]
push r0
pop r1
push r1
pop r0

push 10
pop [1000]
push 7
call slot_mashina
pop [1001]
push [0]
pop r0

hlt
