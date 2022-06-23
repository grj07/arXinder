import requests
import xml.etree.ElementTree as ET
import feedparser

def auth_string(auth_list):
    """Turns author list into a nice string."""
    if len(auth_list)==1:
        app = auth_list[0]
    else:
        for ii in range(len(auth_list)-2):
            author_list[ii] = author_list[ii] + ", " 
        author_list[-1] = " and "+author_list[-1]
        app = "".join(auth_list)
    return app

def parse_abstract(entry):
    """
    This function handles the abstract parsing.

    You can extend it as much as you want without making the main chunk of the code
    harder to read.
    """
    abstract = entry.description
    abstract = abstract.lstrip("<p>")\
                       .rstrip("</p>")\
                       .rstrip()\
                       .replace("\n"," ")
    return abstract
    

#Getting the latest hep-th papers from the arxiv
base_url = 'http://export.arxiv.org/rss/'
subject = 'hep-th'
new_url = base_url+subject
#r = requests.get(new_url)
#with open(subject+'_new','wb') as f:
#    f.write(r.content)
new_feed = feedparser.parse(new_url)
#new_feed = feedparser.parse('hep-th_new')

entries = []
for entry in new_feed.entries:
    link = entry.link
    arxiv_number = link.lstrip('http://arxiv.org/abs/')
#    print("arXiv number: ", arxiv_number)
    title_sp = entry.title.split(".")
    entry_title = title_sp[0]
#    print("Title: ",entry_title)
    abstract = parse_abstract(entry)
#    print("Abstract: ", abstract)
    authors = entry.author
    authors = authors.split(",")
    authors = "<roo>" + "".join(authors) + "</roo>"
    authortree = ET.fromstring(str(authors))
    author_list = []
    for au in authortree.iter('a'):
        author_list.append(au.text)
    auths = auth_string(author_list)
#    print("Authors: ",auths.rstrip())
#    print("link: ", entry.link.rstrip(),"\n")
    entry = ("\n",arxiv_number,
              entry_title,
               auths,abstract)
    entry_text = "\n".join(entry)
    entries.append(entry_text)
content = "".join(entries)

with open('prcd.dat', 'w+') as file:
    file.write(content)

#foo = open("daily_email.dat","r")
#linelist = foo.readlines()
#numberoflines=len(linelist)
#endl = 0
##Something which strips the new-line symbols from the list entries
#def stripper(line):
#    str = line.rstrip()
#    return str
#
##Defining a function which takes a list of lines and turns it into a dictionary corresponding to the arxiv entry
#
#def entry_maker(lines):
#    abflineno = 0 
#    for ii in range(len(lines)-1):
#        if lines[ii].startswith("Title:"):
#            titlineno = ii
#        elif lines[ii].startswith("Author"):
#            authlineno = ii
#        elif lines[ii].startswith("Categories:"):
#            catlineno = ii
#        elif lines[ii].startswith("Comments:"):
#            comlineno = ii
#        elif lines[ii].startswith("\\"):
#            abflineno = ii+1
#    tit = "".join(map(stripper,lines[titlineno:authlineno]))
#    auths = "".join(map(stripper,lines[authlineno:catlineno]))
#    abstr = "".join(lines[abflineno:])
#    tit = tit.lstrip("Title:")
#    auths = auths.lstrip("Authors:")
#    abstr = abstr.replace("\n"," ")
#    ent = ("\n",lines[0].strip("arXiv:")[0:10],
#            tit.lstrip(),
#            auths.lstrip(),
#            abstr)
#    return ent
#
#file = open('prcd.dat', 'w+')
#
#for coun in range(numberoflines):
#    line = linelist[coun]
#    if line.startswith("arXiv"):
#        for coun2 in range(coun + 1,numberoflines):
#            bline = linelist[coun2]
#            if bline.startswith("----"):
#                endl = coun2
#                break
#        content = "\n".join(entry_maker(linelist[coun:endl]))
#        file.write(content)
#file.close()
