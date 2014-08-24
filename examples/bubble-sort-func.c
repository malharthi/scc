
/* bubble sort using a function */

void bubble_sort(int array[], int len) {
  int i, k, temp;
  for (i = 0; i < len; i++) {
    for (k = 0; k < len - i - 1; k++) {
      if (array[k] > array[k + 1]) {
        temp = array[k];
        array[k] = array[k + 1];
        array[k + 1] = temp;
      }
    }
  }
}

void print_array(int array[], int len) {
  int i;
  for (i=0; i < len; i++) {
    printInt(array[i]); printChar(' ');
  }
}

void main() { 
  int array[10] = { 34, 11, 9, 0, 5, 2, 4, 3, 1, 4 };
  int len = 10, i, k, temp;
  
  printStr("Bubble Sort Algorithm\n");
  
  printStr("Before sorting:\n");
  print_array(array, len);
  printChar('\n');

  bubble_sort(array, len);
  
  printStr("After sorting:\n");
  print_array(array, len);
  printChar('\n');
}
