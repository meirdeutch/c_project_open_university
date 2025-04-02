;This is a valid file
mcro my_mcro
    add #50, x
mcroend    
mov #-1, r2
my_mcro
.entry x
.extern y
x: .data 23
my_mcro
dec x 
dec y
clr r1 
my_mcro
mov x, r1
cmp x, r1
my_mcro
sub  #3, r1