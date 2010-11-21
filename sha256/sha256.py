import os, sys
from Crypto.Hash import SHA256

def main():
    if len(sys.argv) != 2:
        print 'usage: python', sys.argv[0], '<file>'
        return

    f = open(sys.argv[1]).read()
    sha256 = SHA256.new(f)
    print sha256.hexdigest()

if __name__ == '__main__':
    main()
