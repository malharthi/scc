
/* insertion sort program*/

void main() {	
	int array[10] = { 34, 11, 9, 0, 5, 2, 4, 3, 1, 4 };
	int i, k, n=10;
	
	printStr("Insertion Sort Algorithm\n");
	
	// print the array
	// printStr("\n Before sorting:\n");
	// for (i = 0; i < n; i++) {
	// 	printInt(array[i]); printChar(' ');
	// }
	
	for (k = 1; k < n; k++) {
		int key = array[k];
		int i = k - 1;
		while ((i >= 0) && (key < array[i])) {
			array[i+1] = array[i];
			i--;
		}
		array[i+1]=key;
	}
	
	// print hte array
	printStr("After sorting:\n");
	for (i = 0; i < n; i++) {
		printInt(array[i]); printChar(' ');
	}
	printChar('\n');
}
