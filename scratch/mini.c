/* 
 * <program>   := <statement>
 * <statement> := "if" <paren_expr> <statement>
 *             := "if" <paren_expr> <statement> "else" <statement> 
 *             := "while" <paren_expr> <statement> 
 *             := "do" <statement> "while" <paren_expr> ";" 
 *             := "print" <paren_expr> ";"
 *             := "system" <paren_expr> ";"
 *             := "{" { <statement> } "}" 
 *             := <expr> ";" 
 *             := ";"
 * 
 * <paren_expr> := "(" <expr> ")"
 * <expr>       := <test> 
 *              := <id> "=" <expr> 
 * <test>       := <sum> 
 *              := <sum> "<" <sum>
 * <sum>        := <term> 
 *              := <sum> "+" <term> 
 *              := <sum> "-"  <term> 
 * <term>       := <id>
 *              := <int> 
 *              := <paren_expr> 
 * <id>         := <a_finite_sequence_of_acceptable_symbols>
 * <num>        := <an_unsigned_decimal_integer> 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*----------------------------------------------------------------------------*/
/* Tokenizer */

enum {
	DO_SYM,
	ELSE_SYM,
	IF_SYM,
	WHILE_SYM,
	PRINT_SYM,
	SYSTEM_SYM,

	LBRA_SYM,
	RBRA_SYM,
	LPAR_SYM,
	RPAR_SYM,
	PLUS_SYM,
	DIV_SYM,
	MUL_SYM,
	MINUS_SYM,
	LESS_SYM,
	SEMI_SYM,
	EQUAL_SYM,
	NUM_SYM,
	STR_SYM,
	ID_SYM,
	EOI_SYM
};

const char *words[] = {"do", "else", "if", "while", "print", "system", NULL};

typedef struct {
	int ch;
	int num_val;
	int sym;
	const char *code;
	char id_name[100];
} State;

void syntax_error(State *s, char *msg) {
	// return error code instead
	fprintf(stderr, "syntax error - %s\n", msg);
	exit(1);
}
void next_ch(State *s) {
	if (s->code) {
		s->ch = *s->code;
		if (s->ch == 0) {
			s->ch = -1;
			s->code = NULL;
		} else {
			s->code++;
		}
	} else {
		s->ch = getchar();
	}
}

void next_sym(State *s) {
again:
	switch (s->ch) {
	case 0:
	case ' ':
	case '\t':
	case '\n':
		next_ch (s);
		goto again;
	case EOF:
		s->sym = EOI_SYM;
		break;
	case '{':
		next_ch(s);
		s->sym = LBRA_SYM;
		break;
	case '}':
		next_ch(s);
		s->sym = RBRA_SYM;
		break;
	case '(':
		next_ch(s);
		s->sym = LPAR_SYM;
		break;
	case ')':
		next_ch(s);
		s->sym = RPAR_SYM;
		break;
	case '/':
		next_ch(s);
		s->sym = DIV_SYM;
		break;
	case '*':
		next_ch(s);
		s->sym = MUL_SYM;
		break;
	case '+':
		next_ch(s);
		s->sym = PLUS_SYM;
		break;
	case '-':
		next_ch(s);
		s->sym = MINUS_SYM;
		break;
	case '<':
		next_ch(s);
		s->sym = LESS_SYM;
		break;
	case ';':
		next_ch(s);
		s->sym = SEMI_SYM;
		break;
	case '=':
		next_ch (s);
		s->sym = EQUAL_SYM;
		break;
#if 0
	case '$':
		// $FOO = replace token with a string containing the env var contents
		next_ch(s);
		s->sym = DOLAR_SYM;
		break;
#endif
	case '"':
		s->id_name[0] = 0;
		next_ch(s);
		int n = 0;
		while (s->ch != '"') {
			s->id_name[n++] = s->ch;
			next_ch(s);
		}
		next_ch(s);
		s->id_name[n] = 0;
		s->sym = STR_SYM;
		break;
	default:
		if (s->ch >= '0' && s->ch <= '9') {
			s->num_val = 0;
			while (s->ch >= '0' && s->ch <= '9') {
				s->num_val = s->num_val * 10 + (s->ch - '0');
				next_ch(s);
			}
			s->sym = NUM_SYM;
		} else if (s->ch >= 'a' && s->ch <= 'z') {
			int i = 0;
			while ((s->ch >= 'a' && s->ch <= 'z') || s->ch == '_' || (s->ch >= '0' && s->ch <= '9')) {
				s->id_name[i++] = s->ch;
				next_ch (s);
			}
			s->id_name[i] = '\0';
			s->sym = 0;
			while (words[s->sym] != NULL && strcmp(words[s->sym], s->id_name) != 0) {
				s->sym++;
			}
			if (words[s->sym] == NULL) {
				s->sym = ID_SYM;
			}
		} else {
			syntax_error(s, "unknown symbol");
		}
	}
}

/*----------------------------------------------------------------------------*/
/* Parser */

enum NodeKind {
	VAR, // variable
	CST, // constant
	STR, // string
	ADD,
	MUL,
	DIV,
	SUB,
	LT,
	SET,
	IF,
	IFELSE,
	WHILE,
	DO,
	PRINT,
	SYSTEM,
	EMPTY,
	SEQ,
	EXPR,
	PROG
};

typedef struct node {
	enum NodeKind kind;
	struct node *o1;
	struct node *o2;
	struct node *o3;
	union {
		int val;
		char id[100];
	};
} node;
static node *paren_expr(State *s);

void consume(State *s, int expected) {
	if (s->sym == expected)
		next_sym(s);
	else
		syntax_error(s, "unknown expected");
}

node *new_node(enum NodeKind k) {
	node *x = malloc(sizeof(node));
	x->kind = k;
	return x;
}

node *id(State *s) {
	node *x = new_node(VAR);
	strcpy (x->id, s->id_name); /// XXX overflow
	next_sym(s);
	return x;
}

node *num(State *s) {
	node *x = new_node(CST);
	x->val = s->num_val;
	next_sym (s);
	return x;
}

node *str(State *s) {
	node *x = new_node(STR);
	strcpy (x->id, s->id_name);
	next_sym (s);
	return x;
}
node *term(State *s) {
	if (s->sym == ID_SYM) {
		return id(s);
	}
	if (s->sym == STR_SYM) {
		return str(s);
	}
	if (s->sym == NUM_SYM) {
		return num(s);
	}
	return paren_expr (s);
}

node *sum(State *s) {
	node *x = term(s);
	while (s->sym == PLUS_SYM || s->sym == MINUS_SYM || s->sym == DIV_SYM || s->sym == MUL_SYM) {
		node *t = x;
		int type = 0;
		switch (s->sym) {
		case PLUS_SYM:
			type = ADD;
			break;
		case MUL_SYM:
			type = MUL;
			break;
		case DIV_SYM:
			type = DIV;
			break;
		case MINUS_SYM:
			type = SUB;
			break;
		}
		x = new_node(type);
		next_sym (s);
		x->o1 = t;
		x->o2 = term(s);
	}
	return x;
}

node *test(State *s) {
	node *x = sum(s);
	if (s->sym == LESS_SYM) {
		node *t = x;
		x = new_node(LT);
		next_sym(s);
		x->o1 = t;
		x->o2 = sum(s);
	}
	return x;
}

node *expr(State *s) {
	if (s->sym != ID_SYM) {
		return test(s);
	}

	node *x = test(s);
	if (x->kind == VAR && s->sym == EQUAL_SYM) {
		node *t = x;
		x = new_node (SET);
		next_sym (s);
		x->o1 = t;
		x->o2 = expr (s);
	}
	return x;
}

static node *paren_expr(State *s) {
	consume (s, LPAR_SYM);
	node *x = expr (s);
	consume (s, RPAR_SYM);
	return x;
}

node *statement(State *s) {
	node *x;
	switch (s->sym) {
	case IF_SYM:
		next_sym (s);
		x = new_node(IF);
		x->o1 = paren_expr (s);
		x->o2 = statement(s);
		if (s->sym == ELSE_SYM)
		{
			x->kind = IFELSE;
			next_sym (s);
			x->o3 = statement(s);
		}
		break;
	case WHILE_SYM:
		x = new_node(WHILE);
		next_sym (s);
		x->o1 = paren_expr (s);
		x->o2 = statement(s);
		break;
	case DO_SYM:
		x = new_node(DO);
		next_sym (s);
		x->o1 = statement(s);
		consume(s, WHILE_SYM);
		x->o2 = paren_expr (s);
		consume(s, SEMI_SYM);
		break;
	case SYSTEM_SYM:
		x = new_node(SYSTEM);
		next_sym (s);
		x->o1 = paren_expr(s);
		consume(s, SEMI_SYM);
		break;
	case PRINT_SYM:
		x = new_node(PRINT);
		next_sym (s);
		x->o1 = paren_expr(s);
		consume(s, SEMI_SYM);
		break;
	case LBRA_SYM:
		x = new_node(EMPTY);
		next_sym (s);
		while (s->sym != RBRA_SYM) {
			node *t = x;
			x = new_node(SEQ);
			x->o1 = t;
			x->o2 = statement(s);
		}
		next_sym(s);
		break;
	case SEMI_SYM:
		x = new_node(EMPTY);
		next_sym(s);
		break;
	default:
		x = new_node(EXPR);
		x->o1 = expr(s);
		consume(s, SEMI_SYM);
	}

	return x;
}

node *program(State *s) {
	node *x = new_node(PROG);
	x->o1 = statement(s);
	consume(s, EOI_SYM);
	return x;
}

void print_ast(node *x) {
	switch (x->kind) {
	case VAR:
		printf("VAR \"%s\" ", x->id);
		break;
	case CST:
		printf("CST \"%d\" ", x->val);
		break;
	case STR:
		printf("STR \"%s\" ", x->id);
		break;
	case MUL:
		print_ast(x->o1);
		printf("MUL ");
		print_ast(x->o2);
		break;
	case ADD:
		print_ast(x->o1);
		printf("ADD ");
		print_ast(x->o2);
		break;
	case SUB:
		print_ast(x->o1);
		printf("SUB ");
		print_ast(x->o2);
		break;
	case LT:
		print_ast(x->o1);
		printf("LT ");
		print_ast(x->o2);
		break;
	case SET:
		printf("SET ");
		print_ast(x->o1);
		print_ast(x->o2);
		break;
	case IF:
		printf("IF ");
		print_ast(x->o1);
		print_ast(x->o2);
		break;
	case IFELSE:
		printf("IF ");
		print_ast(x->o1);
		print_ast(x->o2);
		printf("ELSE ");
		print_ast(x->o3);
		break;
	case EXPR:
		printf("EXPR ");
		print_ast(x->o1);
		break;
	case SEQ:
		printf("SEQ ");
		print_ast(x->o1);
		print_ast(x->o2);
		break;
	case SYSTEM:
		printf("SYSTEM ");
		print_ast(x->o1);
		break;
	case PRINT:
		printf("PRINT ");
		print_ast(x->o1);
		break;
	case WHILE:
		printf("WHILE ");
		print_ast(x->o1);
		print_ast(x->o2);
		break;
	case DO:
		printf("DO ");
		print_ast(x->o1);
		printf("WHILE ");
		print_ast(x->o2);
		break;
	case PROG:
		printf("PROG ");
		print_ast(x->o1);
		break;
	case EMPTY:
		printf("EMPTY ");
		break;
	default:
		syntax_error (NULL, "unknown node");
		break;
	}
}

node *parse(State *s, const char *code) {
	s->code = code;
	next_sym (s);
	return program (s);
}

/*----------------------------------------------------------------------------*/
/* Interpreter */

typedef struct list {
	char *id;
	int value;
	struct list *next;
} list;

list *env;

list *get_id(char *id)
{
	for (list *lst = env; lst; lst = lst->next)
		if (strcmp(lst->id, id) == 0)
			return lst;

	return (list *)NULL;
}

void lookup_error(char *id) {
	fprintf(stderr, "error looking up %s\n", id);
	exit(1);
}

static int lookup_value(char *id) {
	list *pid = get_id (id);
	if (pid) {
		return pid->value;
	}
	lookup_error (id);
	return -1;
}

void add_id(char *id, int value) {
	list *pid = get_id (id);
	if (pid) {
		pid->value = value;
		return;
	}

	list *lst = malloc(sizeof(list));
	lst->id = id;
	lst->value = value;
	lst->next = env;
	env = lst;
}

void eval_error(State *s) {
	fprintf(stderr, "semantics error");
	exit(1);
}

int eval_expr(State *s, node *x) {
	switch (x->kind) {
	case VAR:
		return lookup_value (x->id);
	case CST:
		return x->val;
	case STR:
		return atoi(x->id);
	case MUL:
		return eval_expr(s, x->o1) * eval_expr(s, x->o2);
	case DIV:
		{
			int res = eval_expr (s, x->o2);
			if (res > 0) {
				return eval_expr(s, x->o1) / res;
			}
			eval_error(s);
		}
		break;
	case ADD:
		return eval_expr(s, x->o1) + eval_expr(s, x->o2);
	case SUB:
		return eval_expr(s, x->o1) - eval_expr(s, x->o2);
	case LT:
		return eval_expr(s, x->o1) < eval_expr(s, x->o2);
	case SET:
		{
			node *var = x->o1;
			int val = eval_expr (s, x->o2);
			add_id(var->id, val);
			return val;
		}
		break;
	default:
		break;
	}
	eval_error (s);
	return -1;
}

void eval_statement(State *s, node *x) {
	switch (x->kind) {
	case SYSTEM:
		// TODO: support strings
		if (x->o1->kind == STR) {
			system (x->o1->id);
		} else {
			printf ("err\n");
			eval_error (s);
		}
		break;
	case PRINT:
		if (x->o1->kind == STR) {
			printf("%s\n", x->o1->id); // eval_expr (s, x->o1));
		} else {
			printf("%d\n", eval_expr (s, x->o1));
		}
		break;
	case IF:
		if (eval_expr (s, x->o1)) {
			eval_statement (s, x->o2);
		}
		break;
	case IFELSE:
		if (eval_expr (s, x->o1)) {
			eval_statement(s, x->o2);
		} else {
			eval_statement(s, x->o3);
		}
		break;
	case WHILE:
		while (eval_expr (s, x->o1)) {
			eval_statement (s, x->o2);
		}
		break;
	case DO:
		do {
			eval_statement (s, x->o1);
		} while (eval_expr (s, x->o2));
		break;
	case SEQ:
		eval_statement (s, x->o1);
		eval_statement (s, x->o2);
		break;
	case EXPR:
		eval_expr (s, x->o1);
		break;
	case EMPTY:
		break;
	default:
		eval_error (s);
	}
}

void eval_program(State *s, node *x) {
	if (x->kind == PROG) {
		eval_statement (s, x->o1);
	} else {
		eval_error (s);
	}
}

void free_program(State *s, node *x) {
	// recursive node free
}

int main(int argc, char **argv) {
	State s = {0};
	node *prog = parse (&s, (argc>1)?argv[1]: NULL);
//	print_ast (prog);
//	printf ("\n");
	eval_program (&s, prog);
	free_program (&s, prog);
	return 0;
}
