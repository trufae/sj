# $(CC) -D CONFIG_BIGNUM=1 -o a.out main.c qjs/src/*.c
# $(CC) -o b.out main.c qsj/*.c

ifeq ($(shell ls boilerplate.h),)
HAVE_BOILERPLATE=0
else
HAVE_BOILERPLATE=0
endif

all:
	-sj boilermaker.sj
	$(CC) -g -o sj main.c qsj/*.c
	ls -l sj

release rls:
	$(CC) -o sj main.c qsj/*.c
	sj boilermaker.sj
	$(CC) -Oz -flto -DHAVE_BOILERPLATE=1 -o sj main.c qsj/*.c
