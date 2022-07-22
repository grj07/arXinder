import requests
import xml.etree.ElementTree as ET
import feedparser


def parse_abstract(entry):
    #Returns abstract free of HTML and newline formatting.
    abstract = entry.description.lstrip("<p>")\
                                .rstrip("</p>")\
                                .rstrip()\
                                .replace("\n"," ")
    return abstract
    
def parse_authors(entry):
    #Returns entry authors as list
    authors = entry.author
    authors = authors.split(",")
    authors = "<roo>" + "".join(authors) + "</roo>"
    authortree = ET.fromstring(str(authors))
    author_list = []
    for au in authortree.iter('a'):
        author_list.append(au.text)
    return author_list

def auth_string(auth_list):
    #Turns author list into a nice string.
    if len(auth_list)==1:
        app = auth_list[0]
    else:
        for ii in range(len(auth_list)-2):
            author_list[ii] = author_list[ii] + ", " 
        author_list[-1] = " and "+author_list[-1]
        app = "".join(auth_list)
    return app

#Getting the latest hep-th papers from the arxiv
base_url = 'http://export.arxiv.org/rss/'
subject = 'hep-th'
new_url = base_url+subject
new_feed = feedparser.parse(new_url)

entries = []
#renders entries into format readable by frontend 
for entry in new_feed.entries:
    link = entry.link
    arxiv_number = link.lstrip('http://arxiv.org/abs/')
#    print("arXiv number: ", arxiv_number)
    title_sp = entry.title.split(".")
    entry_title = title_sp[0]
#    print("Title: ",entry_title)
    abstract = parse_abstract(entry)
    author_list = parse_authors(entry)
    auths = auth_string(author_list)
#    print("Author: ",auths)
    entry = ("\n",arxiv_number,
              entry_title,
               auths,abstract)
    entry_text = "\n".join(entry)
    entries.append(entry_text)
content = "".join(entries)

with open('prcd.dat', 'w+') as file:
    file.write(content)

