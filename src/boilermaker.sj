
const res = makeBoilerplate("boilerplate.sj");
if (dump ("boilerplate.h", res)) {
	echo("Successfully created boilerplate.sj".green);
	exit(0);
} else {
	echo("Failed to compile".red);
	exit(1);
}

function makeBoilerplate(filename) {
	const data = slurp(filename);
	let res = "const char boilerplate_sj[] = \\\n";
	for(let line of data.split(/\n/g)) {
		const row = line.trim()
			.replace(/\\/g, '\\\\')
			.replace(/"/g, '\\\"');
		res += "\t\"" + row + "\\n\" \\\n";
	}
	return res + ";\n";
}
