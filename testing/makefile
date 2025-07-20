objects := oneconnectiontest.o Connections.o Neurons.o \
				SignalRingBuffer.o

headers := Connection.h Connections.h Neuron.h Neurons.h \
				SignalRingBuffer.h TCNConstants.h 

files := file1 file2

gnucompiler := C:/msys64/ucrt64/bin/g++.exe

codebase := E:/Coding/c++/TCNV4
include := $(codebase)/include/
testtarget := $(codebase)/testing/lib/
onetest := $(CURDIR)/oneconnectiontest.exe

buildtest:  addprefix($(include)$(headers)
	@echo $?

Connections.o:

Neurons.o:

SignalBuffer.o:

echocompiler:
	echo $(gnucompiler)

trythis:
	# echo $(gnucompiler)
	$(gnucompiler) -c -fdiagnostics-color=always -g \
		oneconnectiontest.cpp -o $(testtarget)oneconnectontest.o



./lib/oneconnectiontest.o : $(include)$(headers)
	@echo Just Compile 
	echo $(gnucompiler)
	$(gnucompiler) -c -fdiagnostics-color=always -g  -I $(include) \
		$(CURDIR)/oneconnectiontest.cpp -o $(testtarget)oneconnectiontest.o


Connection.h:
	@echo $@

Connections.h:
	@echo $@

Neuron.h:
	@echo $@

Neurons.h:
	@echo $@

TCNConstants.h:
	@echo $@

SignalRingBuffer.h:
	@echo $@

./lib/oneconnectiontest.exe : $(include)$(headers)
	@echo "Compile and Link" 
	echo $(gnucompiler)
	$(gnucompiler)  -fdiagnostics-color=always -g \
		-I $(include) \
		$(CURDIR)/oneconnectiontest.cpp -o \
		$(testtarget)oneconnectiontest.exe

runoneconnectiontest:
	$(testtarget)/oneconnectiontest.exe

tryit:
	@echo $(include)
	@echo $(testtarget)
	@echo $(onetest)
	@echo $(CURDIR)
	@echo $(include)Connections.h


some_file: $(files)
	@echo "Look at this variable:" $(files)
	@echo $<	# first prereq
	@echo $^  # all prereqs

file1:
	@echo $@	# ref target name

file2:
	@echo $@


prefixtest: 
	echo (addprefix $(include),$(headers))
