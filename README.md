QNetSoul
========
QNetSoul is a netsoul client (protocol use in Ionis Group campus)

It allows students to connect to the internet and discuss between them.

##Build instructions (Linux)

###Dependencies
Make sure to install Qt 5 first.

---

###Compile
Once you have installed all dependencies, you need to compile the application: 

```bash
# Get it from GitHub
git clone https://github.com/8102/QNetSoul.git
cd QNetSoul

# Compile (We use qmake-qt5 to force the use of Qt 5 and not a link to another version of Qt)
qmake-qt5 && make

# Have fun!
./QNetSoul

# You can also move it to your /usr/bin to use it without moving to a special directory (You need to be root)
sudo mv ./QNetSoul /usr/bin/QNetSoul
