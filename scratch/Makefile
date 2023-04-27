


all:
	gcc -g mini.c
	echo '{a=1;print(a+2);}' | ./a.out | grep 3
	echo '{a=1024;print(a-2);}' | ./a.out | grep 1022
	echo '{a="Hello world";}' | ./a.out | grep 'STR "Hell'
	echo '{print("Hello world");}' | ./a.out | grep ^Hello
	echo '{system("ls /");}' | ./a.out
