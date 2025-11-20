PROJECT := Linearity

# Directory Structure
EXE_DIR:=main
SRC_DIR:=src
INC_DIR:=include
OBJ_DIR:=obj
LIB_DIR:=lib

EXE     := $(notdir $(wildcard $(EXE_DIR)/*.cpp))
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(filter-out $(INC_DIR)/LinkDef.h, $(wildcard $(INC_DIR)/*.h))
LINKDEF := $(notdir $(wildcard $(INC_DIR)/*LinkDef.h))
OBJECTS := $(subst $(SRC_DIR),$(OBJ_DIR),$(SOURCES:.cpp=.o))
SRCFILES := $(notdir $(SOURCES))
OBJFILES := $(notdir $(OBJECTS))
FILES := $(notdir $(basename $(OBJECTS)))


LIB:=-L /lib/x86_64-linux-gnu/ -L../../coda3-decoder/
INC:=-I./ -I./include -I../../coda3-decoder
ROOT:=`root-config --glibs --cflags`

DICT_NAME:=$(LIB_DIR)/dict.cc
LIB_NAME:= $(LIB_DIR)/lib$(PROJECT).so

CC:=g++ -g
CXXFLAGS:=-Wall -Wextra -pedantic
LFLAGS:= -l$(PROJECT) -lboost_program_options -lboost_regex

all: $(OBJECTS) $(LIB_NAME) $(EXE)

verbose: .PHONY all


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "BUILDING OBJECT FILES: $@"
	@$(CC) -fPIC -c -o $@ $^ $(INC) $(ROOT)

$(DICT_NAME): $(HEADERS) $(INC_DIR)/$(LINKDEF)
	@echo "BUILDING ROOT DICTIONARY: $@"
	@rootcling -f $@ -p $(notdir $(HEADERS)) $(INC) $(LINKDEF) -v

$(LIB_NAME): $(DICT_NAME) $(OBJECTS)
	@echo "BUILDING SHARED LIBRARY: $@"
	@$(CC) -shared -fPIC -o $@ $^ $(ROOT) $(CXXFLAGS) $(INC) $(LIB) lib/Dict.cxx

$(EXE): $(LIB_NAME)
	@echo "BUILDING EXECUTABLE: $(basename $@)"
	@$(CC) $(EXE_DIR)/$@ -o $(basename $@) -L$(LIB_DIR) $(LFLAGS) $(INC) $(LFLAGS) $(CXXFLAGS) $(ROOT) -Wl,-rpath $(LIB_DIR)

clean:
	-rm $(LIB_DIR)/*
	-rm $(OBJ_DIR)/*

.PHONY: PRINT
PRINT: 
	$(info SOURCES = $(SOURCES))
	$(info HEADERS = $(HEADERS))
	$(info OBJECTS = $(OBJECTS))
	$(info SRCFILES= $(SRCFILES))
	$(info OBJFILES= $(OBJFILES))
	$(info FILES   = $(FILES))
	$(info LINKDEF = $(LINKDEF))
	$(info DICT = $(DICT_NAME))
	$(info LIB = $(LIB_NAME))
	$(info EXE = $(EXE))

