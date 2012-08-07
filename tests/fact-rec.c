
int fact_recursive(int n) {
	if (n==1)
		return 1;
	else
		return n * fact_recursion(n-1);
}
int main() {
	int n;

	printStr("n? ");
	readInt(n);

	printInt(fact_recursive(n)); printChar('\n');
}