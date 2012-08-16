
// Stack inspector (not complete)

void main()
{
	int d=7,i, text_len=0;
	char text[50] = "sodifh pesrupriu pseori peof sodifo.";
	//char hex[300];
	char hex_digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
							'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char x[1]={10};
							
	while (text[text_len] != 0)
		text_len++;
	
	//printInt(text_len);
	
	for (i=0; i<128; i++)
	{
		printInt(x[i]);
		printChar('\n');
	}
}
