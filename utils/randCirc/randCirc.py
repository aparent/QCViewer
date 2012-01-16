import random

def randCirc(qubits,gates):
  s = ""
  r = ""
  gateset = ["T","H","X","Y","Z","RZ(0.25)","RZ(0.5)"]
  for i in range(qubits):
    s += " "+str(i)
  r += ".v"+s+"\n"
  r += ".i"+s+"\n"
  r += ".o"+s+"\n"
  r += "BEGIN\n"
  for i in range(gates):
    r += random.choice(gateset) 
    loc = random.randint(0,qubits-1)
    r+=" " + str(loc)
    locs=[];
    for j in range(int(random.triangular(0, 6, 0))):
      loc+= random.randint(1,3)
      if loc > 0 and loc < qubits-1:
        locs.append(loc)
    random.shuffle(locs)
    for j in locs:
      r+=" "+str(j)
    r+="\n"
  r += "END\n"
  f = open('circ.qc', 'w') 
  f.write(r)
  print r
