CPP=g++
CFLAGS= -c -Wall -std=c++14

OBJDIR=obj
SRCS=$(wildcard *.cpp)
OBJS=$(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))

TARGET=stm32prog

.PHONY: all clean

all: $(OBJDIR)/ $(TARGET)
	ctags -R .

$(OBJDIR)/:
	mkdir $@

$(TARGET): $(OBJS)
	$(CPP) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	$(CPP) $(CFLAGS) -o $@ $^


clean:
	$(RM) $(OBJDIR)/*.o $(TARGET)
