ACLOCAL_AMFLAGS = -Im4

bin_PROGRAMS = zvmail

AM_CXXFLAGS = -std=c++11 \
			  -g3 \
			  -O0 \
			  -Wall \
			  -Werror

zvmail_SOURCES = src/base/tcpclient.cpp \
				 src/base/imap.cpp
zvmail_CFLAGS = $(SSL_CFLAGS)
zvmail_LDADD = $(SSL_LIBS)
