CC = g++

SRC_DIR = src
HDR_DIR = headers
OUT_DIR = obj
BIN_DIR = bin

CFLAGS = -I $(HDR_DIR) -Werror -Wall -Wpedantic

SRC = 	$(wildcard $(SRC_DIR)/*.cpp)
HDR = 	$(wildcard $(HDR_DIR)/*.h)
_OUT = 	$(patsubst $(SRC_DIR)/%,$(OUT_DIR)/%,$(SRC:.cpp=.o))
OUT = 	$(filter-out $(OUT_DIR)/main.o, $(_OUT))

_HDR := $(OUT:.o=.d)
-include _HDR

TARGET = MC_RCON_Client

.PHONY: clean

all: $(TARGET)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Creating Object file $@ from $<..."
	+@[ -d $(OUT_DIR) ] || mkdir -p $(OUT_DIR)
	$(CC) -MMD $(CFLAGS) -c -o $@ $<
	@echo "Done"

$(TARGET): $(OUT) $(OUT_DIR)/main.o
	@echo "Building $@ binary..."
	+@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	@echo $(SRC)
	$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS)
	@echo "Done"

clean:
	@echo "Cleaning Build Folders..."
	@rm -f $(OUT_DIR)/*
	@rm -f $(BIN_DIR)/$(TARGET)
	@echo "Done"