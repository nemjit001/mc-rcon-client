CC = g++

SRC_DIR = src
HDR_DIR = headers
OUT_DIR = obj
BIN_DIR = bin

CFLAGS = -I $(HDR_DIR) -Werror -Wall -Wpedantic
LIBS =  -pthread -lpthread

SRC = 	$(wildcard $(SRC_DIR)/*.cpp)
HDR = 	$(wildcard $(HDR_DIR)/*.h)
_OUT = 	$(patsubst $(SRC_DIR)/%,$(OUT_DIR)/%,$(SRC:.cpp=.o))
OUT = 	$(filter-out $(OUT_DIR)/main.o, $(_OUT))

_HDR := $(OUT:.o=.d)
-include _HDR

TARGET = MC_RCON_Client

.PHONY: clean

all: $(TARGET)
	@echo "Finished all tasks"

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Creating Object file $@ from $<..."
	+@[ -d $(OUT_DIR) ] || mkdir -p $(OUT_DIR)
	@$(CC) -MMD $(CFLAGS) -c -o $@ $< $(LIBS)
	@echo "Created $@"

$(TARGET): $(OUT) $(OUT_DIR)/main.o
	@echo "Building $@ binary..."
	+@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS) $(LIBS)
	@echo "Created $@ Binary"

clean:
	@echo "Cleaning Build Folders..."
	@rm -f $(OUT_DIR)/*
	@rm -f $(BIN_DIR)/$(TARGET)
	@echo "Done"