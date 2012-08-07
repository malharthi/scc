
void main ()
{
	int i, max;
	int values[5];
	
	printStr("Enter 5 values: ");
	for (i=0; i < 5; i++)
		readInt(values[i]);
	
	max = 0;
	for (i=0; i < 5; i++)  {
		if (values[i] > values[max])
			max = i; 
	}
	
	printStr("Maximum = ");
	printInt(values[max]);
}