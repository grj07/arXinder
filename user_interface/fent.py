import time
import requests
import xml.etree.ElementTree as ET
import feedparser

#Returns abstract free of HTML and newline formatting.
def parse_abstract(entry):
    abstract = entry.description.lstrip("<p>")\
                                .rstrip("</p>")\
                                .rstrip()\
                                .replace("\n"," ")
    return abstract
    
#Returns entry authors as list
def parse_authors(entry):
    authors = entry.author
    authors = authors.split(",")
    authors = "<roo>" + "".join(authors) + "</roo>"
    authortree = ET.fromstring(str(authors))
    author_list = []
    for au in authortree.iter('a'):
        author_list.append(au.text)
    return author_list

#Turns author list into a nice string.
def auth_string(auth_list):
    if len(auth_list)==1:
        app = auth_list[0]
    else:
        app = ""
        for ii in range(len(auth_list)-1):
            app = app + auth_list[ii]+ ","
        app = app + auth_list[-1]
    return app

#Checks subject areas for overlaps to avoid doubling up entries
def subject_overlaps(subj,subject_areas,entry):
    ent_sub = entry.title.split(".")[-1]
    subjects = subject_areas.copy()
    subjects.remove(subj)
    overlap = False
    for subject in subjects:
        if subject in ent_sub:
            overlap = True
            break
    arxiv_number = entry.link.lstrip('http://arxiv.org/abs/')
    if arxiv_number in watchlist:
        overlap = True
    if not subj in ent_sub:
        watchlist.append(arxiv_number)
    return overlap

#Queries arxiv for new entries given subject area and saves 
#file in format designed to be parsed by C-written frontend
def get_new_entries(subject,subjects):
    base_url = 'http://export.arxiv.org/rss/'
    new_url = base_url+subject
    new_feed = feedparser.parse(new_url)
    entries = []
    #renders entries into format readable by frontend 
    for entry in new_feed.entries:
        if subject_overlaps(subject,subjects,entry):
            continue
        entry_title = entry.title.split(".")[0]
        arxiv_number = entry.link.lstrip('http://arxiv.org/abs/')
        abstract = parse_abstract(entry)
        author_list = parse_authors(entry)
        auths = auth_string(author_list)
        entry = ("\n",arxiv_number,
                  entry_title,
                   auths,abstract)
        entry_text = "\n".join(entry)
        entries.append(entry_text)
    content = "".join(entries)
    with open(subject, 'w+') as file:
        file.write(content)

#Parsing the subject area
#print('Your first argument was: '+sys.argv[1])
with open('subjects.conf','r') as sfile:
    lines=sfile.readlines()
    subjects=[line.rstrip() for line in lines]

#watchlist is for fernickity cases of overlaps where the 
#subject isn't covered but it could be in two crosslists
watchlist = []

get_new_entries(subjects[0],subjects)
