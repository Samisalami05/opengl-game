CC := gcc
NAME := main
SOURCEDIR := src
BUILDDIR := build
LIBSDIR := libs
TESTDIR := tests

MAIN_OBJ := $(BUILDDIR)/$(SOURCEDIR)/main.o

LIBS := libs/glad/glad.c
PROG_SRCS := $(shell find $(SOURCEDIR) -name '*.c')
SRCS := $(PROG_SRCS) $(LIBS)
OBJS := $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))

TEST_SRCS := $(shell find $(TESTDIR) -name '*.c')
TEST_OBJS := $(patsubst %.c,$(BUILDDIR)/%.o,$(TEST_SRCS)) 
TEST_APP_OBJS := $(filter-out $(MAIN_OBJ),$(OBJS))

CFLAGS := -Wall
INCLUDES := -Isrc -Ilibs/glfw-3.4/include -Ilibs/glad/include
LDLIBS := -lm -lglfw -ldl -lGL

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDLIBS)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -MMD $(CFLAGS) $(INCLUDES) -c $< -o $@

run: $(NAME)
	./$(NAME)

runtests: $(TEST_OBJS) $(TEST_APP_OBJS)
	$(CC) $^ -o runtests $(LDLIBS)

test: runtests
	./runtests
	

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(NAME)

-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)
