all:
	$(MAKE) -C src
	src/a.out 'console.log("Hello world")'
	src/a.out 'echo("this is nice")'
	src/a.out 'echo("this is err") > /dev/stderr'
