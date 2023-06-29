INC := -I ../src/core
LDFLAGS += -L ../lib
LIBS := -lgrackle

all: main

main: main.cpp
	$(CXX) -g $(INC) $^ -o $@ $(LDFLAGS) $(LIBS)

clean: ; rm -rf main