void str_arr(char str[], int len) {
	int i;
	for (i=0; i<len; i++) {
		printChar(str[i]); printChar(' ');
	}
}
void inc_arr(int arr[], int len) {
	int i;
	for (i=0; i<len; i++) {
        	arr[i] = arr[i]+1;
	}
}
void print_arr(int arr[], int len) {
	int i;
	printInt(len);printChar('\n');

	inc_arr(arr, len);

	for (i=0; i<len; i++) {
                printInt(arr[i]); printChar('\n');
        }
}
void main() {
	int i=5;
	int array[5] = { 10, 20, 32, 42, 16 };
	// for (i=0; i<5; i++) {
	// 	printInt(array[i]); printChar('\n');
	// }
	print_arr(array, 5);
}
