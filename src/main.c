#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
// #include "qjs/src/quickjs.h"
#include "qsj/quickjs.h"
#include "qsj/cutils.h"

#if HAVE_BOILERPLATE
#include "boilerplate.h"
#endif

typedef struct {
	JSRuntime *rt;
	JSContext *ctx;
	bool repl;
} SJ;

static char *read_file(const char *fn) {
	char buf[4096] = {0};
	if (!fn) {
		fgets (buf, sizeof (buf) - 1, stdin);
	} else if (*fn == '|') {
		FILE *fd = popen (fn + 1, "r");
		if (!fd) {
			return NULL;
		}
		fread (buf, sizeof (buf), 1, fd);
		pclose (fd);
	} else {
		FILE *fd = fopen (fn, "r");
		if (!fd) {
			return NULL;
		}
		fread (buf, sizeof (buf), 1, fd);
		fclose (fd);
	}
	return strdup (buf);
}

static JSValue sj_dump(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
	JSRuntime *rt = JS_GetRuntime (ctx);
	size_t plen, plen2;
	char *ret = NULL;
	if (argc == 2) {
		const char *n = JS_ToCStringLen2 (ctx, &plen, argv[0], false);
		const char *n2 = JS_ToCStringLen2 (ctx, &plen2, argv[1], false);
		FILE *fd = fopen (n, "wb");
		if (fd) {
			fwrite (n2, strlen (n2), 1, fd);
			fclose (fd);
			return JS_NewBool (ctx, true);
		}
	}
	return JS_NewBool (ctx, false);
}

static JSValue sj_slurp(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
	JSRuntime *rt = JS_GetRuntime (ctx);
	size_t plen;
	char *ret = NULL;
	if (argc < 1) {
		ret = read_file (NULL);
	} else {
		const char *n = JS_ToCStringLen2 (ctx, &plen, argv[0], false);
		if (argc > 0 && n && *n) {
			ret = read_file (n);
		}
	}
	if (ret != NULL) {
		return JS_NewString (ctx, ret);
	}
	return JS_NewBool (ctx, false);
}

static JSValue sj_system(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
	JSRuntime *rt = JS_GetRuntime (ctx);
	size_t plen;
	const char *n = JS_ToCStringLen2 (ctx, &plen, argv[0], false);
	char *ret = NULL;
	if (n && *n) {
		system (n);
		// ret = k->core->lang->cmd_str (k->core, n);
	}
	// JS_FreeValue (ctx, argv[0]);
	return JS_NewString (ctx, ret? ret: "");
}

static JSValue sj_chdir(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
	JSRuntime *rt = JS_GetRuntime (ctx);
	size_t plen;
	const char *n = JS_ToCStringLen2 (ctx, &plen, argv[0], false);
	char *ret = NULL;
	if (n && *n) {
		if (chdir (n) == 0) {
			return JS_NewBool (ctx, true);
		}
	}
	return JS_NewBool (ctx, false);
}
static JSValue sj_env(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
	JSRuntime *rt = JS_GetRuntime (ctx);
	size_t plen, plen2;
	char *ret = NULL;
	if (argc > 0) {
		const char *n = JS_ToCStringLen2 (ctx, &plen, argv[0], false);
		if (n && *n) {
			if (argc > 1) {
				const char *n2 = JS_ToCStringLen2 (ctx, &plen2, argv[1], false);
				if (n2 && *n2) {
					setenv (n, n2, 1);
					return JS_NewBool (ctx, true);
				}
				return JS_NewBool (ctx, false);
			}
			ret = getenv (n);
			return JS_NewString (ctx, ret? ret: "");
		}
		return JS_NewBool (ctx, false);
	}
	ret = read_file("|env");
	JSValue v = JS_NewString (ctx, ret? ret: "");
	free (ret);
	return v;
}

static JSValue sj_exit(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
	JSRuntime *rt = JS_GetRuntime (ctx);
	if (argc > 0) {
		// we should pick a number, not a string
		size_t plen;
		const char *n = JS_ToCStringLen2 (ctx, &plen, argv[0], false);
		char *ret = NULL;
		if (n && *n) {
			exit (atoi (n));
		}
	}
	exit (0);
	return JS_NewBool (ctx, false);
}

static JSValue sj_echo(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
	JSRuntime *rt = JS_GetRuntime (ctx);
	if (argc == 0) {
		return JS_NewBool (ctx, false);
	}
	int i;
	for (i = 0; i < argc; i++) {
		// TODO: loop and print them all
		size_t plen;
		const char *n = JS_ToCStringLen2 (ctx, &plen, argv[i], false);
		char *ret = NULL;
		if (n && *n) {
			puts (n);
			// ret = k->core->lang->cmd_str (k->core, n);
		}
	}
	return JS_NewBool (ctx, false);
}

static const JSCFunctionListEntry funcs[] = {
	JS_CFUNC_DEF ("system", 1, sj_system), // $
	JS_CFUNC_DEF ("chdir", 1, sj_chdir), // cd
	JS_CFUNC_DEF ("echo", 1, sj_echo),
	JS_CFUNC_DEF ("exit", 1, sj_exit),
	JS_CFUNC_DEF ("dump", 1, sj_dump),
	JS_CFUNC_DEF ("env", 1, sj_env),
	JS_CFUNC_DEF ("slurp", 1, sj_slurp),
};

static SJ sj_new (void) {
	SJ sj = {0};
	sj.rt = JS_NewRuntime ();
	sj.ctx = JS_NewContext (sj.rt);
	JSValue global_obj = JS_GetGlobalObject (sj.ctx);
	JS_SetPropertyFunctionList (sj.ctx, global_obj, funcs, countof(funcs));
	return sj;
}

static void r2qjs_dump_obj(JSContext *ctx, JSValueConst val) {
	const char *str = JS_ToCString (ctx, val);
	if (str) {
		printf ("%s\n", str);
		JS_FreeCString (ctx, str);
	} else {
		printf ("[exception]\n");
	}
}
static void js_std_dump_error1(JSContext *ctx, JSValueConst exception_val) {
	JSValue val;
	bool is_error;

	is_error = JS_IsError (ctx, exception_val);
	r2qjs_dump_obj (ctx, exception_val);
	if (is_error) {
		val = JS_GetPropertyStr (ctx, exception_val, "stack");
		if (!JS_IsUndefined (val)) {
			r2qjs_dump_obj (ctx, val);
		}
		JS_FreeValue (ctx, val);
	}
}

void js_std_dump_error(JSContext *ctx) {
	JSValue exception_val;
	exception_val = JS_GetException (ctx);
	js_std_dump_error1 (ctx, exception_val);
	JS_FreeValue (ctx, exception_val);
}

static void sj_eval(SJ sj, const char *s) {
	int flags = JS_EVAL_TYPE_GLOBAL; //  | JS_EVAL_FLAG_STRICT; //  | JS_EVAL_TYPE_MODULE;
	char *space = strchr (s, ' ');
	JSValue v;
	if (sj.repl) {
		bool shellout = false;
		if (strchr (s, '|')) {
			shellout = true;
			char code[1024];
			snprintf (code, sizeof (code), "system(\"%s\")", s);
			v = JS_Eval (sj.ctx, code, strlen (code), "-", flags);
		}
		if (space && !strchr (s, '(')) {
			// split 
			*space++ = 0;
		}
		if (!shellout) {
			v = JS_Eval (sj.ctx, s, strlen (s), "-", flags);
		}
	} else {
		int sslen = strlen (s) + 64;
		char * ss = malloc (sslen);
		snprintf (ss, sslen, "try{%s;}catch(e){echo(e);}", s);
		v = JS_Eval (sj.ctx, ss, strlen (ss), "-", flags);
		free (ss);
	}
	if (JS_IsException (v)) {
		js_std_dump_error (sj.ctx);
		JSValue e = JS_GetException (sj.ctx);
		r2qjs_dump_obj (sj.ctx, e);
	} else if (sj.repl) {
		if (JS_IsFunction(sj.ctx, v)) {
			int sslen = strlen (s) + 64;
			char * ss = malloc (sslen);
			char *args = strdup ("");
			if (space) {
				free (args);
				args = malloc (1024);
				snprintf (args, 1024, "\"%s\"", space);
				// XXX
			}
			snprintf (ss, sslen, "try{%s(%s);}catch(e){echo(e);}", s, args);
			free (args);
			JS_FreeValue (sj.ctx, v);
			v = JS_Eval (sj.ctx, ss, strlen (ss), "-", flags);
			if (JS_IsException (v)) {
				js_std_dump_error (sj.ctx);
				r2qjs_dump_obj (sj.ctx, v);
			} else {
				r2qjs_dump_obj (sj.ctx, v);
			}
			return;
		} else {
			// print stuff
			r2qjs_dump_obj (sj.ctx, v);
		}
	}
	// print value if in repl mode
	JS_FreeValue (sj.ctx, v);
}

static void sj_free(SJ sj) {
	JS_FreeContext (sj.ctx);
// 	JS_FreeRuntime (sj.rt);
}

int main(int argc, char **argv) {
	SJ sj = sj_new();
	if (argc == 1) {
		sj.repl = true;
	}
	char *code = NULL;
	if (argc > 1 && !strcmp (argv[1], "-e")) {
		code = argv[2];
	}
	if (argc > 1 && !strcmp (argv[1], "-h")) {
		printf ("sj [-v|-e expr]\n");
		return 0;
	}
	if (argc > 1 && !strcmp (argv[1], "-v")) {
		printf ("0.0.1\n");
		return 0;
	}
	if (!code && !sj.repl && argc > 1) {
		code = read_file (argv[1]);
	}
#if HAVE_BOILERPLATE
	char *boilerplate = strdup (boilerplate_sj);
#else
	char *boilerplate = read_file ("boilerplate.sj");
	if (!boilerplate) {
		printf ("cannot read boilerplate.sj\n");
		return 1;
	}
#endif
	char *final = malloc (4096);
	if (sj.repl) {
		sj.repl = false;
		// printf ("%s\n", boilerplate);
		sj_eval (sj, boilerplate);
		sj.repl = true;
		while (true) {
			printf ("\x1b[33m> ");
			fflush (stdout);
			char *line = read_file (NULL);
			printf ("\x1b[0m");
			fflush (stdout);
			if (!line || !*line) {
				break;
			}
			line[strlen (line) - 1] = 0;
			if (*line == '!') {
				snprintf (final, 4096, "system('%s');", line + 1);
				sj_eval (sj, final);
			} else {
				sj_eval (sj, line);
			}
			free (line);
		}
	} else {
		if (code && *code == '#') {
			char *nl = strchr (code, '\n');
			if (nl) {
				code = nl + 1;
			} else {
				free (code);
				code = NULL;
			}
		}
		if (!code || !boilerplate) {
			return -1;
		}
		snprintf (final, 4096, "try{%s%s}catch(e){echo(e)}", boilerplate, code);
		sj_eval (sj, final);
	}
	sj_free (sj);
	// free (code);
	free (final);
	free (boilerplate);
}
