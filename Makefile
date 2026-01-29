#
#
CPP = gcc
CPPFLAGS = -Wall -Wextra -Werror -I$(INCDIR)
MAKEFLAGS += -s

INCDIR = ./inc
SRCDIR = ./src
OBJDIR = ./obj
SRCS = $(SRCDIR)/main.c $(SRCDIR)/init.c

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

wrapper:
	gcc -shared -fPIC -o utils/wrapper.so utils/wrapper.c -ldl

# Build instrumented binary from the hard-coded tests directory
traceable:
	./utils/run_instrumented.sh ./tests trace_tests -Wall -Wextra -g

# Run the instrumented binary
run_tests: traceable
	./trace_tests


# Remove TRACK_ASSIGN instrumentation from the hard-coded tests directory
clean_trace:
	@find ./tests -type f -name "*.c" -exec sed -i 's/TRACK_ASSIGN(\([^,]*\),\(.*\));/\1 =\2;/g' {} +
	@echo "Reverted TRACK_ASSIGN back to regular assignments in ./tests"

clean:
		rm -f $(OBJDIR)/*.o
		rm -rf $(OBJDIR)
		rm -f utils/wrapper.so

fclean: clean
		@printf "[.]   🧹 Removing '\033[33m$(NAME)\033[0m' build...\r"
		rm -f $(NAME)
		printf "[✅]  🧹 Removed '\033[33m$(NAME)\033[0m' build...  \n"

re: fclean wrapper all

.PHONY: all wrapper run_tests traceable clean_trace clean fclean re