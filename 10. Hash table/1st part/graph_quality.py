import matplotlib.pyplot as plt
import numpy as np
import os
import subprocess
from pathlib import Path

HT_SIZE = 1733

CC = 'g++'
CFLAGS = ['-O3', '-I../../common/include', '-mavx']
INFILES = ['test-ht.c', 'strlist.c', 'hash.c']
EXN = 'tst'

FIG_DPI = 300

IMGDIR = Path('pics')
if not IMGDIR.exists():
    IMGDIR.mkdir()

if not Path('words.txt').exists():
    print('Making words.txt')
    import process_words


fns = [1, 2, 3, 4, 5, 6, 7, 8]
FNAME = ['Константа', 'Первый символ', 'Длина', 'Сумма', 'Ротация вправо', 'Ротация влево', 'Моя', 'CRC32']
FCOL = ['#bb8fce', '#af7ac5', '#e74c3c', '#d35400', '#8e44ad', '#2980b9', '#2ecc71', '#ff00ff']

def plotnsave(fn, size, title):
    subprocess.run([CC, *CFLAGS, *INFILES, '-o', EXN, f'-DF{fn}', f'-DHT_SIZE={size}'])
    res = subprocess.run([f'./{EXN}'], capture_output=True)
    d = res.stdout.decode().strip().split(' ')
    ar = np.array([int(x) for x in d])

    fig, ax = plt.subplots()

    ax.set_title(f'Функция F{fn}: {title}')

    ax.bar(range(len(ar)), ar, label=ar.var(), color=FCOL[fn - 1])
    ax.set_xlim(0, size)
    ax.legend(title='Дисперсия')

    fig.savefig(IMGDIR / f'F{fn}-{size}.png', dpi=FIG_DPI)
    #plt.show()

for fn in fns:
    plotnsave(fn, HT_SIZE, FNAME[fn - 1])

# Сравнение суммы символов с CRC32
for fn in [4, 8]:
    plotnsave(fn, 179, f'{FNAME[fn - 1]} (при меньшем размере)')

Path(EXN).unlink()
