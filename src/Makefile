CC=gcc
CFLAGS=-Wall -Werror -Wextra -g
GCOV_FLAGS=-fprofile-arcs -ftest-coverage -lgcov -fsanitize=address
CHECK_FLAGS=-lcheck -lm -lpthread
ALL_FLAGS=$(CFLAGS) $(GCOV_FLAGS) $(CHECK_FLAGS)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
NAME = test
REPORT_NAME = report


all: clean s21_decimal.a gcov_report test

test: $(OBJS) ./tests/*.c 
	$(CC) $(CFLAGS) $(CHECK_FLAGS) ./tests/*.c  $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS) *.o *.gcno *.gcov *.gcda *.info *.a $(REPORT_NAME) $(NAME) check_res finish

s21_decimal.a: $(OBJS)
	ar rc s21_decimal.a $(OBJS) 
	ranlib s21_decimal.a

gcov_report: ./tests/*.c
#   checkmk test.check > ./tests/*.c main.c
	$(CC) -o $(NAME) $(SRCS) ./tests/*.c  $(ALL_FLAGS) 
	./$(NAME)
	geninfo *.gcda *.gcno -b . -o check_res 
	genhtml check_res -o finish
	open ./finish/index.html