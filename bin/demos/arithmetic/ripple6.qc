# A 6 bit reversible adder using the design from <http://arxiv.org/abs/quant-ph/0410184> 
.v b0 a0 0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z 
.i b0 a0 0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z 
.o b0 a0 0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z 
.ol s0 a0 0 s1 a1 s2 a2 s3 a3 s4 a4 s5 a5 z 

BEGIN
tof a1  b1 
tof a2  b2 
tof a3  b3 
tof a4  b4 
tof a5  b5 
tof a1 0 
tof a0 b0 
tof 0 
tof a2 a1 
tof 0 b1 a1 
tof a3 a2 
tof a1 b2 a2 
tof a4 a3 
tof a2 b3 a3 
tof a5 a4 
tof a3 b4 a4 
tof a5 z 
tof a4 b5 z 
tof b2 
tof b3 
tof b4 
tof b1 
tof a1 b2 
tof a2 b3 
tof a3 b4 
tof 0 b1 
tof a3 b4 a4 
tof b4 
tof a5 a4 
tof a2 b3 a3 
tof b3 
tof a4 a3 
tof a1 b2 a2 
tof b2 
tof a3 a2 
tof 0 b1 a1 
tof a2 a1 
tof b1 
tof b0 a0 0 
tof a1 0 
tof a0  b0 
tof a1  b1 
tof a2  b2 
tof a3  b3 
tof a4  b4 
tof a5  b5 
END
