extern int MAprintf(const char* str, char c1, char c2, char c3, char c4, char c5, long unsigned c6);

int main() {
	const char s[] = "Hello, %c | %c | %c | %c | %c | %b \xf0\x9f\xa5\x94 world!\n";
	char s2[] = "aboba\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	char s3[] = "mayo\n123 ujgiuthuhtuihjuhjitrj ijgjggjti jigjrti jrtio \0 THIS SHOULD NOT APPEAR";
	int d = MAprintf(s, s[0], s[1], s[2], s[4], s[5], 694201337);
	printf("Returned value: %d\n", d);

	return 0;
}
