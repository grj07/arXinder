# arXinder
Software to assist in daily management of journal articles and references

-------------------------------------------------------------

0.0.2
New version no longer requires daily email file, queries arxiv and processes todays
entries using feedparser. Includes shell script (execute in bash : bash arxinder.sh)

-------------------------------------------------------------

Version 0.0.1:

Basic idea of program contains two aspects (in seperate directories for now):

1)data_processing is for managing the reference data, this will be written in python,
  but the data format etc is tbd (will depend on inSpire and arXiv API, standard usage
  in reference management software). 
  Very basic version here takes the daiy arxiv email and turns it into a simple format for
  the C user interface to read. Files here are:

	-daily_email.dat is the downloaded daily email from the arxiv with new hep-th and 
	 hep-ph publications.

	-datprc.py is a python program which takes daily_email.dat and produces prcd.dat,
	 which is correctly formatted for the user interface
        
2) user_interface is a program written in ncurses (C) which prints prc.dat on the output,
  and one can seek left and right through the entries. The final intent here is to write this
  for an app like tinder so that people can swipe right and left on arxiv papers they are interested
  in or don't care about. This would be integrated with some form of reference data base and have basic
  analytical tools like tagging common authors, references, keyword analysis etc.. For the moment the
  files here are:
	-prcd.dat is the aforementioned processed data used to read in the entries.
	-ui.c is the interface/visualiser written in C for ncurses (compile with "gcc -o ui ui.c -lncurses")
	-ui is program (execute with "./ui")
