CC=clang++
OBJ=obj
SRC=src

SRCS=$(shell find $(SRC) -type f -name '*.cpp')
OBJS=$(foreach obj,$(SRCS:src/%=%),$(OBJ)/$(basename $(obj)).o)

CFLAGS=-Iinclude/ -Wall -std=c++20
DEBUGCFLAGS=$(CFLAGS) -g3
RELEASECFLAGS=$(CFLAGS) -Ofast

TARGET=bin/ktool

DEBUGTARGET=bin/ktool_debug
DEBUGOBJS=$(foreach obj,$(SRCS:src/%=%),$(OBJ)/$(basename $(obj)).d.o)

.PHONY: all clean debug

$(TARGET): $(OBJS)
	@mkdir -p $(dir $(TARGET))
	@$(CC) $^ -o $@
	@strip $(TARGET)

clean:
	@find obj -name '*.o' -type f -delete
	@$(RM) $(TARGET)

debug_clean:
	@find obj -name '*.d.o' -type f -delete
	@$(RM) $(TARGET)

debug: $(DEBUGTARGET)

$(DEBUGTARGET): $(DEBUGOBJS)
	@mkdir -p $(dir $(DEBUGTARGET))
	@$(CC) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CC) $(RELEASECFLAGS) -Iinclude/$(shell dirname $(<:src/%=%)) -c $< -o $@

$(OBJ)/%.d.o: $(SRC)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CC) $(DEBUGCFLAGS) -Iinclude/$(shell dirname $(<:src/%=%)) -c $< -o $@

print_info:
	@echo $(OBJS)
	@echo $(SRCS)
