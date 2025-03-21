#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bodoux.h"

int main(void)
{
	char const sentence[] = "私はその人を常に先生と呼んでいた。\n"
								 "だからここでもただ先生と書くだけで本名は打ち明けない。\n"
								 "これは世間を憚かる遠慮というよりも、その方が私にとって自然だからである。";

	int len = strlen(sentence);
	
	char* boundaries = malloc(len);
	memset(boundaries, 0, len);
	
	parse_boundaries_ja(sentence, boundaries);

	for (int i = 0; i < len; i++) {
		if (boundaries[i]) printf("|");
		printf("%c", sentence[i]);
	}
	
	free(boundaries);
	
	return 0;
}