
/* Converting from Decimal to Hex */

void main()
{
	int i, text_len=0;
	char text[200] = "Mohannad Atiyah Mohammad Al-Harthi";
	//char hex[300];
	char hex_digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
							'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	
	while (text[text_len] != 0)
		text_len++;
	
	for (i = 0; i < text_len+1; i++)
	{
		char shift, lower, higher;
		
		// extracting 4 lower bits
		lower = text[i] && 15; // text[i] & 00001111
		
		// shifting 4 higher bits to the right
		// using division by 2 4 times instead of shift
		higher = text[i];
		for (shift=0; shift < 4; shift++)
			higher = higher / 2;
		
		printChar(hex_digits[higher]);
		printChar(hex_digits[lower]);
		printChar(' ');
	}
}