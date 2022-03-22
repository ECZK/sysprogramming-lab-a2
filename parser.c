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
int comparator(const void *a, const void *b);
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

	cards = realloc(NULL, sizeof(CARD_T)*(total_cards+1));
	
	// first read to get rid of the header
	getline(&lineptr, &n, fd);

	while ((getline(&lineptr, &n, fd)) != -1) {
		CARD_T *card = parse_card(lineptr);
		int is_dupe = dupe_check(card->id, card->name);
		// when there are no duplicates
		if (is_dupe == NO_DUPE) {
			cards[total_cards] = card;
			cards = realloc(cards, sizeof(CARD_T)*(total_cards+1));
			total_cards++;
			// break;
		} 
		// when the dupe is much higher than the current card
		else if (is_dupe == DUPE) {}
		// index of card that needs to be replaced
		else {
			CARD_T *old_card = cards[is_dupe];
			free_card(old_card);
			cards[is_dupe] = card;
			// printf("NEED TO REPLACE AT POSITION %d\n\n", is_dupe);
		}

	}

	// qsort(cards, total_cards, sizeof(CARD_T), comparator); 

	for (int i=0; i<total_cards; i++) {
		print_card(cards[i]);
		free_card(cards[i]);
	}

	if (fd == NULL) return -2;
	free(cards);
	free(lineptr);
	fclose(fd);
	return 0;
}

int comparator(const void *a, const void *b) {
	const CARD_T *a_ptr = a;
	const CARD_T *b_ptr = b; 

	return strcmp(a_ptr->name, b_ptr->name);
}

int dupe_check(unsigned id, char *name) {
	for (int i=0; i<total_cards; i++){
		CARD_T *card = cards[i];
		if (strcmp(name, card->name) == 0) {
			if (id > card->id) return DUPE; // dupe is higher
			else return i;					// dupe is lower
		}
	}
	return NO_DUPE;
}

// i have to use memory so I can actually return 
// fixed text instead
char *fix_text(char *text) {
	char *buffer = strdup(text);
	char *stringp = text;
	int offset = 0;

	// replacing "" to "
	char *token;
	
	if ((strstr(stringp, "\"\"")) != NULL) { 
		token = strsep(&stringp, "\"\"");
		while (stringp != NULL) {
			// passing text before " into the buffer
			memmove(buffer+offset, token, strlen(token)); 
			offset += strlen(token);

			// passing " into the buffer
			memmove(buffer+offset, "\"", strlen("\"")); 
			offset += strlen("\"");

			// skip over the remaining "
			stringp++;

			// check if there are any remaining "" in stringp,
			// if not, then transfer the rest of stringp into the buffer 
			if ((strstr(stringp, "\"\"")) == NULL) {
				memmove(buffer+offset, stringp, strlen(stringp)); 
				offset += strlen(stringp);
				break;
			}

			token = strsep(&stringp, "\"\"");
		}
	}

	// to make sure you end the string	
	if (offset > 0) memmove(buffer+offset, "\0", 1); 

	// copy updated contents from buffer into stringp again
	// set offset back to prevent overflow
	strcpy(stringp, buffer); 
	offset = 0;

	char *location = strstr(stringp, "</b>");
	while (location != NULL) { 
		memmove(location, END, strlen(END));
		location = strstr(stringp, "</b>");
	}

	location = strstr(stringp, "</i>");
	while (location != NULL) { 
		memmove(location, END, strlen(END));
		location = strstr(stringp, "</i>");
	}

	memmove(buffer, stringp, strlen(stringp));
	// printf("%s\n", buffer);


	location = strstr(stringp, "\\n");
	while (location != NULL) { 
		// to know where to overwrite
		int position = location - stringp; 
		strcpy(location, "\n");

		// strcpy modifies where location would be pointing
		// therefore we need to retrieve the same location from buffer
		// since stringp is essentially a copy of buffer before this
		location = strstr(buffer, "\\n");
		// copying into where the position + bold to prevent overwriting
		// the copied BOLD and skipping location by 3 to go over <b>
		strcpy(stringp+position+strlen("\n"), location+2);
		memmove(buffer, stringp, strlen(stringp));
		memmove(buffer+strlen(stringp), "\0", 1);
		// find the next <b>
		location = strstr(stringp, "\\n");
	}

	location = strstr(stringp, "<b>");
	while (location != NULL) { 
		// to know where to overwrite
		int position = location - stringp; 
		strcpy(location, BOLD);
		// strcpy modifies where location would be pointing
		// therefore we need to retrieve the same location from buffer
		// since stringp is essentially a copy of buffer before this
		location = strstr(buffer, "<b>");
		// copying into where the position + bold to prevent overwriting
		// the copied BOLD and skipping location by 3 to go over <b>
		strcpy(stringp+position+strlen(BOLD), location+3);
		// reallocating memory to make sure that the fixed string can
		// be moved back into the buffer
		buffer = realloc(buffer, sizeof(char *)*(strlen(buffer)+1));
		memmove(buffer, stringp, strlen(stringp));
		memmove(buffer+strlen(stringp), "\0", 1);
		// find the next <b>
		location = strstr(stringp, "<b>");
	}

	location = strstr(stringp, "<i>");
	while (location != NULL) { 
		// to know where to overwrite
		int position = location - stringp; 
		strcpy(location, ITALIC);
		// strcpy modifies where location would be pointing
		// therefore we need to retrieve the same location from buffer
		// since stringp is essentially a copy of buffer before this
		location = strstr(buffer, "<i>");
		// copying into where the position + bold to prevent overwriting
		// the copied ITALIC and skipping location by 3 to go over <i>
		strcpy(stringp+position+strlen(ITALIC), location+3);
		// reallocating memory to make sure that the fixed string can
		// be moved back into the buffer
		buffer = realloc(buffer, sizeof(char *)*(strlen(buffer)+1));
		memmove(buffer, stringp, strlen(stringp));
		memmove(buffer+strlen(stringp), "\0", 1);
		// find the next <b>
		location = strstr(stringp, "<i>");
	}

	return buffer;
}

/*
 * This short function simply frees both fields
 * and then the card itself
 */
void free_card(CARD_T *card) {
	// free the fields
	free(card->name);
	free(card->text);
	// free the card
	free(card);
}

CARD_T *parse_card(char *line) {
	// fields with blanks: cost[2], text[3], attack[4], health[5] 

	CARD_T *card = malloc(sizeof(CARD_T));
	char *stringp = line;

	// parsing id
	char *token = strsep(&stringp, ",");
	card->id = atoi(token);

	// parsing name
	stringp++; 
	token = strsep(&stringp, "\"");
	card->name = strdup(token);

	// parsing cost
	stringp++;
	token = strsep(&stringp, ",");
	card->cost = atoi(token);

	// parsing text

	// text needs to have memory allocated so it will
	// not break the free_memory function

	char buffer[1024];
	int offset = 0; 

	// text is null
	if (strncmp(stringp, ",", strlen(",")) == 0) {
		card->text = strdup("");
	}
	else { // text is not null
		stringp++;
		// while loop through the text and find the next "
		char *dbquote = strstr(stringp, "\"");
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
		card->text = fix_text(buffer);
	}

	// parsing attack
	stringp++;
	token = strsep(&stringp, ",");
	card->attack = atoi(token);
	
	// parsing health
	token = strsep(&stringp, ",");
	card->health = atoi(token);
	
	// parsing type
	stringp++;
	token = strsep(&stringp, "\"");

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

