CXX = clang++
SRC = \
			main.cpp \
			defines.cpp \
			envelope.cpp \
			fmvoice.cpp \
			fmsynth.cpp \
			jackcpp/src/jackaudioio.cpp

CXXFLAGS += -std=c++11 -g -Wall
CXXFLAGS += -Ijackcpp/include/
LDFLAGS += -ljack
OBJ = ${SRC:.cpp=.o}

test: ${OBJ}
	${CXX} ${CXXFLAGS} -o test ${OBJ} ${LDFLAGS} 

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} -o $*.o $*.cpp
	${CXX} -MM $(CXXFLAGS) $*.cpp > $*.d

clean:
	@rm -f *.o jackcpp/src/*.o ${TARGETS} *.d jackcpp/src/*.d
