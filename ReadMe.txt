Compile and Eval Objects on Language Scheme
#include "Scheme.h"

Interpreter interpreter;
(interpreter.Run("((or (and (number? 1) +) (and (boolean? 1) not)) 1)") == "1") // true
