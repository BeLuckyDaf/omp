BUILD_DIR := build

SRCS := main.c netpbm.c sobel.c
OBJS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))
CLIBS := -pthread -lm
CC := gcc

.PHONY: netpbm-sobel
netpbm-sobel: $(BUILD_DIR)/netpbm-sobel

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
$(BUILD_DIR)/netpbm-sobel: $(OBJS)
	$(CC) $^ -o $@ $(CLIBS)

clean:
	rm -rf ./$(BUILD_DIR)/*.o
