COMP  = g++
FLAG  = -c -Wall -O2
LINK  = -lSDL2 -lSDL2_image
SRCS := $(wildcard *.cpp) $(wildcard **/*.cpp) $(wildcard */*/*.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))
EXEC  = SutranAI

all : $(SRCS) $(EXEC)

$(EXEC) : $(OBJS)
	$(COMP) $(OBJS) $(LINK) -o $@

clean:
	rm -rf *.o */*.o */*/*.o
	rm $(EXEC)

.cpp.o :
	$(COMP) $(FLAG) $< -o $@
