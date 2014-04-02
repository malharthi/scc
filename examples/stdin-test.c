
// File reading test
// Run by:
//    cat <file> | test4

void memcpy(char src[], char dest[], int dest_at, int src_len) {
  int i;
  for (i = 0; i < src_len; i++) {
    dest[dest_at] = src[i];
    dest_at++;
  }
}

int strlen(char str[]) {
  int i = 0;
  while (str[i] != 0)
    i++;
  return i; 
}

int main() {
  char input_buffer[128];
  char file_buffer[256];
  int p = 0;
  int len;

  // Read from STDIN until '~' is found
  while (1) {
    readStr(input_buffer, 128);
    if (input_buffer[0] == '~') {
      file_buffer[p] = 0;
      p++;
      break;
    }
    else {
      len = strlen(input_buffer);
      memcpy(input_buffer, file_buffer, p, len);
      p = p + len;
      file_buffer[p] = '\n';
      p++;
    }
  }
  // Print file content from buffer
  printStr(file_buffer); printChar('\n');
  return 0;
}

// int main() {
//  char buffer1[10] = "Mohannad", buffer2[10];
//  memcpy(buffer1, buffer2, strlen(buffer1) + 1);
//  printStr(buffer2); printChar('\n');
//  return 0;
// }
