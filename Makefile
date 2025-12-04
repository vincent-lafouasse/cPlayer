NAME = cPlayer

BUILD_DIR = ./build
INSTALL_PREFIX = $(BUILD_DIR)/bin
FULL_PATH = $(INSTALL_PREFIX)/$(NAME)

.PHONY: all
all: build

.PHONY: buildsystem
buildsystem:
	cmake -B $(BUILD_DIR) -G Ninja

.PHONY: build
build: buildsystem
	cmake --build build

.PHONY: run
run: build
	$(FULL_PATH)

.PHONY: re
re: fclean build

.PHONY: qtest
qtest: buildsystem
	cmake --build build
	GTEST_COLOR=1 ctest --test-dir build $(CTEST_OPT)

.PHONY: test
ifneq ($(TEST_WITH_MEMCHECK),)
test: CTEST_OPT += -T memcheck
endif
test: CTEST_OPT += -V
test: qtest


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
.PHONY: b c r fc t qt fmt
b: build
c: clean
fc: fclean
r: run
t: test
qt: qtest
fmt: format

-include $(DEPS)
