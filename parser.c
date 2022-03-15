#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"

/*
 * I've left these definitions in from the
 * solution program. You don't have to
 * use them, but the `dupe_check()` function
 * unit test expects certain values to be
 * returned for certain situations!
 */
#define DUPE -1
#define NO_DUPE -2

/*
 * These are the special strings you need to
 * print in order for the text in the terminal
 * to be bold, italic or normal (end)
 */
#define BOLD "\e[1m"
#define ITALIC "\e[3m"
#define END "\e[0m"

/*
 * You will have to implement all of these functions
 * as they are specifically unit tested by Mimir
 */
int dupe_check(unsigned, char*);
char *fix_text(char*);
void free_card(CARD_T*);
CARD_T *parse_card(char*);
void print_card(CARD_T*);

/*
 * We'll make these global again, to make
 * things a bit easier
 */
CARD_T **cards = NULL;
size_t total_cards = 0;

int main(int argc, char **argv) {
	// TODO: V  Open the file
	//       2. Read lines from the file...
	//          a. for each line, `parse_card()`
    //          b. add the card to the array
	//       3. Sort the array
	//       4. Print and free the cards
	//       5. Clean up!

	if (argc != 2) return -1;

	char *infile = argv[1];
	FILE * fd = fopen(infile, "r");

	char *lineptr = NULL;	// retrieved line
	size_t n = 0;			// 
	
	// first read to get rid of the header
	getline(&lineptr, &n, fd);
	printf("%s", lineptr);

	while ((getline(&lineptr, &n, fd)) != -1) {
	CARD_T *card = parse_card(lineptr);
	print_card(card);
	}


	if (fd == NULL) return -2;
	free(lineptr);
	fclose(fd);
	return 0;
}

/*
 * This function has to return 1 of 3 possible values:
 *     1. NO_DUPE (-2) if the `name` is not found
 *        in the `cards` array
 *     2. DUPE (-1) if the `name` _is_ found in
 *        the `cards` array and the `id` is greater
 *        than the found card's id (keep the lowest one)
 *     3. The last case is when the incoming card has
 *        a lower id than the one that's already in
 *        the array. When that happens, return the
 *        index of the card so it may be removed...
 */
int dupe_check(unsigned id, char *name) {
	return 0;
}

/*
 * This function has to do _five_ things:
 *     1. replace every "" with "
 *     2. replace every \n with `\n`
 *     3. replace every </b> and </i> with END
 *     4. replace every <b> with BOLD
 *     5. replace every <i> with ITALIC
 *
 * The first three are not too bad, but 4 and 5
 * are difficult because you are replacing 3 chars
 * with 4! You _must_ `realloc()` the field to
 * be able to insert an additional character else
 * there is the potential for a memory error!
 */
char *fix_text(char *text) {

	// char *quotes = strstr(text, "</b>");

	// while(quotes != NULL) {
	// 	memmove(quotes, END, strlen(END));
	// 	printf("%s\n", quotes);
	// 	quotes = strstr(quotes+strlen("</b>"), "</b>");
	// }

	
	return text;
}

/*
 * This short function simply frees both fields
 * and then the card itself
 */
void free_card(CARD_T *card) {

}

/*
 * This is the tough one. There will be a lot of
 * logic in this function. Once you have the incoming
 * card's id and name, you should call `dupe_check()`
 * because if the card is a duplicate you have to
 * either abort parsing this one or remove the one
 * from the array so that this one can go at the end.
 *
 * To successfully parse the card text field, you
 * can either go through it (and remember not to stop
 * when you see two double-quotes "") or you can
 * parse backwards from the end of the line to locate
 * the _fifth_ comma.
 *
 * For the fields that are enum values, you have to
 * parse the field and then figure out which one of the
 * values it needs to be. Enums are just numbers!
 */
CARD_T *parse_card(char *line) {
	// fields with blanks: cost[2], text[3], attack[4], health[5] 
	printf("%s\n", line);

	CARD_T *card = malloc(sizeof(CARD_T));
	char *stringp = line;

	// parsing id
	char *token = strsep(&stringp, ",");
	card->id = atoi(token);

	// call dupe check as soon as you find a unique key
	// call fix text for replacing the "ugly" str in the line

	// parsing name
	stringp++; 
	token = strsep(&stringp, "\"");
	card->name = strdup(token);
	printf("after name:%s\n", stringp);

	// parsing cost
	stringp++;
	token = strsep(&stringp, ",");
	card->cost = atoi(token);
	printf("after cost:%s\n", stringp);

	// parsing text

	// text needs to have memory allocated so it will
	// not break the free_memory function

	char buffer[1024];
	int offset = 0; 

	// text is null
	if (strncmp(stringp, ",", strlen(",")) == 0) {
		card->text = strdup("");
		printf("\ntext is null\n\n");
	}
	else { // text is not null
		stringp++;
		// while loop through the text and find the next "
		// char *dbquote;
		char *dbquote = strstr(stringp, "\"");
		// printf("dbquote:%s", dbquote);
		// printf("dbquote:%s", dbquote+1);
		while((dbquote = strstr(stringp, "\""))) {
			// check if it's a terminating quote
			if (strncmp(dbquote+1, ",", strlen(",")) == 0) {
				token = strsep(&stringp, "\"");
				strcpy(buffer+offset, token);
				break;
			}
			// not a terminating quote
			else {
				token = strsep(&stringp, "\"");
				strcpy(buffer+offset, token);
				offset += strlen(token);
				stringp++;
				strcpy(buffer+offset, "\"\"");
				offset += strlen("\"\"");
			}
		}
		printf("buffer:%s\n", buffer);
		// char *fixed_text = fix_text(buffer);
		card->text = strdup(buffer);
	}

	// parsing attack
	stringp++;
	token = strsep(&stringp, ",");
	card->attack = atoi(token);
	printf("attack:%s\n", token);
	printf("stringp:%s\n", stringp);
	
	// parsing health
	token = strsep(&stringp, ",");
	card->health = atoi(token);
	printf("health:%s\n", token);
	printf("stringp:%s\n", stringp);
	
	// parsing type
	stringp++;
	token = strsep(&stringp, "\"");
	printf("type:%s\n", token);
	printf("stringp:%s\n", stringp);

	switch(token[0]) {
		case 'H' : card->type = HERO; break;
    	case 'M' : card->type = MINION; break;
		case 'S' : card->type = SPELL; break;
    	default : card->type = WEAPON; break;
	}

	// parsing class
	stringp++;	// get rid of remaining "
	stringp++;	// get rid of ,,
	token = strsep(&stringp, "\"");
	printf("class:%s\n", token);
	printf("stringp:%s\n", stringp);

	if (strcmp(token, "DEMONHUNTER") == 0) card->class = DEMONHUNTER;
	if (strcmp(token, "DRUID") == 0) card->class = DRUID;
	if (strcmp(token, "HUNTER") == 0) card->class = HUNTER;
	if (strcmp(token, "MAGE") == 0) card->class = MAGE;
	if (strcmp(token, "NEUTRAL") == 0) card->class = NEUTRAL;
	if (strcmp(token, "PALADIN") == 0) card->class = PALADIN;
	if (strcmp(token, "PRIEST") == 0) card->class = PRIEST;
	if (strcmp(token, "ROGUE") == 0) card->class = ROGUE;
	if (strcmp(token, "SHAMAN") == 0) card->class = SHAMAN;
	if (strcmp(token, "WARLOCK") == 0) card->class = WARLOCK;
	if (strcmp(token, "WARRIOR") == 0) card->class = WARRIOR;

	// // parsing rarity
	stringp++;	// get rid of remaining "
	stringp++;	// get rid of ,,
	token = strsep(&stringp, "\"");
	printf("rarity:%s\n", token);
	printf("stringp:%s\n", stringp);

	switch(token[0]) {
		case 'F' : card->rarity = FREE; break;
		case 'C' : card->rarity = COMMON; break;
		case 'E' : card->rarity = EPIC; break;
		case 'R' : card->rarity = RARE; break;
    	default : card->rarity = LEGENDARY; break;
	}

	return card;
}

/*
 * Because getting the card class centered is such
 * a chore, you can have this function for free :)
 */
void print_card(CARD_T *card) {
	printf("%-29s %2d\n", card->name, card->cost);
	unsigned length = 15 - strlen(class_str[card->class]);
	unsigned remainder = length % 2;
	unsigned margins = length / 2;
	unsigned left = 0;
	unsigned right = 0;
	if (remainder) {
		left = margins + 2;
		right = margins - 1;
	} else {
		left = margins + 1;
		right = margins - 1;
	}
	printf("%-6s %*s%s%*s %9s\n", type_str[card->type], left, "", class_str[card->class], right, "", rarity_str[card->rarity]);
	printf("--------------------------------\n");
	printf("%s\n", card->text);
	printf("--------------------------------\n");
	printf("%-16d%16d\n\n", card->attack, card->health);
}

