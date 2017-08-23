# General notes on getting started

## Eclipse remote server

Download from
'''
http://www.eclipse.org/downloads/download.php?file=/tm/downloads/drops/R-3.5GA-201305311734/rseserver-linux-3.5-GA.tar&mirror_id=346
'''

Fix a bug
Edit server.pl and daemon.pl - hardcode path
cpan Shell

## github access
ssh-keygen -t rsa -b 4096 -C "rstilbor@gmail.com"
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_rsa

Add SSH Key to gthub account ~/ssh/id_rsa.pub

git clone ...
git remote rename origin github

