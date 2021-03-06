CC = g++
CFLAGS = `wx-config --cflags` -std=c++14 -O2
LIBS = `wx-config --libs` -lwx_gtk3u_aui-3.1 -lwx_gtk3u_stc-3.1 -s
OBJS = main.o def.o dialog.o edit.o frame.o panel.o CustomTabArt.o \
	language.o languageCpp.o languageCss.o languageHtml.o languageJava.o \
	languageJS.o languagePhp.o languagePython.o languageXml.o connection.o
INSTALL_DIR = $(HOME)/.NoteCode

all : NoteCode

NoteCode :  $(OBJS)
	$(CC) -o NoteCode $(OBJS) $(LIBS) 

	
dialog.o : dialog.cpp dialog.h
	$(CC) $(CFLAGS)  -c dialog.cpp

def.o : def.cpp def.h
	$(CC) $(CFLAGS) -c def.cpp

edit.o : edit.cpp edit.h
	$(CC) $(CFLAGS) -c edit.cpp
	
frame.o : frame.cpp frame.h
	$(CC) $(CFLAGS) -c frame.cpp
	
main.o : main.cpp main.h
	$(CC) $(CFLAGS) -c main.cpp
	
panel.o : panel.cpp panel.h
	$(CC) $(CFLAGS) -c panel.cpp
	
CustomTabArt.o : CustomTabArt.cpp CustomTabArt.h
	$(CC) $(CFLAGS) -c CustomTabArt.cpp
	
language.o : languages/language.cpp language.h
	$(CC) $(CFLAGS) -c languages/language.cpp

languageCpp.o : languages/languageCpp.cpp language.h
	$(CC) $(CFLAGS) -c languages/languageCpp.cpp
	
languageCss.o : languages/languageCss.cpp language.h
	$(CC) $(CFLAGS) -c languages/languageCss.cpp

languageJava.o : languages/languageJava.cpp language.h
	$(CC) $(CFLAGS) -c languages/languageJava.cpp

languageJS.o : languages/languageJS.cpp language.h
	$(CC) $(CFLAGS) -c languages/languageJS.cpp

languagePhp.o : languages/languagePhp.cpp language.h
	$(CC) $(CFLAGS) -c languages/languagePhp.cpp

languagePython.o : languages/languagePython.cpp language.h
	$(CC) $(CFLAGS) -c languages/languagePython.cpp

languageXml.o : languages/languageXml.cpp language.h
	$(CC) $(CFLAGS) -c languages/languageXml.cpp
	
languageHtml.o : languages/languageHtml.cpp language.h
	$(CC) $(CFLAGS) -c languages/languageHtml.cpp
	
connection.o : connection.cpp connection.hpp
	$(CC) $(CFLAGS) -c connection.cpp
	
clean: 
	rm -f main $(OBJS)
	
install:
	sudo cp ./NoteCode.desktop /usr/share/applications
	mkdir -p $(INSTALL_DIR)
	sudo cp ./NoteCode /usr/bin/
	sudo cp ./images/48/NoteCode.png /usr/share/pixmaps
	cp ./html $(INSTALL_DIR)
	cp ./config.json $(INSTALL_DIR)
	cp -r ./script $(INSTALL_DIR)
	cp -r ./images $(INSTALL_DIR)
	
