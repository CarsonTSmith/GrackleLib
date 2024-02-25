INC := -I ../src/core
LDFLAGS += -L $(CURDIR)/../lib
LIBS := -lgrackle

all: main

main: main.cpp
	$(CXX) -g $(INC) $^ -o $@ $(LDFLAGS) $(LIBS)

clean: ; rm -rf main
