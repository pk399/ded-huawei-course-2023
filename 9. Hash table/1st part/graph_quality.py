import matplotlib.pyplot as plt
import numpy as np
import os
import subprocess
from pathlib import Path

CC = 'g++'
CFLAGS = ['-O3', '-I../../common/include']
INFILES = ['test-ht.c', 'strlist.c', 'hash.c']
EXN = 'tst'

IMGDIR = Path('pics')
if not IMGDIR.exists():
    IMGDIR.mkdir()

if not Path('words.txt').exists():
    print('Making words.txt')
    import process_words


fns = [1, 2, 3, 4, 5, 6, 7]
FNAME = ['Константа', 'Первый символ', 'Длина', 'Сумма', 'Ротация вправо', 'Ротация влево', 'Моя']
FCOL = ['#bb8fce', '#af7ac5', '#e74c3c', '#d35400', '#8e44ad', '#2980b9', '#2ecc71']

for fn in fns:
    subprocess.run([CC, *CFLAGS, *INFILES, '-o', EXN, f'-DF{fn}'])
    res = subprocess.run([f'./{EXN}'], capture_output=True)
    d = res.stdout.decode().strip().split(' ')
    ar = np.array([int(x) for x in d])

    fig, ax = plt.subplots()

    ax.set_title(f'Функция F{fn}: {FNAME[fn - 1]}')

    ax.bar(range(len(ar)), ar, label=ar.var(), color=FCOL[fn - 1])
    ax.legend(title='Дисперсия')

    fig.savefig(IMGDIR / f'F{fn}-1729.png')
    #plt.show()

Path(EXN).unlink()
