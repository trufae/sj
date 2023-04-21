#!/usr/local/bin/sj

// eval("global.apiTest = 123;");
const data = slurp("api.sj");
try {
eval (data.trim());
apiTest();
} catch (e) {
	echo (e);
}

echo ("pos")

// this is new
echo ("Hello".cyan);
echo(Shell.ls)
true || die("oops");
slurp()
echo(slurp("../README.md"))
slurp()

echo("THIS IS RED".red);
echo("THIS IS GREEN".green);
echo("THIS IS YELLOW".yellow);
echo("THIS IS BLUE".blue);

exit(0);

var a = $("|ls");
echo("Files: " + a);

echo(red("dirs"));

for (let a of ls.directories("/")) {
	echo("d-> " + a);
}
echo("files");
for (let f of ls.files()) {
	echo("f-> " + f);
}
echo("hidden");
for (let f of ls.hidden()) {
	echo("h-> " + f);
}

exit(0);

pushd ("/bin");
pwd();
function foo() {
	echo("pwny");
	Shell.ls("/")
	Shell.ls("/foo")
	Shell.ls("/bar")
}

foo();

echo (Shell.data);
