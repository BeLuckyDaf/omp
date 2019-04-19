BUILD_DIR := build

SRCS := main.c netpbm.c sobel.c
OBJS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))
CLIBS := -pthread -lm
CC := gcc

.PHONY: program
program: $(BUILD_DIR)/program

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.

# CREATING THE BUILD DIRECTORY
$(BUILD_DIR)/.:
	mkdir -p $@

.SECONDEXPANSION:
	
# COMPILING SOURCE FILES TO OBJECTS
$(BUILD_DIR)/%.o: src/%.c | $$(@D)/.
	$(CC) -c $< -o $@

$(BUILD_DIR)/%.o: ./%.c | $$(@D)/.
	$(CC) -c $< -o $@
	
# LINKING THE OBJECTS INTO AN EXECUTABLE
$(BUILD_DIR)/program: $(OBJS)
	$(CC) $(CLIBS) $^ -o $@

clean:
	rm -rf ./$(BUILD_DIR)/*.o
