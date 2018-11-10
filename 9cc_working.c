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

// codes to generate stack machine like code
void gen(Node *node) {
  if (node->ty == ND_NUM) {
   printf(" push %d\n", node->val);
   return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf(" pop rdi\n")
  printf(" pop rax\n")

  switch (node->ty)
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



int main(int argc, char **argv) {
	if (argc !=2) {
		fprintf(stderr, "wrong number of inputs\n");
		return 1;
	}

	tokenize(argv[1]);

        // output the magical spell
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// check if the first input is a number
	// then print the first mov
	if (tokens[0].ty != TK_NUM)
	  error(0);
	printf("  mov rax, %d\n", tokens[0].val);
	
	// consume tokens like + (num) and - (num)
	//print out assenbly
	int i = 1;
	while (tokens[i].ty != TK_EOF) {
	  if (tokens[i].ty == '+') {
	    i++;
	    if (tokens[i].ty != TK_NUM)
	      error(i);
	    printf("  add rax, %d\n", tokens[i].val);
	    i++;
	    continue;
	  }

          if (tokens[i].ty == '-') {
            i++;
            if (tokens[i].ty != TK_NUM)
              error(i);
            printf("  sub rax, %d\n", tokens[i].val);
            i++;
            continue;
          }

	  error(i);
	}

	printf(" ret\n");
	return 0;
}
