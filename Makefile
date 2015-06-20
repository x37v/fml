CXX = clang++
SRC = \
			main.cpp \
			jackcpp/src/jackaudioio.cpp

CXXFLAGS += -Ijackcpp/include/
LDFLAGS += -ljack
OBJ = ${SRC:.cpp=.o}

.cpp.o:
	${CXX} -c ${CXXFLAGS} -o $*.o $<

test: ${OBJ}
	${CXX} ${CXXFLAGS} -o test ${OBJ} ${LDFLAGS} 

clean:
	@rm -f *.o jackcpp/src/*.o ${TARGETS}
