

void main()
{	
	char x,y;
	char msg[15]="Enter number: ";
	
	printStr(msg);
	readInt(x);
	
	printStr(msg);
	readInt(y);
	
	printStr("result: ");
	printInt(x*y);
}
