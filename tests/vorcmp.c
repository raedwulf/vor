#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *f1 = fopen(argv[1], "rb");
	FILE *f2 = fopen(argv[2], "rb");
	int c1, c2;
	do {
		c1 = fgetc(f1);
		c2 = fgetc(f2);
	} while (c1 == c2 && c1 != EOF && c2 != EOF);

	fclose(f2);
	fclose(f1);

	return (c1 != c2);
}
