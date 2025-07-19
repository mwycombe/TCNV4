objects = oneconnectiontest.o Connections.o Neurons.o \
				SignalRingBuffer.o
headers = Connection.h Connections.h Neuron.h Neurons.h \
				SignalRingBuffer.h TCNConstants.h 
oneconnectiontest:  oneconnectiontest.o, Connection.h, Connections.h \
	Neuron.h, Neurons.h Signal.h SignalRingBuffer.h TCNConstants.h \
	Connections.o Neurons.o SignalRingBuffer.o logger.h logger.o
	@echo "Link everything together"
	C:/msys64/ucrt64/bin/g++.exe" 

oneconnectiontest.o: oneconnectiontest.cpp TCNConstants.h
	# compile this cpp file
	@echo "Compile oneconnection.cpp"
	C:/msys64/ucrt64u/bin/g++.exe -std=c++20 -c -fdiagnostics-color=always \
				-g -I${workspaceFolder}/include 
				"-c",
				"-fdiagnostics-color=always",
				"-g",
				"-I${workspaceFolder}\\include",
				"${file}"

Connections.o: Connections.cpp Connections.h TCNConstants.h
	# compile if any changes
	@echo "Compile Connections.cpp"

Neurons.o:	Neurons.cpp Neurons.h TCNConstants.h
	# compile if any changes
	@echo "Compile Neurons.cpp"

SignalRingBuffer.o: SignalRingBuffer.cpp SignalRingBuffer.h TCNConstants.h
	# compile if any changes
	@echo "Compile SignalRingBuffer.cpp"

logger.o: logger.cpp logger.h
	# compile if any changes
	@echo "Compile logger.cpp"

			"label": "C/C++: Validated compile-only single file, no linking, default output",
			"command": "C:\\msys64\\ucrt64\\bin\\g++.exe",
			"args": [
				"-std=c++20",
				"-c",
				"-fdiagnostics-color=always",
				"-g",
				"-I${workspaceFolder}\\include",
				"${file}"
			],


