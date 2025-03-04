
#ifndef SSO_LOSS_H
#define SSO_LOSS_H
#include <fstream>
#include <memory>
#include <sstream>

typedef struct funcsize {
    int func_start;
    int func_end;
    int normalized_length;
    std::string excerpt;
} FuncSize;

FuncSize calculate_function_length(std::string infile) {
  // scan through wat file to find the start of the function
  int func_start = 0;
  int func_end = 0;
  std::ifstream watfile;
  watfile.open(infile);

  FuncSize fs;

  char a = 0;
  // find the start of the function -- `(func`
  for(; watfile.good(); a = watfile.get()) {
    func_start = watfile.tellg();
    func_start--;
    if (a == 'f') { a = watfile.get();
      if (a == 'u') { a = watfile.get();
        if (a == 'n') { a = watfile.get();
          if (a == 'c') {
            break;
          }
        }
      }
    }
  }

  // we know that a function is a close sexpr: (func ...) so count open/close
  // parentheses until we find the end of the expression
  int paren_count = 1;
  for(; watfile.good(); a = watfile.get()) {
    if (a == '(') { paren_count++; }
    else if (a == ')') { paren_count--; }
    if (paren_count == 0) {
      func_end = watfile.tellg();
      break;
    }
  }
  fs.func_start = func_start;
  fs.func_end = func_end;
  // output function for debug. we ignore whitespace characters as well as
  // parentheses because both can differ in semantically equivalent code
  int data_chars = 0;
  std::stringstream ss;
  watfile.seekg(0);
  for(; watfile.good(); a = watfile.get()) {
    if (watfile.tellg() >= func_start && watfile.tellg() <= func_end) {
      if (a != ' ' && a != '\t' && a != '\n' && a != '(' && a != ')') {
        data_chars++;
      }
      ss << a;
    }
  }

  fs.normalized_length = data_chars;
  fs.excerpt = ss.str();
  watfile.close();

  return fs;
}

// this is out cost function (SSO paper section 3.1) sizes come from
// support/sso_loss.h:calculate_function_size and eq is the hamming distance
// between the results of the target execution and the rewrite execution
float sso_cost_function(int target_size, int rewrite_size, int eq) {
    return ((float)rewrite_size/(float)target_size) + eq;
}

#endif // SSO_LOSS_H