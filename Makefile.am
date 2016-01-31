ACLOCAL_AMFLAGS = -Im4

bin_PROGRAMS = zvmail
if GOOGLE_TEST
bin_PROGRAMS += test_zvmail
endif

# Main program ============================================
zvmail_CXXFLAGS = -std=c++11 \
				  -g3 \
				  -O0 \
				  -pthread \
				  -Wall \
				  -Werror \
				  -DDEBUG
zvmail_SOURCES = src/utils/utils.cpp \
				 src/utils/logger.cpp \
				 src/base/tcpclient.cpp \
				 src/base/imap.cpp \
				 src/main.cpp
zvmail_CFLAGS = $(SSL_CFLAGS) \
				$(PCRE_CFLAGS) \
				$(SASL_CFLAGS)
zvmail_LDADD = $(SSL_LIBS) \
			   $(PCRE_LIBS) \
			   $(SASL_LIBS)

if GOOGLE_TEST
# Test program ============================================
test_zvmail_CXXFLAGS = -std=c++11 \
					   -O0 \
					   -g3 \
					   -pthread \
					   -Wall \
					   -Werror \
					   -isystem $(GTEST_DIR)/include \
					   -isystem $(GMOCK_DIR)/include
test_zvmail_SOURCES = test/main.cpp \
					  src/utils/utils.cpp \
					  src/utils/logger.cpp \
					  src/base/tcpclient.cpp \
					  src/base/imap.cpp
test_zvmail_CFLAGS = $(SSL_CFLAGS) \
					 $(PCRE_CFLAGS) \
					 $(SASL_CFLAGS)
test_zvmail_LDADD = test/libgmock.a \
					$(SSL_LIBS) \
					$(PCRE_LIBS) \
					$(SASL_LIBS)

# Custom targets ==========================================
check: test_zvmail
	@echo "Running test cases..."
	@./test_zvmail

test/libgmock.a:
	@echo "Building Google Test Framework..."
	@g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
		-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
		-pthread -c $(GTEST_DIR)/src/gtest-all.cc
	@g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
		-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
		-pthread -c $(GMOCK_DIR)/src/gmock-all.cc
	@ar -rc test/libgmock.a gtest-all.o gmock-all.o
	@rm gtest-all.o gmock-all.o

endif

# idea from: http://bit.ly/1R9CGuC
ctags: zvmail
	@echo "generating ctags..."
	find . \( -name \*.cpp -o -name \*.h \) \
		| xargs g++ -std=c++11 -M -I $(SSL_CFLAGS) $(PCRE_CFLAGS) $(SASL_CFLAGS) . \
		| sed -e 's/[\\ ]/\n/g' | sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' \
		| ctags -V -f .ctags --c++-kinds=+p --fields=+iaS \
	   	--extra=+q -L - --language-force=c++ --exclude='.git'

.PHONY: check rebuild-gtest
