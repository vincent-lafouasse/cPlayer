NAME = cPlayer

BUILD_DIR = ./build
INSTALL_PREFIX = $(BUILD_DIR)/src

.PHONY: all
all: build

.PHONY: build
build: $(INSTALL_PREFIX)/$(NAME)

.PHONY: run
run: build
	$(INSTALL_PREFIX)/$(NAME)

$(INSTALL_PREFIX)/$(NAME):
	cmake -B $(BUILD_DIR) -G Ninja
	cmake --build build --target $(NAME)

.PHONY: re
re: fclean build

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# aliases
.PHONY: b c r
b: build
c: clean
r: run

-include $(DEPS)
