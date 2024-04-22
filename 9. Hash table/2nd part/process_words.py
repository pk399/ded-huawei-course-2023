import lzma

def get_words():
    with lzma.open('../War and Peace.txt.xz', 'rt') as f:
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
    return sorted(words)

if __name__ == '__main__':
    words = get_words()

    with open('words.txt', 'wt') as o:
        o.write('\n'.join(words))

    print(f'Word count: {len(words)}')
