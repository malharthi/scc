
/* bubble sort */

void main()
{ 
  int array[10] = { 34, 11, 9, 0, 5, 2, 4, 3, 1, 4 };
  int lim = 10, i, k, temp;
  
  printStr("Bubble Sort Algorithm\n");
  
  // print the array
  printStr("Before sorting:\n");
  for (i=0; i < lim; i++) {
    printInt(array[i]); printChar(' ');
  }
  printChar('\n');
  
  for (i = 0; i < lim; i++) {
    for (k = 0; k < lim - i - 1; k++) {
      if (array[k] > array[k + 1]) {
        temp = array[k];
        array[k] = array[k + 1];
        array[k + 1] = temp;
      }
    }
  }
  
  // print hte array
  printStr("After sorting:\n");
  for (i=0; i < lim; i++) {
    printInt(array[i]); printChar(' ');
  }
  printChar('\n');
}
