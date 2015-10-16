#
# 刘新国 2015年3月
#
BUILD := build/#
SRC   := src/#
CC := gcc
CXX := g++
ver = debug#
ifeq ($(ver), debug)
	TARGET  := MeshView-d#
	OBJDir  := $(BUILD)debug/
	CCFLAGS := -pthread -g -Wall
else
	TARGET  := MeshView#
	OBJDir  := $(BUILD)release/
	CCFLAGS := -o3 -Wall
endif
LIBS := -lpthread -lglut -lGL -lGLU -lglut -lm 
TARGET  := $(BUILD)$(TARGET)
CPPFILE := $(wildcard $(SRC)*.cpp)
CPPOBJS := $(patsubst %.cpp, %.o, $(CPPFILE))
CPPDEPS := $(patsubst %.o, %.d, $(CPPOBJS))
CFILE   := $(wildcard $(SRC)*.c)
COBJS   := $(patsubst %.c, %.o, $(CFILE))
CDEPS   := $(patsubst %.o, %.d, $(COBJS))
OBJS    := $(addprefix $(OBJDir), $(CPPOBJS) $(COBJS))
DEPS    := $(addprefix $(OBJDir), $(CPPDEPS) $(CDEPS))

$(TARGET) : $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

.PHONY : test cleanobj cleandep clean

cleanobj :
	rm -f $(OBJS)
cleandep :
	rm -f $(DEPS)
clean: cleanobj cleandep
	rm -f $(TARGET)
	rm -r $(OBJDir)

$(OBJDir)src/ply.o: src/ply.c
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	$(CC) -c -o $@ $<

# Compile .c
$(OBJDir)%.o: %.c
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	$(CC) -c $(CCFLAGS) -o $@ $<
# Compile .cpp
$(OBJDir)%.o: %.cpp
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	$(CC) -c $(CCFLAGS) -o $@ $<
# c file dependence
$(OBJDir)%.d: %.c
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	gcc -MM -MT '$(patsubst %.c, $(OBJDir)%.o, $<) $@' $< > $@
# cpp file dependence
$(OBJDir)%.d: %.cpp
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	gcc -MM -MT '$(patsubst %.cpp, $(OBJDir)%.o, $<) $@' $< > $@

test:
	echo $(BUILD)$(BUILD)
	echo $(ROOT)

-include $(DEPS)

