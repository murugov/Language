jmp main

sum:

push r0
push r0
pop r1
pop [r0]
push r1
pop r0


pop r1
push r0
push 2
add
pop r0
push r1
pop [r0]
pop r0


pop r1
push r0
push 3
add
pop r0
push r1
pop [r0]
pop r0


push r0
push 2
add
pop r1
push [r1]


push r0
push 3
add
pop r1
push [r1]

add

push r0
push -3
add
pop r1
pop [r1]


push r0
push -3
add
pop r1
push [r1]


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

push 1
push 2
call sum
pop [1000]
push [0]
pop r0

hlt
