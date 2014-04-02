
// Recursive factorial 

int fact_recursive(int n) {
  if (n==1)
    return 1;
  else
    return n * fact_recursive(n-1);
}

int main() {
  int n;

  printStr("n? ");
  readInt(n);

  printInt(fact_recursive(n)); printChar('\n');
}
