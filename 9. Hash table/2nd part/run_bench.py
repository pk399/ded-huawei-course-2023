import subprocess
from pathlib import Path
from process_words import get_words
from random import shuffle
import sys
import numpy as np

CC = 'g++'
CFLAGS = ['-I../../common/include', '-g']
EXN = 'tst'

RUN_TESTS = 100
WORDS_SIZE = 13333
words = get_words()
shuffle(words)
words = words[:WORDS_SIZE]

CONTESTANTS = {'baseline0': (('-O0',), ('test-ht.c', 'strlist.c', 'hash.c')),
               'baseline1': (('-O3',), ('test-ht.c', 'strlist1.c', 'hash1.c')),
               'opt1': (('-O3', '-mavx'), ('test-ht2.c', 'strlist2.c', 'hash2.c')),
               'opt2': (('-O3', '-mavx'), ('test-ht3.c', 'strlist2.c', 'hash3.c', 'hf.o')),
               'opt2.1': (('-O3', '-mavx'), ('test-ht4.c', 'strlist3.c', 'hash4.c', 'hf.o')),
               'opt3': (('-O3', '-mavx'), ('test-ht4.c', 'strlist3.c', 'hash5.c', 'hf.o'))}
REF = 'baseline1'

results = {}

for tn, test in CONTESTANTS.items():
    acf, infiles = test
    res = subprocess.run([CC, *CFLAGS, *acf, *infiles, '-o', EXN], capture_output=True)
    if (res.returncode != 0):
        print(res.stderr.decode())
        print(res.stdout.decode())
        sys.exit(1)

    if False and tn == 'opt3':
        subprocess.run(['valgrind', '--tool=callgrind', '--branch-sim=yes', '--cache-sim=yes',
                        '--dump-instr=yes', '--collect-jumps=yes',
                        f'./{EXN}', str(len(words)), *[x + '\n' for x in words]])
        sys.exit()
 

    times = []
    for _ in range(RUN_TESTS):
        res = subprocess.run([f'./{EXN}', str(len(words)), *[x + '\n' for x in words]], capture_output=True)
        time = int(res.stdout.decode().strip())
        times.append(time)
    
    times = np.array(times)

    results[tn] = (times.mean(), Path(EXN).stat().st_size)

#Path(EXN).unlink()

def score(res):
    return res[0]#/res[1]

ref_score = score(results[REF])

print("test_name: score improvement")
for tn, res in results.items():
    print(f'{tn}: {score(res):.1f} {(ref_score - score(res))/ref_score:+.2%}')
