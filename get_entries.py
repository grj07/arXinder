import json, feedparser, html
import math
import shutil
import datetime, holidays, pytz, time, calendar

#Returns abstract free of HTML and newline formatting.
def parse_feed_abstract(entry):
    abstract = entry.description.lstrip("<p>")\
                                .rstrip("</p>")\
                                .rstrip()\
                                .replace("\n"," ")
    return abstract

#Returns entry authors as list
def parse_feed_authors(entry):
    authors = entry.author.split(',')
    parsed = []
    for author in authors:
        if '>' in author:
            parsed.append(author.split('>')[1].split('<')[0])
        else:
            parsed.append(author)
    return parsed

#auth list to string for C program
def auth_string(auth_list):
    l = len(auth_list)
    if l>30:
        short_list = auth_list[:30]+["+ "+str(l-30)+" more"]
    else:
        short_list = auth_list
    auth_str = ','.join(short_list)
    return(auth_str)

#converts GMT time to EST
def gmt_to_est(gmtdt):
    gmt = pytz.timezone('GMT')
    est = pytz.timezone('US/Eastern')
    feed_time = gmt.localize(gmtdt)
    feed_time = feed_time.astimezone(est)
    feed_time = feed_time.replace(tzinfo=None)
    return feed_time

def est_to_gmt(estdt):
    gmt = pytz.timezone('GMT')
    est = pytz.timezone('US/Eastern')
    feed_time = est.localize(estdt)
    feed_time = feed_time.astimezone(gmt)
    feed_time = feed_time.replace(tzinfo=None)
    return feed_time


#works out if there is an arxiv update/deadline on given day EST
def no_deadl_today(cuttime):
    if cuttime.weekday()>4:
        return True 
    else:
        yankhols = holidays.US(years = cuttime.year, state = 'NY')
        if cuttime.date() in yankhols:
            return True 
        else:
            return False 

#calculates the arxiv upload deadline for n feeds back from this batch
def calc_upload_deadline(feed_time,n):
    feed_time = gmt_to_est(feed_time)
    time_step = datetime.timedelta(hours = 6.5)
    cutoff_time = feed_time - time_step #cutoff_time is now 6hrs 30 mins before feed EST
    while(no_deadl_today(cutoff_time)):
        cutoff_time = cutoff_time - datetime.timedelta(days=1) #now previous deadline
    for ii in range(n):
        cutoff_time = cutoff_time-datetime.timedelta(days=1)
        while(no_deadl_today(cutoff_time)):
            cutoff_time = cutoff_time - datetime.timedelta(days=1)
    #now n deadines before that
    return cutoff_time

#gives date of original feed (in gmt) of upload deadline (in EST)
def calc_original_feed_date(upload_deadline):
    feed_time = upload_deadline
    while(feed_time.weekday() in [4,5]):
        feed_time = feed_time + datetime.timedelta(days=1)
    feed_time = feed_time.replace(hour = 20, minute = 30, second =0,microsecond = 0)
    feed_time = est_to_gmt(feed_time)
    return feed_time

#gives time of latest feed
def latest_feed_time(utc_time):
    cur_est_time = gmt_to_est(utc_time)
    est_min = 60*cur_est_time.hour+cur_est_time.minute
    check_time = cur_est_time
    #check when latest upload deadline 
    #skip back to latest deadline day
    if est_min>14*60 and not no_deadl_today(cur_est_time):
        #case where after deadline and after feed update
        if est_min>20.5*60:
            feed_date = check_time.replace(hour = 20, minute = 30, second =0,microsecond = 0)
            return est_to_gmt(feed_date)
        else:
            check_time = check_time-datetime.timedelta(days=1) 
    while(no_deadl_today(check_time)):
        check_time = check_time-datetime.timedelta(days=1)
    deadl = check_time.replace(hour = 14, minute = 00, second =0,microsecond = 0)
    feed_time = calc_original_feed_date(deadl)
    #check whether latest feed time is before or after relevant time 
    if feed_time > utc_time: 
        deadl = deadl - datetime.timedelta(days=1)
        while(no_deadl_today(check_time)):
            deadl = deadl-datetime.timedelta(days=1)
        feed_time = calc_original_feed_date(deadl)
    return feed_time 

#work out what the next feed to read is
def det_next_feed(conf,subjects):
    time_now = datetime.datetime.utcnow()
    pf_time = latest_feed_time(time_now)
    pf_date = pf_time+datetime.timedelta(hours=12)
    date_str = pf_date.strftime("%d%m%Y")
    depth = 0
    doneq = False
    if conf['previous_feed'] in conf['read_feeds']:
        feed_list = conf['read_feeds'] +[conf['next_feed']]
    else:
        feed_list = conf['read_feeds'] +[conf['previous_feed']]
    for sub in subjects:
        feed_str = sub+date_str
        if feed_str in feed_list:
            continue
        next_sub = sub
        doneq = True 
        break
    while not doneq:
        depth = depth +1
        up_dline = calc_upload_deadline(pf_time,depth)
        pf_date = calc_original_feed_date(up_dline)
        date_str = pf_date.strftime("%d%m%Y")
        for sub in subjects:
            feed_str = sub+date_str
            if feed_str in feed_list:
                continue
            next_sub = sub
            doneq = True 
            break
    return [depth,next_sub,date_str]


#The feeds returned from queries are very different from what comes from the rss feed. 
#As a result, they have seperate functions for the stages of the process 


#get new entries from rss feed. 0th entry is the date of feed  
#query count = 1
def get_new_entries(subject): 
    base_url = 'http://export.arxiv.org/rss/'
    new_url = base_url+subject
    new_feed = feedparser.parse(new_url)
    feed_time = new_feed.updated_parsed
    new_entries = [[time.strftime("%d%m%Y",feed_time),time.strftime("%a %d %b %Y")],new_feed.entries]
    return new_entries

#get old entries from query.
#query count = 1
def get_old_entries(subject):
    #latest feed, should be downloaded or code returns 0
    time_now = datetime.datetime.utcnow()
    pf_time = latest_feed_time(time_now)
    feed_string =subject + pf_time.strftime("%d%m%Y")
    #depth and ent_count climb with number of entries
    depth = 0
    ent_count= 0
    while feed_string in conf['saved_feeds']:
        with open("p_feeds/"+feed_string) as f_file:
            head = json.loads(f_file.readlines()[0])
        ent_count = ent_count+head["raw_no_of_entries"]
        depth = depth + 1
        up_dl =calc_upload_deadline(pf_time,depth)
        feed_time = calc_original_feed_date(up_dl)
        feed_string =subject + feed_time.strftime("%d%m%Y")
    if depth == 0:
        print("Wrong usage of get_old_entries - download latest entries first")
        return 0
    lw_dl = est_to_gmt(calc_upload_deadline(pf_time,depth+1))
    up_dl = est_to_gmt(up_dl)
    #subtract 20 from entry count to capture anomalies
    ent_count = ent_count - 20
    #build string for query
    query_string ="search_query=cat:"+subject
    query_string = query_string+"&sortBy=submittedDate&sortOrder=descending"
    query_string = query_string+"&start="+str(ent_count)+"&max_results=200"
    link = "http://export.arxiv.org/api/query?"+query_string
    feed = feedparser.parse(link)
    time.sleep(3)
    entries = []
    for entry in feed.entries:
        entry_time = datetime.datetime.utcfromtimestamp(calendar.timegm(entry.published_parsed))
        if entry_time < up_dl and entry_time >= lw_dl:
            entries.append(entry)
    entries.reverse()
    old_entries = [[feed_time.strftime("%d%m%Y"),feed_time.strftime("%a %d %b %Y")],entries]
    #correct the entry count on other feeds for more accuracy -- TO DO
    return old_entries 

#Captures cross-listed entries to prevent repeating
def filter_new_entries(entries,subject):
    filtered_entries = []
    raw_no_of_entries = len(entries[1])
    for entry in entries[1]:
        title_bits = entry.title.split(".")
        arxiv_number = entry.link.lstrip('http://arxiv.org/abs/')
        if "UPDATED" in title_bits[-1]:
            raw_no_of_entries = raw_no_of_entries-1
        else:
            if '[' in title_bits[-1]:
                title = "".join(title_bits[:-2])
            else:
                title = "".join(title_bits[:-3])
            if arxiv_number not in conf['crosslist'] and entry.title.split("[")[1].split("]")[0] not in conf['crosssub']:
                if subject not in title_bits[-2]+title_bits[-1]:
                    entry.title = title+" - (cross-listed "+entry.title.split("[")[1].split("]")[0]+")"
                    conf['crosslist'].append(arxiv_number)
                else:
                    entry.title = title
                filtered_entries.append(entry)
    return [[entries[0],raw_no_of_entries],filtered_entries]

def filter_old_entries(entries,subject):
    filtered_entries = []
    for entry in entries[1]:
        arxiv_number = entry.id[-12:-2]
        if arxiv_number not in conf['crosslist'] and entry.arxiv_primary_category['term'] not in conf['crosssub']:
            if not subject == entry.arxiv_primary_category['term']:
                conf['crosslist'].append(arxiv_number)
                entry.title = entry.title+" - (cross-listed "+entry.arxiv_primary_category['term']+")"
            filtered_entries.append(entry)
    return [[entries[0],len(entries[1])],filtered_entries]

#formatting is creating a list to write linewise to file readable by frontend
def format_new_entries(entries,subject):
    header = {"subject":subject,"raw_no_of_entries": entries[0][1],\
            "date":entries[0][0][0],"datef":entries[0][0][1], "cur_entry_no":1}
    formatted_entries = [str(header).replace("\'","\"")]
    for entry in entries[1]:
        arxiv_number = entry.link.lstrip('http://arxiv.org/abs/')
        abstract = html.unescape(parse_feed_abstract(entry))
        author_list = parse_feed_authors(entry)
        auths = html.unescape(auth_string(author_list))
        entry = ["\n",arxiv_number,html.unescape(entry.title),auths,abstract]
        entry_text = "\n".join(entry)
        formatted_entries.append(entry_text)
    if len(entries[1])<1:
        formatted_entries.append("\n\nxxxx.xxxxx\nNO ENTRIES FOR THIS SUBJECT TODAY\nNone\nplease hit backspace to continue\n")
    header['filtered_no_of_entries']=len(formatted_entries)-1
    formatted_entries[0] = str(header).replace("\'","\"")
    return formatted_entries

def format_old_entries(entries,subject):
    header = {"subject":subject,"raw_no_of_entries": entries[0][1],\
            "date":entries[0][0][0],"datef":entries[0][0][1], "cur_entry_no":1}
    formatted_entries = [str(header).replace("\'","\"")]
    for entry in entries[1]:
        arxiv_number = entry.id.lstrip('http://arxiv.org/abs/')[:-2]
        abstract = entry.summary.replace("\n"," ")
        author_list = [author['name'] for author in entry.authors]
        auths = auth_string(author_list)
        entry = ["\n",arxiv_number,entry.title.replace("\n","").replace("  "," "),auths,abstract]
        entry_text = "\n".join(entry)
        formatted_entries.append(entry_text)
    if len(entries[1])<1:
        formatted_entries.append("\n\nxxxx.xxxxx\nNO ENTRIES FOR THIS SUBJECT TODAY\nNone\nplease hit backspace to continue\n")
    header['filtered_no_of_entries']=len(formatted_entries)-1
    formatted_entries[0] = str(header).replace("\'","\"")
    return formatted_entries

#store to file and update config with useful data
def store_to_file(entries,filename):
    head = json.loads(entries[0])
    conf['saved_feeds'].append(head['subject']+head['date'])
    if head['subject'] in conf:
        conf[head['subject']].append(head['raw_no_of_entries'])
    else:
        conf[head['subject']]=[head['raw_no_of_entries']]
    content = "".join(entries)
    with open(filename, 'w') as file:
        file.write(content)

            

#load in the current configuration
with open('conf.json','r') as cfile:
    conf = json.load(cfile)

#Parsing the subject area
#print('Your first argument was: '+sys.argv[1])
with open('subjects.conf','r') as sfile:
    lines=sfile.readlines()
    subjects=[line.rstrip() for line in lines]

#update read feeds
for feed in conf['saved_feeds']:
    if feed not in conf['read_feeds']:
        with open("p_feeds/"+feed,"r") as feedfile:
            head = json.loads(feedfile.readlines()[0])
            if head['cur_entry_no'] > head['filtered_no_of_entries']:
                conf['read_feeds'].append(feed)


#det next feed determines which feed to read next [0] is depth, [1] is entries [2] is date
next_feed = det_next_feed(conf,subjects)

#construct strings for file manipulations 
nf_string = next_feed[1]+next_feed[2]
file_string = 'p_feeds/' + nf_string

if conf['previous_feed'] in conf['read_feeds']:
    conf['previous_feed']=conf['next_feed']
conf['next_feed'] = nf_string
if conf['previous_feed']==" ":
    conf['previous_feed']=conf['next_feed']

#needs to download if not saved already
if not nf_string in conf['saved_feeds']:
    #no overlaps to be caught on first subject
    if next_feed[1] == subjects[0]:
        conf['crosslist'] = []
        conf['crosssub'] = []
    #Depth 1 or more, old entries queried from the arxiv
    if next_feed[0]:
        ents = format_old_entries(filter_old_entries(get_old_entries(next_feed[1]),next_feed[1]),next_feed[1])
    else:
    #Depth 0: next feed rss from the arxiv
        ents = format_new_entries(filter_new_entries(get_new_entries(next_feed[1]),next_feed[1]),next_feed[1])
    head =  json.loads(ents[0])
    store_to_file(ents,file_string)

#copy over file and save config
shutil.copyfile(file_string,next_feed[1])
conf['crosssub'].append(next_feed[1])
with open('conf.json','w') as cfile:
    json.dump(conf,cfile)
