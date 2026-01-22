#
#
CPP = gcc
CPPFLAGS = -Wall -Wextra -Werror -I$(INCDIR)
MAKEFLAGS += -s

INCDIR = ./inc
SRCDIR = ./src
OBJDIR = ./obj
SRCS = $(SRCDIR)/main.c

OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

NAME = memprof

$(NAME): $(OBJDIR) $(OBJS)
		@printf "[.]   📦 Compiling '\033[33m$(NAME)\033[0m'...\r"
		$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
		@echo "🚀 '\033[33m$(NAME)\033[0m' compiled \033[32msuccessfully\033[0m!"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
		$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR):
		mkdir -p $(OBJDIR)

all: $(NAME)

wrapper: tests/tests.c
	gcc -shared -fPIC -o utils/wrapper.so utils/wrapper.c -ldl
	gcc -o tests/tests tests/tests.c

tests: wrapper
	LD_PRELOAD=./utils/wrapper.so ./tests/tests

clean:
		rm -f $(OBJDIR)/*.o
		rm -rf $(OBJDIR)
		rm -f utils/wrapper.so

fclean: clean
		@printf "[.]   🧹 Removing '\033[33m$(NAME)\033[0m' build...\r"
		rm -f $(NAME)
		printf "[✅]  🧹 Removed '\033[33m$(NAME)\033[0m' build...  \n"

re: fclean all

.PHONY: all clean fclean re