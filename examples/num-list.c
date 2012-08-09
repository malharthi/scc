
// A prorgam that parses a string contains numbers seperated
// by spaces.
void main()
{
	int i, j=0, value=0, str_len, sum=0;
	int numbers[10];
	char buffer[100];
	
	printStr("enter seperated values: \n");
	readStr(buffer, 100);
	
	str_len=0;
	while (buffer[str_len] != 0)
		str_len++;
		
	for (i=0; i<str_len; i++)
	{
		if (buffer[i] == ',' || buffer[i] == ' ')
			continue;
			
		if (!(buffer[i] >= '0' && buffer[i] <= '9'))
		{
			printStr("ERROR: numbers only allowed.\n");
			break;
		}
		
		while (buffer[i] >= '0' && buffer[i] <= '9')
		{
			int digit = buffer[i] - 48;
			value = value * 10 + digit;
			i++;
		}
		
		numbers[j] = value;
		value = 0;
		j++;
	}
	
	for (i=0; i<j; i++)
	{
		sum = sum + numbers[i];
		
		printInt(numbers[i]);
		printChar(' ');
	}
	
	printStr("\n\n Median(numbers) = ");
	printInt(sum / j);
}