const global = globalThis;

class Shell {
	static prompt(newPrompt) {
		if (newPrompt) {
			Shell.promptDelegate = eval(newPrompt);
		}
		return Shell.promptDelegate();
	}
	static pwd() {
		return slurp('|pwd').trim();
	}
	static oldpwd = [];
	static ls(path = ".", options = "") {
		return slurp("|ls "+options +" " + path).trim();
	}
	static home() {
		return env("HOME");
	}
	static pushd(path) {
		if (chdir(path)) {
			Shell.oldpwd.push(pwd());
		}
	}
	static popd() {
		if (Shell.oldpwd.length == 0) {
			return false;
		}
		const d = Shell.oldpwd.pop();
		if (d) {
			chdir(d);
		}
	}
	static $(cmd) {
		return slurp("|" + cmd);
	}
	static cat(filename) {
		if (filename[0] == '|') {
			return "";
		}
		return slurp(filename);
	}
}
Shell.ls.files = function(path) {
	return Shell.ls(path, "-F").split(/\n/g).filter((x) => {
			return (!x.endsWith("/") && !x.endsWith("@"));
		});
}
Shell.ls.hidden = function(path) {
	return Shell.ls(path, "-a").split(/\n/g).filter((x) => x.startsWith("."));
}
Shell.ls.directories = function(path) {
	return Shell.ls(path, "-F").split(/\n/g).filter((x) => x.endsWith("/"));
}
env.entries = function() {
	const e = {};
	const tickle = (line) => {
		const [k, v] = line.split("=", 2);
		e[k] = v;
	};
	env().split("\n").forEach(tickle);
	return e;
}
function sj_init() {
const colors = {
	"red": "31m",
	"green": "32m",
	"yellow": "33m",
	"white": "38m",
	"blue": "34m",
	"magenta": "35m",
	"cyan": "36m",
	"reset": "0m",
};
for (let k of Object.keys(colors)) {
	Object.defineProperty(String.prototype, k, {
		get() {
			return "\x1b[" + colors[k] + this + "\x1b[0m";
		}
	});
}
String.prototype.grep = function(w) {
	const s = this;
	return s.split(/\n/g).filter((x)=>{return x.indexOf(w)!= -1;}).join('\n');
}
	Object.defineProperty(Shell.prototype, "ls", {
		get() {
			system("ls");
		}
	});
}
sj_init();
const clear = () => { echo('\x1b[2J\x1b[0;0m'); }
const gotoxy = (x,y) => { echo('\x1b[2J\x1b[0;0m'); }
const $ = system;
const cd = function(arg) {
	if (!arg) {
		return chdir(home());
	}
	if (arg == "-") {
		return Shell.popd();
	}
	return chdir(arg);
}

const { prompt, home, pushd, popd, pwd, ls, cat } = Shell;
/* MAIN */
