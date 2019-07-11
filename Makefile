LDFLAGS = -Wall
CPPFLAGS = -std=c++14 -O3 -fopenmp -g
SUFFIX = default
BINARY = bin/mbreference-${SUFFIX}
TESTER = bin/tester-${SUFFIX}
TESTER_DETAILED = bin/tester-detailed-${SUFFIX}

all: ${BINARY} note

${BINARY}: build/evolution/individual.o build/main.o
	g++ ${CPPFLAGS} -o $@ $^

tester: build/evolution/individual.o build/tester.o
	g++ ${CPPFLAGS} -o ${TESTER} $^

# for now it's the same as tester
tester-detailed: build/evolution/individual.o build/tester.o
	g++ ${CPPFLAGS} -o ${TESTER_DETAILED} $^

note:
	echo -n "branch: " > ${BINARY}.note
	git branch | sed -n '/\* /s///p' >> ${BINARY}.note
	echo -n "commit: " >> ${BINARY}.note
	git rev-parse HEAD >> ${BINARY}.note
	git diff >> ${BINARY}.note

build/utils/json11/json11.o: utils/json11/json11.hpp utils/json11/json11.cpp
	g++ -c ${CPPFLAGS} -o $@ utils/json11/json11.cpp
build/mb/mb.o: constants.h utils/json11/json11.hpp mb/gate.h mb/mb.h mb/mb.cpp
	g++ -c ${CPPFLAGS} -o $@ mb/mb.cpp
build/evolution/individual.o: constants.h evolution/individual.h evolution/individual.cpp
	g++ -c ${CPPFLAGS} -o $@ evolution/individual.cpp
build/individualMB.o: utils/json11/json11.hpp mb/gate.h mb/mb.h constants.h evolution/individual.h individualMB.h individualMB.cpp
	g++ -c ${CPPFLAGS} -o $@ individualMB.cpp
build/main.o: constants.h utils/strutil.h evolution/individual.h evolution/evolver.h evolution/evolverMAFPO.h main.cpp
	g++ -c ${CPPFLAGS} -o $@ main.cpp
build/tester.o: constants.h utils/strutil.h evolution/individual.h evolution/evolver.h evolution/evolverMAFPO.h tester.cpp
	g++ -c ${CPPFLAGS} -o $@ tester.cpp

tests/evolution/individual: constants.h evolution/individual.h evolution/individual.cpp tests/evolution/individual.cpp
	g++ ${CPPFLAGS} -o $@ $^
tests/evolution/evolver: constants.h evolution/individual.h evolution/individual.cpp evolution/evolver.h tests/evolution/evolver.cpp
	g++ ${CPPFLAGS} -o $@ $^
tests/evolution/evolverMAFPO: constants.h evolution/individual.h evolution/individual.cpp evolution/evolver.h evolution/evolverMAFPO.h tests/evolution/evolverMAFPO.cpp
	g++ ${CPPFLAGS} -o $@ $^
tests/mb/gate: utils/mbreftypes.h utils/randutils.h utils/bitutils.h mb/gate.h utils/json11/json11.hpp utils/json11/json11.cpp tests/mb/gate.cpp
	g++ ${CPPFLAGS} -o $@ $^
tests/mb/mb: constants.h mb/mb.h mb/mb.cpp mb/gate.h utils/mbreftypes.h utils/randutils.h utils/bitutils.h utils/json11/json11.cpp tests/mb/mb.cpp
	g++ ${CPPFLAGS} -o $@ $^
tests/individualMB: constants.h individualMB.h individualMB.cpp evolution/individual.h evolution/individual.cpp mb/mb.h mb/mb.cpp mb/gate.h utils/json11/json11.cpp tests/individualMB.cpp
	g++ ${CPPFLAGS} -o $@ $^

clean:
	rm -f ${BINARY} ${BINARY}.note build/*.o build/evolution/*.o
clean-tests:
	rm -f tests/evolution/individual tests/evolution/evolver tests/evolution/evolverMAFPO tests/mb/gate tests/mb/mb tests/individualMB
