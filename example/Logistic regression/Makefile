# tool marcros
CC := g++
CCFLAG := -std=gnu++2a#-std=c++17 #-std=gnu++17
DBGFLAG := -g
CCOBJFLAG := -c
INCLUDEFLAG := -I
LINKFLAG := -L
LIBFLAG := -l

# path marcros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := .
DBG_PATH := debug
INCLUDE_PATH := G:/repository/MetaAutoDiff D:/eigen-eigen-323c052e1731/Eigen D:/eigen-eigen-323c052e1731/unsupported/Eigen

# compile maroros
TARGET_NAME := main
ifeq ($(OS), Windows_NT)
    TARGET_NAME := $(addsuffix .exe, $(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME) 
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)

# src file & obj file
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*, .c*)));
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
INCLUDE := $(addprefix $(INCLUDEFLAG) , $(INCLUDE_PATH))

# clean file list
DISTCLEAN_LIST := $(OBJ)\
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET)\
              $(TARGET_DEBUG)\
			  $(DISTCLEAN_LIST)

# default rule
default:all
# non-phony target
$(TARGET):$(SRC)
	$(CC) $(CCFLAG) -o $@ $? $(INCLUDE)

$(OBJ_PATH)/.%o:$(SRC_PATH)/.%cpp
	$(CC) $(CCOBJFLAG) -o $@ $< $(INCLUDE)



.phony:all
all:$(TARGET)





