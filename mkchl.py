import json

def entry_template(entry):
    entstr = """<h3><a href="http://www.arxiv.org/pdf/"""
    entstr = entstr+entry['arxiv_no']+"""\">"""+entry['arxiv_no']+"""</a></h3>
<h3>"""+entry['title']+"""</h3>
<h4>"""+", ".join(entry['authors'])+"""</h4>
<p>"""+entry['abstract']+"""</p>\n"""
    return(entstr)

#need feed metadata
with open("saved_entries.d","r") as sefile:
    chli = sefile.readlines()
    feeds = []
    head = eval(chli[0].rstrip())
    entries = []
    lc = -1;
    entry = {}
    check = True
    for line in chli:
        typ = (lc)%5
        if typ == 1:
            entry = {}
            entry['arxiv_no']=line.rstrip()
        elif typ == 2:
            entry['title']=line.rstrip()
        elif typ == 3:
            entry['authors']=line.split(',')
            entry['authors'][-1]=entry['authors'][-1].rstrip()
        elif typ == 4 and lc!=-1:
            entry['abstract']=line.rstrip()
        elif typ == 0 and lc!=0:
            if check:
                entries.append(entry)
            else:
                check = True
            if len(line)>4:
                feeds.append([head,entries])
                head =  eval(line.rstrip())
                entries = []
                check = False
                continue
        lc  = lc+1
    if typ==4:
        entries.append(entry)
    feeds.append([head,entries])
hstring = """<!DOCTYPE html>
<html>
<head>
<title>Feed for """+ feeds[0][0]['datef'] +"""</title>
<script>MathJax = {
  tex: {
    inlineMath: [['$', '$']]
  },
  svg: {
    fontCache: 'global'
  }
};
  </script>
<script src="https://polyfill.io/v3/polyfill.min.js?features=es6"></script>
<script type="text/javascript" id="MathJax-script" async 
  src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js">
</script>
<style>p{
   word-wrap: break-word;
   width: 1000px;
   font-size:20px;
   font-family: serif;
   text-align: justify;
}
h1{
   text-align: center;
   font-family: sans-serif;
}
h2{
	line-height : 10px;
}
h3{
	line-height : 10px;
}
</style>
	</head>
	<body>
"""
dt = feeds[0][0]['datef']
for feed in feeds:
    head = feed[0]
    hstring = hstring+ """<h1>Choices from """+head['subject']+""" feed on """+head['datef']+"""</h1>\n"""
    for entry in feed[1]:
        hstring = "".join([hstring,entry_template(entry)])

end =""" 
</body>
</html>
"""

hstring = "".join([hstring,end])
with open("chosen_entries.html","w") as ofile:
    ofile.write(hstring)
