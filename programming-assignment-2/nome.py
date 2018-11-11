from subprocess import PIPE
from subprocess import run
from math import pow
boba = []
bobo = []
bob2 = []
bob3 = []
bob4 = []
bob5 = []
for i in range (30) :
    run(["/usr/bin/time", "-v", "-o", "bob.txt",  "./ep2", "10", "50", "10"])

    olar = run(["grep", "Maximum resident set size", "bob.txt"], stdout = PIPE).stdout.decode('utf-8')
    bob = []
    for i in olar:
        if (i in ['0','1','2','3','4','5','6','7','8','9']):
            bob.append(i)
    olar = 0
    for i in range (len(bob)):
        olar += int(bob[i])*(pow(10, len(bob) - i - 1))
    boba.append(olar)

    olar = run(["grep", "Elapsed (wall clock) time", "bob.txt"], stdout = PIPE).stdout.decode('utf-8')
    bob = []
    for i in olar:
        if (i in ['0','1','2','3','4','5','6','7','8','9', '.', ":"]):
            bob.append(i)
    bob2 = bob[4:]
    j = 0
    for i in bob2:
        if (i in [":"]):
            j += 1
            continue
    bob3 = bob2[:j]
    mins = 0
    for i in range (len(bob3)):
        mins += int(bob3[i])*(pow(10, len(bob3) - i - 1))
    bob4 = bob2[j + 1:]
    for i in bob4:
        if (i in ["."]):
            j += 1
            continue
    bob5 = bob4[:j]
    segs = 0
    for i in range (len(bob5)):
        segs += int(bob5[i])*(pow(10, len(bob5) - i - 1))
    bob5 = bob4[j +1:]
    msegs = 0
    for i in range (len(bob5)):
        msegs += int(bob5[i])*(pow(10, len(bob5) - i - 1))
    total = mins*60 + segs + msegs/100
    bobo.append(total)
grandeBob = []
for i in range(len(boba)):
    grandeBob.append([boba[i], bobo[i]])
run(["rm","bob.txt"])
print(grandeBob)
