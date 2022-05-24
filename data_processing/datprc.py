#This script is meant to parse the arxiv daily email and turn each new paper into a
#format easily read by user interface program
foo = open("daily_email.dat","r")
linelist = foo.readlines()
numberoflines=len(linelist)
endl = 0
#Something which strips the new-line symbols from the list entries
def stripper(line):
    str = line.rstrip()
    return str

#Defining a function which takes a list of lines and turns it into a dictionary corresponding to the arxiv entry

def entry_maker(lines):
    abflineno = 0 
    for ii in range(len(lines)-1):
        if lines[ii].startswith("Title:"):
            titlineno = ii
        elif lines[ii].startswith("Author"):
            authlineno = ii
        elif lines[ii].startswith("Categories:"):
            catlineno = ii
        elif lines[ii].startswith("Comments:"):
            comlineno = ii
        elif lines[ii].startswith("\\"):
            abflineno = ii+1
    tit = "".join(map(stripper,lines[titlineno:authlineno]))
    auths = "".join(map(stripper,lines[authlineno:catlineno]))
    abstr = "".join(lines[abflineno:])
    tit = tit.lstrip("Title:")
    auths = auths.lstrip("Authors:")
    abstr = abstr.replace("\n"," ")
    ent = ("\n",lines[0].strip("arXiv:")[0:10],
            tit.lstrip(),
            auths.lstrip(),
            abstr)
    return ent

file = open('prcd.dat', 'w+')

for coun in range(numberoflines):
    line = linelist[coun]
    if line.startswith("arXiv"):
        for coun2 in range(coun + 1,numberoflines):
            bline = linelist[coun2]
            if bline.startswith("----"):
                endl = coun2
                break
        content = "\n".join(entry_maker(linelist[coun:endl]))
        file.write(content)
file.close()
