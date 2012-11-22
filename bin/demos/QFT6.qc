.v 1 2 3 4 5 6
.i 1 2 3 4 5 6
.o 1 2 3 4 5 6
.ol
.c
BEGIN
H 1
RZ(1 pi/2) 2 1
H 2
RZ(1 pi/4) 3 1
RZ(1 pi/2) 3 2
H 3
RZ(1 pi/8) 4 1
RZ(1 pi/4) 4 2
RZ(1 pi/2) 4 3
H 4
RZ(1 pi/16) 5 1
RZ(1 pi/8) 5 2
RZ(1 pi/4) 5 3
RZ(1 pi/2) 5 4
H 5
RZ(1 pi/32) 6 1
RZ(1 pi/16) 6 2
RZ(1 pi/8) 6 3
RZ(1 pi/4) 6 4
RZ(1 pi/2) 6 5
H 6;
MEASURE 1
MEASURE 2
MEASURE 3
MEASURE 4
MEASURE 5
MEASURE 6
END
