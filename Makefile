CC := gcc
NAME := main
SOURCEDIR := src
BUILDDIR := build
LIBSDIR := libs

LIBS := libs/glad/glad.c
PROG_SRCS := $(shell find $(SOURCEDIR) -name '*.c')
SRCS := $(PROG_SRCS) $(LIBS)
OBJS := $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))

CFLAGS := -Wall
INCLUDES := -Isrc2 -Ilibs/glfw-3.4/include -Ilibs/glad/include
LDLIBS := -lm -lglfw -ldl -lGL

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDLIBS)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD $(CFLAGS) $(INCLUDES) -c $< -o $@

run: $(NAME)
	./$(NAME)


.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(NAME)

-include $(OBJS:.o=.d)
