import csv

output = ""

with open("latex.csv") as f:
    r = csv.reader(f)

    for row in r:
        name = row[0].strip()
        latex = row[1].strip()

        latex = latex.replace('R', 'RRRR')
        latex = latex.replace('L', 'LLLL')

        latex = latex.replace('\\', '\\\\')

        latex = 'SHPRINTF(buf, sz, "' + latex + '");'

        if 'RRRR' in latex: 
            latex = f'LBRACE(OP_{name}); ' + latex + f' RBRACE(OP_{name});'

            latex = latex.split('RRRR')
            latex = latex[0] + f'"); LR(OP_{name}); SHPRINTF(buf, sz, "' + latex[1]
        
        if 'LLLL' in latex:
            latex = latex.split('LLLL')
            latex = latex[0] + f'"); LL(OP_{name}); SHPRINTF(buf, sz, "' + latex[1]
        

        latex = f'case OP_{name}: {latex} break;\n'
        latex = latex.replace('SHPRINTF(buf, sz, "");', '')

        output += latex

with open('latex.include', 'wt') as o:
    o.write(output)