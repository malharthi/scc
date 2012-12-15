
/* Factorial */

void main () {
	int number, i, factorial = 1;
	
	printStr("enter a number: ");
	readInt(number);
	
	for (i = 1; i <= number; i++)
	{
		factorial = factorial * i;
	}
	
	printStr("Factorial = ");
	printInt(factorial); printChar('\n');
}
