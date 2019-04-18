BUILD_DIR := build

SRCS := main.c netpbm.c sobel.c
OBJS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))
CLIBS := -pthread -lm

.PHONY: program
program: $(BUILD_DIR)/program

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@

.SECONDEXPANSION:

$(BUILD_DIR)/%.o: src/%.c | $$(@D)/.
	$(CC) -c $< -o $@

$(BUILD_DIR)/%.o: ./%.c | $$(@D)/.
	$(CC) -c $< -o $@

$(BUILD_DIR)/program: $(OBJS)
	$(CC) $(CLIBS) $^ -o $@

clean:
	rm -rf ./$(BUILD_DIR)/*.o