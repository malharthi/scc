// fib algorithm
int fib(int i) {
  printStr("fib\n");
  if (i == 1) {
    return 1;
  } else {
    if (i == 0) {
      return 0;
    } else {
      return fib(i-1) + fib(i-2);
    }
  }
}
// fib algorithm
int read() {
  int ret;
  readInt(ret);
  return ret;
}
void main() {
	int x = read();
	printInt(fib(x));
  if (x<10) 
    printInt(x);
}
