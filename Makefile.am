ACLOCAL_AMFLAGS = -Im4

bin_PROGRAMS = zvmail
if GOOGLE_TEST
bin_PROGRAMS += test_zvmail
endif

# Main program ============================================
zvmail_CXXFLAGS = -std=c++11 \
				  -g3 \
				  -O0 \
				  -Wall \
				  -Werror
zvmail_SOURCES = src/base/tcpclient.cpp \
				 src/base/imap.cpp \
				 src/utils/utils.cpp
zvmail_CFLAGS = $(SSL_CFLAGS) \
				$(PCRE_CFLAGS)
zvmail_LDADD = $(SSL_LIBS) \
			   $(PCRE_LIBS)

if GOOGLE_TEST
# Test program ============================================
test_zvmail_CXXFLAGS = -std=c++11 \
					   -O0 \
					   -g3 \
					   -pthread \
					   -isystem \
					   -Wall \
					   -Werror
					   $(GTEST_DIR)/include
test_zvmail_SOURCES = test/main.cpp \
					  src/base/tcpclient.cpp \
					  #src/base/imap.cpp \
					  src/utils/utils.cpp
test_zvmail_CFLAGS = $(SSL_CFLAGS) \
					 $(PCRE_CFLAGS)
test_zvmail_LDADD = test/libgtest.a \
					$(SSL_LIBS) \
					$(PCRE_LIBS)

# Custom targets ==========================================
check: test_zvmail
	@echo "Running test cases..."
	@./test_zvmail

test/libgtest.a:
	@echo "Building Google Test Framework..."
	@g++ -isystem -pthread \
		-I$(GTEST_DIR) \
		-c $(GTEST_DIR)/src/gtest-all.cc
	@ar -rv test/libgtest.a gtest-all.o > /dev/null 2>&1
	@rm gtest-all.o

endif

# idea from: http://bit.ly/1R9CGuC
ctags: zvmail
	@echo "generating ctags..."
	find . \( -name \*.cpp -o -name \*.h \) \
		| xargs g++ -std=c++11 -M -I $(SSL_CFLAGS) $(PCRE_CFLAGS) . \
		| sed -e 's/[\\ ]/\n/g' | sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' \
		| ctags -V -f .ctags --c++-kinds=+p --fields=+iaS \
	   	--extra=+q -L - --language-force=c++ --exclude='.git'

.PHONY: check rebuild-gtest
