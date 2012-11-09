.v c1 c2 c3 c4 c5 x0 x1 x2 x3 x4 x5 x6 x7 y0 y1 y2 y3 y4 y5 y6 y7 r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16 ya0 ya1 ya2 ya3 ya4 xa0 xa1 xa2 xa3 xa4 z00 z01 z02 z03 z04 z05 z06 z07 z20 z21 z22 z23 z24 z25 z26 z27 
.i x0 x1 x2 x3 x4 x5 x6 x7 y1 y2 y3 y4 y5 y6 y7
.o c1 c2 c3 c4 c5 x0 x1 x2 x3 x4 x5 x6 x7 y0 y1 y2 y3 y4 y5 y6 y7 r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16 ya0 ya1 ya2 ya3 ya4 xa0 xa1 xa2 xa3 xa4 z00 z01 z02 z03 z04 z05 z06 z07 z20 z21 z22 z23 z24 z25 z26 z27 
.ol 0 0 0 0 0 x0 x1 x2 x3 x4 x5 x6 x7 y0 y1 y2 y3 y4 y5 y6 y7 r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

BEGIN ADD4(c b0 a0 b1 a1 b2 a2 b3 a3 z)
tof  a0 b0
tof  a0 c
tof  c b0 a0
tof  a1 b1
tof  a1 a0
tof  a0 b1 a1
tof  a2 b2
tof  a2 a1
tof  a1 b2 a2
tof  a3 b3
tof  a3 a2
tof  a2 b3 a3
tof  a3 z
tof  a2 b3 a3
tof  a3 a2
tof  a2 b3
tof  a1 b2 a2
tof  a2 a1
tof  a1 b2
tof  a0 b1 a1
tof  a1 a0
tof  a0 b1
tof  c b0 a0
tof  a0 c
tof  c b0
END ADD4

BEGIN ADD8(c b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 b6 a6 b7 a7 z)
tof  a0 b0
tof  a0 c
tof  c b0 a0
tof  a1 b1
tof  a1 a0
tof  a0 b1 a1
tof  a2 b2
tof  a2 a1
tof  a1 b2 a2
tof  a3 b3
tof  a3 a2
tof  a2 b3 a3
tof  a4 b4
tof  a4 a3
tof  a3 b4 a4
tof  a5 b5
tof  a5 a4
tof  a4 b5 a5
tof  a6 b6
tof  a6 a5
tof  a5 b6 a6
tof  a7 b7
tof  a7 a6
tof  a6 b7 a7
tof  a7 z
tof  a6 b7 a7
tof  a7 a6
tof  a6 b7
tof  a5 b6 a6
tof  a6 a5
tof  a5 b6
tof  a4 b5 a5
tof  a5 a4
tof  a4 b5
tof  a3 b4 a4
tof  a4 a3
tof  a3 b4
tof  a2 b3 a3
tof  a3 a2
tof  a2 b3
tof  a1 b2 a2
tof  a2 a1
tof  a1 b2
tof  a0 b1 a1
tof  a1 a0
tof  a0 b1
tof  c b0 a0
tof  a0 c
tof  c b0
END ADD8

BEGIN SUB8(c b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 b6 a6 b7 a7 z)
tof b0 
tof b1
tof b2
tof b3
tof b4 
tof b5
tof b6
tof b7
ADD8 c b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 b6 a6 b7 a7 z
tof b0 
tof b1
tof b2
tof b3
tof a0 
tof a1
tof a2
tof a3
tof b4 
tof b5
tof b6
tof b7
tof a4 
tof a5
tof a6
tof a7
END SUB4

BEGIN ADD4OP(c b0 a0 b1 a1 b2 a2 b3 a3 r0 r1 r2 r3 z)
tof b0 r0
tof b1 r1
tof b2 r2 
tof b3 r3
ADD4 c r0 a0 r1 a1 r2 a2 r3 a3 z
END ADD4OP

BEGIN CADD4(  ctrl c b0 a0 b1 a1 b2 a2 b3 a3 z)
tof  ctrl a0 b0
tof  a0 c
tof  c b0 a0
tof  ctrl a1 b1
tof  a1 a0
tof  a0 b1 a1
tof  ctrl a2 b2
tof  a2 a1
tof  a1 b2 a2
tof  ctrl a3 b3
tof  a3 a2
tof  a2 b3 a3
tof  ctrl a3 z
tof  a2 b3 a3
tof  a3 a2
tof  ctrl a2 b3
tof  a1 b2 a2
tof  a2 a1
tof  ctrl a1 b2
tof  a0 b1 a1
tof  a1 a0
tof  ctrl a0 b1
tof  c b0 a0
tof  a0 c
tof  ctrl c b0
END CADD4

BEGIN CADD5(  ctrl c b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 z)
tof  ctrl a0 b0
tof  a0 c
tof  c b0 a0
tof  ctrl a1 b1
tof  a1 a0
tof  a0 b1 a1
tof  ctrl a2 b2
tof  a2 a1
tof  a1 b2 a2
tof  ctrl a3 b3
tof  a3 a2
tof  a2 b3 a3
tof  ctrl a4 b4
tof  a4 a3
tof  a3 b4 a4
tof  ctrl a4 z
tof  a3 b4 a4
tof  a4 a3
tof  ctrl a3 b4
tof  a2 b3 a3
tof  a3 a2
tof  ctrl a2 b3
tof  a1 b2 a2
tof  a2 a1
tof  ctrl a1 b2
tof  a0 b1 a1
tof  a1 a0
tof  ctrl a0 b1
tof  c b0 a0
tof  a0 c
tof  ctrl c b0
END CADD5

BEGIN MULT4(a0 a1 a2 a3 b0 b1 b2 b3 c r0 r1 r2 r3 r4 r5 r6 r7)
tof  a0 b0 r0
tof  a0 b1 r1
tof  a0 b2 r2
tof  a0 b3 r3
CADD4  a1 c b0 r1 b1 r2 b2 r3 b3 r4 r5
CADD4  a2 c b0 r2 b1 r3 b2 r4 b3 r5 r6
CADD4  a3 c b0 r3 b1 r4 b2 r5 b3 r6 r7
END MULT4

BEGIN MULT5(a0 a1 a2 a3 a4 b0 b1 b2 b3 b4 c r0 r1 r2 r3 r4 r5 r6 r7 r8 r9)
tof  a0 b0 r0
tof  a0 b1 r1
tof  a0 b2 r2
tof  a0 b3 r3
tof  a0 b4 r4
CADD5  a1 c b0 r1 b1 r2 b2 r3 b3 r4 b4 r5 r6
CADD5  a2 c b0 r2 b1 r3 b2 r4 b3 r5 b4 r6 r7
CADD5  a3 c b0 r3 b1 r4 b2 r5 b3 r6 b4 r7 r8
CADD5  a4 c b0 r4 b1 r5 b2 r6 b3 r7 b4 r8 r9
END MULT5

BEGIN KARA8 ( x0 x1 x2 x3 x4 x5 x6 x7 y0 y1 y2 y3 y4 y5 y6 y7 c1 c2 c3 c4 c5 ya0 ya1 ya2 ya3 ya4 xa0 xa1 xa2 xa3 xa4 z00 z01 z02 z03 z04 z05 z06 z07 z20 z21 z22 z23 z24 z25 z26 z27 r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16)
ADD4OP c1 y0 y4 y1 y5 y2 y6 y3 y7 ya0 ya1 ya2 ya3 ya4
ADD4OP c2 x0 x4 x1 x5 x2 x6 x3 x7 xa0 xa1 xa2 xa3 xa4
MULT4 x0 x1 x2 x3 y0 y1 y2 y3 c3 z00 z01 z02 z03 z04 z05 z06 z07
MULT4 x4 x5 x6 x7 y4 y5 y6 y7 c4 z20 z21 z22 z23 z24 z25 z26 z27
MULT5 ya0 ya1 ya2 ya3 ya4 xa0 xa1 xa2 xa3 xa4 c1 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13
ADD4OP c2 x0 x4 x1 x5 x2 x6 x3 x7 xa0 xa1 xa2 xa3 xa4
ADD4OP c1 y0 y4 y1 y5 y2 y6 y3 y7 ya0 ya1 ya2 ya3 ya4
SUB8 c1 r4 z00 r5 z01 r6 z02 r7 z03 r8 z04 r9 z05 r10 z06 r11 z07 r12
SUB8 c1 r4 z20 r5 z21 r6 z22 r7 z23 r8 z24 r9 z25 r10 z26 r11 z27 r12
ADD8 c1 r0 z00 r1 z01 r2 z02 r3 z03 r4 z04 r5 z05 r6 z06 r7 z07 r8
ADD8 c1 r8 z20 r9 z21 r10 z22 r11 z23 r12 z24 r13 z25 r14 z26 r15 z27 r16
MULT4 x4 x5 x6 x7 y4 y5 y6 y7 c4 z20 z21 z22 z23 z24 z25 z26 z27
MULT4 x0 x1 x2 x3 y0 y1 y2 y3 c3 z00 z01 z02 z03 z04 z05 z06 z07
END KARA8

BEGIN
KARA8 x0 x1 x2 x3 x4 x5 x6 x7 y0 y1 y2 y3 y4 y5 y6 y7 c1 c2 c3 c4 c5  ya0 ya1 ya2 ya3 ya4 xa0 xa1 xa2 xa3 xa4 z00 z01 z02 z03 z04 z05 z06 z07 z20 z21 z22 z23 z24 z25 z26 z27 r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16
END
