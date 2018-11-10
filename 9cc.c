#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token type number
enum {
  TK_NUM = 256, // int token
  TK_EOF,       // input end token
};

// token type
typedef struct {
  int ty;      // token type
  int val;     // if ty is TK_NUM, the value of it
  char *input; // Token char (for error message)
} Token;

// Tokenized results are saved in this 
// assume that tokens are under 100
Token tokens[100];

// split the char of p into token and save it in tokens
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    // skip empty space
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "cannot tokenize: %s\n", p);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

// function to report error
void error(int i) {
  fprintf(stderr, "unexpected token: %s\n",
          tokens[i].input);
  exit(1);
}

// shape of Node
enum {
  ND_NUM = 256,
};

typedef struct Node {
  int ty;           
  int op; // added this
  struct Node *lhs; 
  struct Node *rhs; 
  int val;          
} Node;

Node *new_node(int op, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->op = op;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->op = ND_NUM;
  node->val = val;
  return node;
}

static Node *mul();

Node *mul() {
  int i = 0;
  Node *lhs = term();
  if (tokens[i].ty == TK_EOF)
    return lhs;
  if (tokens[i].ty == '*') {
    i++;
    return new_node('*', lhs, mul());
  }
  if (tokens[i].ty == '/') {
    i++;
    return new_node('/', lhs, mul());
  }
  printf("unexpected token: %s",
        tokens[i].input);
}


Node *expr() {
  int i = 0;
  Node *lhs = mul();
  if (tokens[i].ty == TK_EOF)
    return lhs;
  if (tokens[i].ty == '+') {
    i++;
    return new_node('+', lhs, expr());
  }
  if (tokens[i].ty == '-') {
    i++;
    return new_node('-', lhs, expr());
  }
  printf("un expected token: %s",
        tokens[i].input);
}

int term() {
  int i = 0;
  if (tokens[i].ty == TK_NUM)
    return new_node_num(tokens[i++].val);
  if (tokens[i].ty == '(') {
    i++;
    Node *node = expr();
    if (tokens[i].ty != ')')
      printf("there is no closing braket for the opened braket: %s", tokens[i].input);
    i++;
    return node;
  }
  printf("the token is not a braket or a number: %s", tokens[i].input);
}

// codes to generate stack machine like code
void gen(Node *node) {
  if (node->ty == ND_NUM) {
   printf(" push %d\n", node->val);
   return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf(" pop rdi\n");
  printf(" pop rax\n");

  switch (node->ty){
  case '+':
    printf(" add rax, rdi\n");
    break;
  case '-':
    printf(" sub rax, rdi\n");
    break;
  case '*':
    printf(" mul rdi\n");
    break;
  case '/':
    printf(" mov rdx, 0\n");
    printf(" div rdi\n");
  }
}


int main(int argc, char **argv) {
	if (argc !=2) {
		fprintf(stderr, "wrong number of inputs\n");
		return 1;
	}
	
	// tokenize and parse
	tokenize(argv[1]);
	Node* node = expr();

        // output the magical spell
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// generate the code following the tree
	gen(node);

	// the output is in the top of the stack
	// load it to RAX and make it the output of the function
	printf(" pop rax\n");
	printf(" ret\n");
	return 0;
}

