// SCC testing


void ln() {
	printChar('\n');
}

int sum(int p1, int p2) {
	return p1+p2;
}

void main()
{
	int age;
	char name[20];

	printStr("name? ");
	readStr(name, 20);

	printStr("age? ");
	readInt(age);

	printStr("Welcome, ");
	printStr(name); printStr(", ");
	printInt(age); printStr(" :-)"); ln();
}
