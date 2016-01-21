CONN_MODULES_FILES = conn/PracticalSocket conn/TCPConnection
WIFI_MODULES_FILES=wifi/IWConfig wifi/IWConfObject
MAIN_MODULES_FILES = main/APDME main/BandwidthObserver main/ConnTrack main/EM \
				main/QoSImprover main/SystemConfiguration main/TransmissionPowerControl \
				main/WindowManager

MISSING_HEADERS = -I utils/ -I main/ -I conn/ -I wifi/ 

REMOVE = rm -rf

BINARY = TXRegulator

all: binary

binary: compile_conn_modules compile_wifi_modules compile_main_modules 
	$(CXX) -o $(BINARY) \
	conn/PracticalSocket.o conn/TCPConnection.o \
	main/APDME.o main/BandwidthObserver.o main/ConnTrack.o main/EM.o \
	main/QoSImprover.o main/SystemConfiguration.o main/TransmissionPowerControl.o \
	main/WindowManager.o \
	wifi/IWConfig.o wifi/IWConfObject.o \
        -l pthread

compile_conn_modules: 
	$(foreach file, $(CONN_MODULES_FILES), $(CXX) -c $(file).cpp $(MISSING_HEADERS) -o $(file).o ;)

compile_wifi_modules:
	$(foreach file, $(WIFI_MODULES_FILES), $(CXX) -Wall -c $(file).cpp $(MISSING_HEADERS) -o $(file).o ;)

compile_main_modules:
	$(foreach file, $(MAIN_MODULES_FILES), $(CXX) -c $(file).cpp $(MISSING_HEADERS) -o $(file).o ;)

clean:
	$(foreach file, $(CONN_MODULES_FILES), $(REMOVE) $(file).o ;)
	$(foreach file, $(WIFI_MODULES_FILES)$, $(REMOVE) $(file).o ;)
	$(foreach file, $(MAIN_MODULES_FILES), $(REMOVE) $(file).o ;)
	$(REMOVE) $(BINARY)
