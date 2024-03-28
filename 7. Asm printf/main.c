extern void MAprintf(const char* str, char c1, char c2, char c3, char c4, char c5, char c6);

int main() {
	const char s[] = "Hello, %c | %c | %c | %c | %c | %c world!\n";
	MAprintf(s, s[0], s[1], s[2], s[4], s[5], s[3]);

	return 0;
}
