import datetime, holidays, pytz

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
    auth_str = ','.join(auth_list)
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

#gives date of original feed 
def calc_original_feed_date(upload_deadline):
    feed_time = upload_deadline
    while(feed_time.weekday() in [4,5]):
        feed_time = feed_time + datetime.timedelta(days=1)
    feed_time = feed_time + datetime.timedelta(hours=6.5)
    feed_time = est_to_gmt(feed_time)
    return feed_time

#gives time of latest feed
def latest_feed_time(utc_time):
    cur_est_time = gmt_to_est(utc_time)
    est_min = 60*cur_est_time.hour+cur_est_time.minute
    check_time = cur_est_time
    #check when latest upload deadline was
    if est_min<20.5*60:
        check_time = check_time-datetime.timedelta(days=1)
    if check_time.weekday() == 6:
        check_time = check_time-datetime.timedelta(days=2)
    while(no_deadl_today(check_time)):
        check_time = check_time-datetime.timedelta(days=1)
    check_time = check_time.replace(hour = 18, minute = 30, second =0,microsecond = 0)
    #if the latest deadline was friday, the latest feed will be thursdays EST (Friday GMT)
    if check_time.weekday() == 4:
        check_time = check_time-datetime.timedelta(days=1)
    #Otherwise the latest feed will always come before the next deadline
    feed_date = check_time.replace(hour = 20, minute = 30, second =0,microsecond = 0)
    feed_date = est_to_gmt(feed_date)
    return feed_date

#work out what the next feed to read is
def det_next_feed(conf,subjects):
    time_now = datetime.datetime.utcnow()
    pf_time = latest_feed_time(time_now)
    pf_date = pf_time+datetime.timedelta(hours=12)
    date_str = pf_date.strftime("%d%m%Y")
    depth = 0
    doneq = False
    for sub in subjects:
        feed_str = sub+date_str
        if feed_str in conf['read_feeds']:
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
            if feed_str in conf['read_feeds']:
                continue
            next_sub = sub
            doneq = True 
            break
    return [depth,next_sub,date_str]
