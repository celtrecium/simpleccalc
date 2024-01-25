#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

const char OPERATORS[]         =  {'+', '-', '*', '/', '(', ')' };
const int  OPERATORS_PRIORITY[] = { 2,   2,   3,   3,   4,   0  };

void read_input(void);
char *read_number(char *str_ptr, double *var_ptr);
bool is_operator(char ch);
int get_operator_priority(char op);
void process_lower_priority_operators(int priority);
void process_operator(char op);
void eval_operation(char op);
double eval(char *str);

#define DEFINE_STACK(name, T) T name##s_stack[256];                \
T *name##s_stack_tail = name##s_stack;                             \
const size_t name##s_stack_size = sizeof(name##s_stack)/sizeof(T); \
void push_##name(T n) {                                            \
  if (name##s_stack_tail > name##s_stack_size + name##s_stack) {   \
    fputs (#name "s stack overflow", stderr);                      \
    exit(EXIT_FAILURE);                                            \
  }                                                                \
  *(name##s_stack_tail++) = n;                                     \
}                                                                  \
T pop_##name(void) {                                               \
  if (name##s_stack >= name##s_stack_tail) {                       \
    fputs(#name "s stack underflow", stderr);                      \
    exit(EXIT_FAILURE);                                            \
  }                                                                \
  return *(--name##s_stack_tail);                                  \
}

DEFINE_STACK(number, double);
DEFINE_STACK(operator, char);

char input_buffer[256];

void read_input(void) {
  memset(input_buffer, 0, sizeof(input_buffer));
  fgets(input_buffer, 256, stdin);
}

char *read_number(char *src, double *dst) {
  char number_buffer[16];
  char *src_begin = src;
  int dots_count = 0;

  while (*src && (isdigit(*src) || *src == '.') && dots_count < 2) {
    ++src;

    if (*src == '.') {
      ++dots_count;
    }
  }

  number_buffer[src - src_begin] = 0;
  memcpy(number_buffer, src_begin, (unsigned long)(src - src_begin));

  *dst = atof(number_buffer);

  return src;
}

bool is_operator(char ch) {
  int i = 0;

  for(; i < (int)sizeof(OPERATORS); ++i) {
    if (ch == OPERATORS[i]) {
      return true;
    }
  }

  return false;
}

int get_operator_priority(char op) {
  int i = 0;

  for(; i < (int)sizeof(OPERATORS); ++i) {
    if (op == OPERATORS[i]) {
      return OPERATORS_PRIORITY[i];
    }
  }

  return INT_MAX;
}

void process_lower_priority_operators(int priority) {
  while (priority <= get_operator_priority(operators_stack_tail[-1])
         && operators_stack < operators_stack_tail
         && operators_stack_tail[-1] != '(') { 
    eval_operation(pop_operator());
  }

  if (operators_stack_tail[-1] == '(') {
    pop_operator();
  }
}

void eval_operation(char op) {
  double r = pop_number();
  double l = pop_number();

  switch (op) {
  case '+':
    push_number(l + r);
    break;
  case '-':
    push_number(l - r);
    break;
  case '*':
    push_number(l * r);
    break;
  case '/':
    push_number(l / r);
    break;
  }
}

void process_operator(char op) {
  process_lower_priority_operators(get_operator_priority(op));
  
  if (op != ')') {
    push_operator(op);
  }
}

double eval(char *str) {
  char *ptr = str;
  double number = 0;
  
  while (*ptr) {
    if (*ptr == ' ' || *ptr == '\n' || *ptr == '\t') {
      ++ptr;
    } else if (isdigit(*ptr)) {
      ptr = read_number(ptr, &number);
      push_number(number);
    } else if (is_operator(*ptr)) {
      process_operator(*ptr);
      ++ptr;
    } else {
      fprintf(stderr, "Unknown token '%c'\n", *ptr);

      exit(EXIT_FAILURE);
    }
  }

  process_lower_priority_operators(-1);
  
  return pop_number();
}

int main(void) {
  puts("simpleccalc v1.0");

  while(1) {
    fputs(">>> ", stdout);
    read_input();
    printf("%f\n", eval(input_buffer));
  }
  
  return EXIT_SUCCESS;
}
