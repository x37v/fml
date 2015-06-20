CXX = clang++
SRC = \
			main.cpp \
			fmvoice.cpp \
			jackcpp/src/jackaudioio.cpp

CXXFLAGS += -std=c++11 -g -Wall
CXXFLAGS += -Ijackcpp/include/
LDFLAGS += -ljack
OBJ = ${SRC:.cpp=.o}

.cpp.o:
	${CXX} -c ${CXXFLAGS} -o $*.o $<

test: ${OBJ}
	${CXX} ${CXXFLAGS} -o test ${OBJ} ${LDFLAGS} 

clean:
	@rm -f *.o jackcpp/src/*.o ${TARGETS}
