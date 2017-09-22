# General notes on getting started

Install eclipse on taregt machine
sudo apt-get install -y xauth

vagrant ssh -- -X /opt/eclipse/eclipse


## Eclipse marketplace
CppCheck
CppStyle


### Install cpplint
'''
sudo curl -L "https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py" -o /usr/bin/cpplint.py
sudo chmod a+x /usr/bin/cpplint.py
'''

/usr/bin/cpplint.py
Line 1895 - start .h filename at char 3
Line 5743 - take chrono out of unapproved list

### Install clang-format - not needed with google style sheet?
'''
sudo apt-get install -y clang-format
'''

### Install cppcheck
'''
sudo apt-get install -y cppcheck
'''


### Google style sheet
cd /opt/eclipse
sudo wget -nv -O eclipse-cpp-google-style.xml https://raw.githubusercontent.com/google/styleguide/gh-pages/eclipse-cpp-google-style.xml





## Eclipse remote server

Download from
'''
http://www.eclipse.org/downloads/download.php?file=/tm/downloads/drops/R-3.5GA-201305311734/rseserver-linux-3.5-GA.tar&mirror_id=346
'''

Fix a bug
Edit server.pl and daemon.pl - hardcode path
cpan Shell - only for daemon





## github access
ssh-keygen -t rsa -b 4096 -C "rstilbor@gmail.com"
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_rsa

Add SSH Key to gthub account ~/ssh/id_rsa.pub

git clone ...
git remote rename origin github

