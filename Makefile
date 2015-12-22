CXX = clang++
SRC = \
			main.cpp \
			defines.cpp \
			envelope.cpp \
			fmvoice.cpp \
			fmsynth.cpp \
			midiproc.cpp \
			adsr.cpp \
			util.cpp \
			jackcpp/src/jackaudioio.cpp \
			jackcpp/src/jackmidiport.cpp

CXXFLAGS += -std=c++11 -g -Wall
CXXFLAGS += -Ijackcpp/include/
LDFLAGS += -ljack
OBJ = ${SRC:.cpp=.o}

fml: ${OBJ}
	${CXX} ${CXXFLAGS} -o fml ${OBJ} ${LDFLAGS} 

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} -o $*.o $*.cpp
	${CXX} -MM $(CXXFLAGS) $*.cpp > $*.d

clean:
	@rm -f *.o jackcpp/src/*.o ${TARGETS} *.d jackcpp/src/*.d

run: fml
	eval './fml &' && sleep 1 && \
		jack_connect fm:output0 system:playback_1 && \
		jack_connect fm:output1 system:playback_2 && \
		jack_connect system:midi_capture_1 fm:fml_in ; \
		jack_connect system:midi_capture_2 fm:fml_in ; \
		jack_connect system:midi_capture_3 fm:fml_in ; \
		jack_connect system:midi_capture_4 fm:fml_in ; \
		jack_connect system:midi_capture_5 fm:fml_in ; \
		jack_connect system:midi_capture_6 fm:fml_in ; \
		jack_connect system:midi_capture_7 fm:fml_in ; \
		wait
