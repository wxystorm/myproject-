# step 1: ../build/bin/pony ../test/test_13.pony -emit=ast
# step 2: ../build/bin/pony ../test/test_13.pony -emit=mlir -opt
# step 3: ../build/bin/pony ../test/test_13.pony -emit=mlir
# step 4: ../build/bin/pony ../test/test_13.pony -emit=jit
def transpose_transpose(x) {
  return transpose(transpose(x));
}

def transpose_transpose_transpose(x) {
  return transpose(transpose(transpose(x)));
}

def main() {
  var a<2, 3> = [[1, 2, 3], [4, 5, 6]];
  var b = transpose_transpose(a);
  print(b);
  var c = transpose_transpose_transpose(a);
  print(c);
}


