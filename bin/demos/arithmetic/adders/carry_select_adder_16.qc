# A 16 bit reversible carry-select adder with made from 4 4x4 Ripple adders from  <http://arxiv.org/abs/quant-ph/0410184> 
.v c0a0 b0a0 a0a0 b1a0 a1a0 b2a0 a2a0 b3a0 a3a0 za0 cc0a0 cb0a0 ca0a0 cb1a0 ca1a0 cb2a0 ca2a0 cb3a0 ca3a0 cza0 c0a1 b0a1 a0a1 b1a1 a1a1 b2a1 a2a1 b3a1 a3a1 za1 cc0a1 cb0a1 ca0a1 cb1a1 ca1a1 cb2a1 ca2a1 cb3a1 ca3a1 cza1 c0a2 b0a2 a0a2 b1a2 a1a2 b2a2 a2a2 b3a2 a3a2 za2 cc0a2 cb0a2 ca0a2 cb1a2 ca1a2 cb2a2 ca2a2 cb3a2 ca3a2 cza2 c0a3 b0a3 a0a3 b1a3 a1a3 b2a3 a2a3 b3a3 a3a3 za3 cc0a3 cb0a3 ca0a3 cb1a3 ca1a3 cb2a3 ca2a3 cb3a3 ca3a3 cza3 
.i c0a0 b0a0 a0a0 b1a0 a1a0 b2a0 a2a0 b3a0 a3a0 za0 cc0a0 cb0a0 ca0a0 cb1a0 ca1a0 cb2a0 ca2a0 cb3a0 ca3a0 cza0 c0a1 b0a1 a0a1 b1a1 a1a1 b2a1 a2a1 b3a1 a3a1 za1 cc0a1 cb0a1 ca0a1 cb1a1 ca1a1 cb2a1 ca2a1 cb3a1 ca3a1 cza1 c0a2 b0a2 a0a2 b1a2 a1a2 b2a2 a2a2 b3a2 a3a2 za2 cc0a2 cb0a2 ca0a2 cb1a2 ca1a2 cb2a2 ca2a2 cb3a2 ca3a2 cza2 c0a3 b0a3 a0a3 b1a3 a1a3 b2a3 a2a3 b3a3 a3a3 za3 cc0a3 cb0a3 ca0a3 cb1a3 ca1a3 cb2a3 ca2a3 cb3a3 ca3a3 cza3 
.o c0a0 b0a0 a0a0 b1a0 a1a0 b2a0 a2a0 b3a0 a3a0 za0 cc0a0 cb0a0 ca0a0 cb1a0 ca1a0 cb2a0 ca2a0 cb3a0 ca3a0 cza0 c0a1 b0a1 a0a1 b1a1 a1a1 b2a1 a2a1 b3a1 a3a1 za1 cc0a1 cb0a1 ca0a1 cb1a1 ca1a1 cb2a1 ca2a1 cb3a1 ca3a1 cza1 c0a2 b0a2 a0a2 b1a2 a1a2 b2a2 a2a2 b3a2 a3a2 za2 cc0a2 cb0a2 ca0a2 cb1a2 ca1a2 cb2a2 ca2a2 cb3a2 ca3a2 cza2 c0a3 b0a3 a0a3 b1a3 a1a3 b2a3 a2a3 b3a3 a3a3 za3 cc0a3 cb0a3 ca0a3 cb1a3 ca1a3 cb2a3 ca2a3 cb3a3 ca3a3 cza3 
.ol c0a0 b0a0 a0a0 b1a0 a1a0 b2a0 a2a0 b3a0 a3a0 za0 cc0a0 cb0a0 ca0a0 cb1a0 ca1a0 cb2a0 ca2a0 cb3a0 ca3a0 cza0 c0a1 b0a1 a0a1 b1a1 a1a1 b2a1 a2a1 b3a1 a3a1 za1 cc0a1 cb0a1 ca0a1 cb1a1 ca1a1 cb2a1 ca2a1 cb3a1 ca3a1 cza1 c0a2 b0a2 a0a2 b1a2 a1a2 b2a2 a2a2 b3a2 a3a2 za2 cc0a2 cb0a2 ca0a2 cb1a2 ca1a2 cb2a2 ca2a2 cb3a2 ca3a2 cza2 c0a3 b0a3 a0a3 b1a3 a1a3 b2a3 a2a3 b3a3 a3a3 za3 cc0a3 cb0a3 ca0a3 cb1a3 ca1a3 cb2a3 ca2a3 cb3a3 ca3a3 cza3 

BEGIN ADD4( c0 b0 a0 b1 a1 b2 a2 b3 a3 z )
MAJ c0 b0 a0 
MAJ a0 b1 a1 
MAJ a1 b2 a2 
MAJ a2 b3 a3 
tof a3 z 
UMA a2 b3 a3 
UMA a1 b2 a2 
UMA a0 b1 a1 
UMA c0 b0 a0 
END ADD4

BEGIN MAJ( a b c )
tof c b 
tof c a 
tof a b c 
END MAJ

BEGIN UMA( a b c )
tof a b c 
tof c a 
tof a c 
END UMA

BEGIN IADD4( c0 b0 a0 b1 a1 b2 a2 b3 a3 z )
UMA c0 b0 a0 
UMA a0 b1 a1 
UMA a1 b2 a2 
UMA a2 b3 a3 
tof a3 z 
MAJ a2 b3 a3 
MAJ a1 b2 a2 
MAJ a0 b1 a1 
MAJ c0 b0 a0 
END IADD4

BEGIN MUX9( c b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 b6 a6 b7 a7 b8 a8 )
swap c a8 b8 
swap c a7 b7 
swap c a6 b6 
swap c a5 b5 
swap c a4 b4 
swap c a3 b3 
swap c a2 b2 
swap c a1 b1 
swap c a0 b0 
END MUX9

BEGIN
tof a3a3 ca3a3 
tof a3a2 ca3a2 
tof a3a1 ca3a1 
tof a3a0 ca3a0 
tof a2a3 ca2a3 
tof a2a2 ca2a2 
tof a2a1 ca2a1 
tof a2a0 ca2a0 
tof a1a3 ca1a3 
tof a1a2 ca1a2 
tof a1a1 ca1a1 
tof a1a0 ca1a0 
tof a0a3 ca0a3 
tof a0a2 ca0a2 
tof a0a1 ca0a1 
tof a0a0 ca0a0 
tof b3a3 cb3a3 
tof b3a2 cb3a2 
tof b3a1 cb3a1 
tof b3a0 cb3a0 
tof b2a3 cb2a3 
tof b2a2 cb2a2 
tof b2a1 cb2a1 
tof b2a0 cb2a0 
tof b1a3 cb1a3 
tof b1a2 cb1a2 
tof b1a1 cb1a1 
tof b1a0 cb1a0 
tof b0a3 cb0a3 
tof b0a2 cb0a2 
tof b0a1 cb0a1 
tof b0a0 cb0a0 
ADD4 c0a3 b0a3 a0a3 b1a3 a1a3 b2a3 a2a3 b3a3 a3a3 za3 
ADD4 c0a2 b0a2 a0a2 b1a2 a1a2 b2a2 a2a2 b3a2 a3a2 za2 
ADD4 c0a1 b0a1 a0a1 b1a1 a1a1 b2a1 a2a1 b3a1 a3a1 za1 
ADD4 c0a0 b0a0 a0a0 b1a0 a1a0 b2a0 a2a0 b3a0 a3a0 za0 
ADD4 cc0a3 cb0a3 ca0a3 cb1a3 ca1a3 cb2a3 ca2a3 cb3a3 ca3a3 cza3 
ADD4 cc0a2 cb0a2 ca0a2 cb1a2 ca1a2 cb2a2 ca2a2 cb3a2 ca3a2 cza2 
ADD4 cc0a1 cb0a1 ca0a1 cb1a1 ca1a1 cb2a1 ca2a1 cb3a1 ca3a1 cza1 
ADD4 cc0a0 cb0a0 ca0a0 cb1a0 ca1a0 cb2a0 ca2a0 cb3a0 ca3a0 cza0 
MUX9 c0a0 b0a0 cb0a0 a0a0 ca0a0 b1a0 cb1a0 a1a0 ca1a0 b2a0 cb2a0 a2a0 ca2a0 b3a0 cb3a0 a3a0 ca3a0 za0 cza0 
tof cza0 c0a1 
MUX9 c0a1 b0a1 cb0a1 a0a1 ca0a1 b1a1 cb1a1 a1a1 ca1a1 b2a1 cb2a1 a2a1 ca2a1 b3a1 cb3a1 a3a1 ca3a1 za1 cza1 
tof cza0 c0a1 
tof cza1 c0a2 
MUX9 c0a2 b0a2 cb0a2 a0a2 ca0a2 b1a2 cb1a2 a1a2 ca1a2 b2a2 cb2a2 a2a2 ca2a2 b3a2 cb3a2 a3a2 ca3a2 za2 cza2 
tof cza1 c0a2 
tof cza2 c0a3 
MUX9 c0a3 b0a3 cb0a3 a0a3 ca0a3 b1a3 cb1a3 a1a3 ca1a3 b2a3 cb2a3 a2a3 ca2a3 b3a3 cb3a3 a3a3 ca3a3 za3 cza3 
tof cza2 c0a3 
IADD4 c0a3 b0a3 a0a3 b1a3 a1a3 b2a3 a2a3 b3a3 a3a3 za3 
IADD4 c0a2 b0a2 a0a2 b1a2 a1a2 b2a2 a2a2 b3a2 a3a2 za2 
IADD4 c0a1 b0a1 a0a1 b1a1 a1a1 b2a1 a2a1 b3a1 a3a1 za1 
IADD4 c0a0 b0a0 a0a0 b1a0 a1a0 b2a0 a2a0 b3a0 a3a0 za0 
END
