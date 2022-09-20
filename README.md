# arXinder
Software to assist in sorting daily feeds of journal articles and references from arxiv.org

Version 0.1

Build Dependencies:
Building and running arXinder require the following to be installed on the users system:

	gcc
	ncurses
	jansson
	python 3.10

Furthermore, the following python libraries are needed (also listed in requirements.txt):

	feedparser
	holidays
	pytz

Build:
to build the program move into the application folder and run:
	make

To run the program: 
navigate to the directory containing the file

	arXinder

run the program in a large-enough virtual terminal using 

	./arXinder

view list of chosen papers by opening 

	chosen_entries.html

using a web browser.

Structure:

The software consists of a user interface written in C which displays entries from a feed. This feed is labelled by subject area and date.

The user interface is written in ncurses, and processes a feed of a given subject (as set in conf/subjects.conf) for a given day (starting with the most recent feed and running backwards in time).
The user then navigates the entries, using return to save an entry and backspace to dismiss it. This can carry on for as long as the user wishes, with feeds from previous days generated as the program runs on.
The key 'u' can be used to undo within a feed only.
The key 's' can be used to adjust subject areas. The program must then be restarted to enact changes.
The key 'q' can be used to exit the menu and the program.

Upon program termination, the script "py_scripts/mkchl.py" generates an html-formatted documents in the main directory called

	chosen_entries.html

This can be viewed with your favourite browser, e.g. by typing the following into the terminal

	chromium chosen_entries.html .

If there are no new feeds in the day, the program will resume from where it exited when exit. Otherwise it will download the latest feeds and resume the previous session once caught up.

Python scripts:

get_entries.py -- queries arxiv rss/api and lays up the next feed for the user interface
resume.py -- checks the feeds and configuration to see if resume is the correct response or if there is a new feed (then the title animation will run)
mk_html.py -- generates html output
