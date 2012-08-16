void print_arr(int arr[], int len) {
	int i;
	printInt(len);printChar('\n');
	
	for (i=0; i<len; i++) {
		printInt(arr[i]); printChar('\n');
	}
	printChar('\n');
}
void main() {
	int i=5;
	int array[5] = { 10, 21, 32, 42, 15 };
	// for (i=0; i<5; i++) {
	// 	printInt(array[i]); printChar('\n');
	// }
	print_arr(i, 1);
}
