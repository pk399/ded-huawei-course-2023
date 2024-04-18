import lzma

with lzma.open('War and Peace.txt.xz', 'rt') as f:
    d = f.read()

t = d[7450:-18410] # Remove not war and peace
t = t.lower()

words = []
w = ''
for c in t:
    if c not in 'qwertyuiopasdfghjklzxcvbnm':
        words.append(w)
        w = ''
    else:
        w += c

words = [x for x in set(words) if len(x) > 1]

with open('words.txt', 'wt') as o:
    o.write('\n'.join(sorted(words)))


print(f'Word count: {len(words)}')
