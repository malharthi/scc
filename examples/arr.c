// Passing arrays between functions

void str_arr2(char str[], int len) {
	int i;
	for (i=0; i<len; i++) {
		printChar(str[i]); printChar(' ');
	}
}
// Takes a char array (string). E.g., Giving "Greg" and prints "G r e g"
void str_arr(char str[], int len) {
	int i;
	for (i=0; i<len; i++) {
		printChar(str[i]); printChar(' ');
	}
	str_arr2(str, len);
}
// Increment every array element by one
void inc_arr(int arr[], int len) {
	int i;
	for (i=0; i<len; i++) {
        arr[i] = arr[i]+1;
	}
}
// Increments then rints array into terminal
// To test passing arrays that are passed to the function
void print_arr(int arr[], int len) {
	int i;
	printInt(len);printChar('\n');
	inc_arr(arr, len);
	for (i=0; i<len; i++) {
        printInt(arr[i]); printChar(' ');
    }
}
void main() {
	int i=5;
	int array[5] = { 10, 20, 32, 42, 16 };
	char name[7] = "Gregory";
	str_arr(name, 7); printChar('\n');
	for (i=0; i<5; i++) {
		printInt(array[i]); printChar(' ');
	}
	print_arr(array, 5);
	printChar('\n');
}
