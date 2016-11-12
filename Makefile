ARDUINO_LIB_PATH=~/Arduino/libraries
LIB_LIST=Reader ReaderMFRC522 ReaderMFRC530 ReaderMFRC531 Tag TagMF0ICU1 TagMF1ICS70 TagMF1S503x
SOURCE_PATH=`pwd`

all: 
	@echo "Use [install], [unistall] or [doc]"

install:
	@echo "Instaling all libraries..."
	@for lib in $(LIB_LIST) ; do \
		ln -s  $(SOURCE_PATH)/$$lib $(ARDUINO_LIB_PATH); \
	done
	@echo "done."
	
uninstall:
	@echo "Uninstaling all libraries..."
	@for lib in $(LIB_LIST) ; do \
		rm -r $(ARDUINO_LIB_PATH)/$$lib ; \
	done
	@echo "done."
		
doc:
	@echo "Running doxygen..."
	@rm -rf doc
	@mkdir doc
	doxygen doxygen.conf
	@cd doc/latex; make pdf; cp refman.pdf ../../Documentation.pdf 
	@cd ../..
	@rm -rf doc
	@echo "done."
	
	
