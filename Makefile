NAME = cPlayer

BUILD_DIR = ./build
INSTALL_PREFIX = $(BUILD_DIR)/src
FULL_PATH = $(INSTALL_PREFIX)/$(NAME)

.PHONY: all
all: build

.PHONY: build
build:
	cmake -B $(BUILD_DIR) -G Ninja
	cmake --build build --target $(NAME)

.PHONY: run
run: build
	$(FULL_PATH)

.PHONY: re
re: fclean build

.PHONY: test
test: build
	cmake --build build
	GTEST_COLOR=1 ctest --test-dir build $(CTEST_OPT)

.PHONY: vtest
ifneq ($(TEST_WITH_MEMCHECK),)
vtest: CTEST_OPT += -T memcheck
endif
vtest: CTEST_OPT += -V
vtest: test


.PHONY: clean
clean:
	rm -rf $(FULL_PATH)

.PHONY: fclean
fclean: clean
	rm -rf $(BUILD_DIR)

.PHONY: format
format:
	clang-format -i $(shell find src -name '*.c' -or -name '*.h')
	clang-format -i $(shell find lib/FileReader -name '*.c' -or -name '*.h')
	clang-format -i $(shell find test -name '*.cpp' -or -name '*.hpp' -or -name '*.h')

# aliases
.PHONY: b c r fc t vt fmt
b: build
c: clean
fc: fclean
r: run
t: test
vt: vtest
fmt: format

-include $(DEPS)
