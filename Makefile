CC=clang++
OBJ=obj
SRC=src

SRCS=$(shell find $(SRC) -type f -name '*.cpp')
OBJS=$(foreach obj,$(SRCS:src/%=%),$(OBJ)/$(basename $(obj)).o)

CFLAGS=-Iinclude/ -Wall -std=c++17
DEBUGCFLAGS=$(CFLAGS) -g3
RELEASECFLAGS=$(CFLAGS) -Ofast

TARGET=bin/stool

DEBUGTARGET=bin/stool_debug
DEBUGOBJS=$(foreach obj,$(SRCS:src/%=%),$(OBJ)/$(basename $(obj)).d.o)

.PHONY: all clean debug

$(TARGET): $(OBJS)
	@mkdir -p $(dir $(TARGET))
	@$(CC) $^ -o $@
	@strip $(TARGET)

clean:
	@$(RM) -rf $(OBJ)
	@$(RM) -rf $(dir $(TARGET))

debug: $(DEBUGTARGET)

$(DEBUGTARGET): $(DEBUGOBJS)
	@mkdir -p $(dir $(DEBUGTARGET))
	@$(CC) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(shell dirname $@)
	@$(CC) $(RELEASECFLAGS) -Iinclude/$(shell dirname $(<:src/%=%)) -c $< -o $@

$(OBJ)/%.d.o: $(SRC)/%.cpp
	@mkdir -p $(shell dirname $@)
	@$(CC) $(DEBUGCFLAGS) -Iinclude/$(shell dirname $(<:src/%=%)) -c $< -o $@

print_info:
	@echo $(OBJS)
	@echo $(SRCS)
