
CC = g++
CFLAGS = -O3
AR = ar crv

TARGET  = libflsengine.a
OBJ_DIR = ./obj
SRC_DIR = ./src
DEP_DIR = ./dep
INCLUDE_DIR = ./include
 
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, %.o, $(SRC))
 
vpath %.o $(OBJ_DIR)
vpath %.h $(INCLUDE_DIR)
vpath %.cpp $(SRC_DIR)
vpath %.d $(DEP_DIR)
 
all: $(TARGET)
 
$(TARGET) : create_dir $(OBJECTS)
	$(AR) $(TARGET) $(addprefix $(OBJ_DIR)/, $(OBJECTS))
 
%.o : %.cpp %.d
	$(CC) $(CFLAGS) -c $< -I$(INCLUDE_DIR) -o $(OBJ_DIR)/$@

%.d: %.cpp
	@set -e;\
	rm -f $(DEP_DIR)/$@;\
	$(CC) -M $(CFLAGS) -I$(INCLUDE_DIR) $< > $(DEP_DIR)/$@.$$$$;\
	sed 's,\($*\)\.o[ :]*,\1.o $(DEP_DIR)/$@ : ,g' < $(DEP_DIR)/$@.$$$$ > $(DEP_DIR)/$@;\
	rm -f $(DEP_DIR)/$@.$$$$

-include $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SRC))

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

create_dir:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(DEP_DIR)

.PHONY : clean
clean:
	rm -rf $(TARGET) $(OBJ_DIR)/*.o $(DEP_DIR)/*.d