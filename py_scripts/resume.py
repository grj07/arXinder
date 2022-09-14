import json
import shutil
import datetime, holidays, pytz, time

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


#load in the current configuration
with open('config/conf.json','r') as cfile:
    conf = json.load(cfile)

#Parsing the subject area
with open('config/subjects.conf','r') as sfile:
    lines=sfile.readlines()
    subjects=[line.rstrip() for line in lines]

with open("state/res","r") as rfile:
    file = rfile.readlines()[0]
    resume = file[0]
    sub_no = file[1:]

next_feed = det_next_feed(conf,subjects)
if next_feed[1] == subjects[0] and next_feed[0]==0:
    #This is the case where there is a full resume, 
    #entry file is ready as get_entries has run
    resume = '0'
    sub_no = '0'
    conf['next_feed'] = ' '
    conf['previous_feed'] = ' '
else:
    resume = '1'
    sub_no = sub_no
    file_string = "state/p_feeds/"+conf['previous_feed']
    shutil.copyfile(file_string,conf['previous_feed'][:-8])

fstr = resume+' '+sub_no

with open("state/res","w") as wfile:
    wfile.write(fstr)
with open('config/conf.json','w') as cfile:
    json.dump(conf,cfile)
