.v 1 2
.i 1 2
.o 1 2
.ol 1 2
.c

BEGIN ControlledH (c, t)

P* t
H t
T* t
tof c t
T t
H t
P t

END ControlledH

BEGIN ControlledP (c, t)

t2 c t
T* c
T* t
t2 c t
P c
T t

END ControlledP

BEGIN ControlledY (c, t)

p* t
not c t
p t

END ControlledY

BEGIN

ControlledH 1 2
ControlledP 1 2
ControlledY 1 2

END
