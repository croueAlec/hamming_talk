#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct s_hamming
{
	bool v[16];
}	t_ham;


void error(char *s)
{
	dprintf(2, "%s\n", s);
	exit(1);
}

void encode(t_ham *ham, char c)
{
	// Unused bits are set to 0
	ham->v[3] = 0;
	ham->v[5] = 0;
	ham->v[12] = 0;

	// We encode our bits in the payload
	ham->v[6] = c & 0b10000000;
	ham->v[7] = c & 0b1000000;
	ham->v[9] = c & 0b100000;
	ham->v[10] = c & 0b10000;
	ham->v[11] = c & 0b1000;
	ham->v[13] = c & 0b100;
	ham->v[14] = c & 0b10;
	ham->v[15] = c & 0b1;

	// We encode the checks
	ham->v[1] = ham->v[5] ^ ham->v[9] ^ ham->v[13] ^ ham->v[3] ^ ham->v[7] ^ ham->v[11] ^ ham->v[15];
	ham->v[2] = ham->v[6] ^ ham->v[10] ^ ham->v[14] ^ ham->v[3] ^ ham->v[7] ^ ham->v[11] ^ ham->v[15];
	ham->v[4] = ham->v[5] ^ ham->v[6] ^ ham->v[7] ^ ham->v[12] ^ ham->v[13] ^ ham->v[14] ^ ham->v[15];
	ham->v[8] = ham->v[9] ^ ham->v[10] ^ ham->v[11] ^ ham->v[12] ^ ham->v[13] ^ ham->v[14] ^ ham->v[15];

	// Verify the checks;
	if (ham->v[1] != ((ham->v[5] + ham->v[9] + ham->v[13] + ham->v[3] + ham->v[7] + ham->v[11] + ham->v[15]) % 2))
		printf("Error on 1");
	if (ham->v[2] != ((ham->v[6] + ham->v[10] + ham->v[14] + ham->v[3] + ham->v[7] + ham->v[11] + ham->v[15]) % 2))
		printf("Error on 2");
	if (ham->v[4] != ((ham->v[5] + ham->v[6] + ham->v[7] + ham->v[12] + ham->v[13] + ham->v[14] + ham->v[15]) % 2))
		printf("Error on 4");
	if (ham->v[8] != ((ham->v[9] + ham->v[10] + ham->v[11] + ham->v[12] + ham->v[13] + ham->v[14] + ham->v[15]) % 2))
		printf("Error on 8");

	
}

void scrambler(t_ham *ham)
{
	// Here we will either flip zero, one or two bits to check the resilience of our data
	srand(time(NULL));
	int random = rand();

	switch (random % 3)
	{
	case 0:
		printf("no bit has been flipped\n");
		return ;
	case 1:
		ham->v[random % 15] = !ham->v[random % 15];
		printf("bit %d has been flipped\n", random % 15);
		__attribute__ ((fallthrough));
	case 2:
		ham->v[random % 12] = !ham->v[random % 12];
		printf("bit %d has been flipped\n", random % 12);
		break;
	}
}

char decode(t_ham *ham)
{
	char c = 0;

	// Check the bonus security
	if (ham->v[3])
		dprintf(2, "Error on bit 3");
	if (ham->v[5])
		dprintf(2, "Error on bit 5");
	if (ham->v[12])
		dprintf(2, "Error on bit 12");

	c = c << 1 | ham->v[6];
	c = c << 1 | ham->v[7];
	c = c << 1 | ham->v[9];
	c = c << 1 | ham->v[10];
	c = c << 1 | ham->v[11];
	c = c << 1 | ham->v[13];
	c = c << 1 | ham->v[14];
	c = c << 1 | ham->v[15];

	printf("%c\n", c);
	return c;
}

int main(int argc, char const *argv[])
{
	if (argc == 1)
		error("Invalid argument number");

	t_ham ham;

	encode(&ham, 'd');
	scrambler(&ham);
	decode(&ham);

	(void)argv;

	return 0;
}
