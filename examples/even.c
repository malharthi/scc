
// A program that lists even numbers between two numbers

void main()
{	
	int i, start, end;
	
	printStr("enter start number: ");
	readInt(start);
	
	printStr("enter end number: ");
	readInt(end);
	
	for (i=start; i<end; i++) {
		if (i%2 != 0)
			continue;
			
		printInt(i);
		printChar(' ');
	}
}
