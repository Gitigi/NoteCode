import sys

try:
	from urllib import request
	urlopen = request.urlopen
except:
	from urllib import urlopen

if(len(sys.argv) >= 2):
	loadedFile = urlopen(sys.argv[1])
	print(loadedFile.read())

else:
	print("Error:")
	print("usage:fileLoader.py url")