#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "card.h"

int cmpfunc(const void * a, const void * b);
void print_card(CARD_T*);
void find_card(char *name, FILE *infile_cards);

INDEX_T **indexes;
size_t total_cards;

int main(int argc, char **argv) {

    // open the files
	FILE *infile_index = fopen("index.bin", "rb");
    FILE *infile_cards = fopen("cards.bin", "rb");

    if (infile_index == NULL) return -2;

    // read the total_cards and allocate memory
    fread(&total_cards, sizeof(size_t), 1, infile_index);
	indexes = realloc(NULL, sizeof(INDEX_T)*(total_cards));

    // read & create the indexes 
    for (int i=0; i<total_cards; i++){
        INDEX_T *index = malloc(sizeof(INDEX_T));

        unsigned length;
        fread(&length, sizeof(unsigned), 1, infile_index); 

        char *name = malloc(sizeof(char)*(length+1));
		fread(name, sizeof(char), length, infile_index); 

        off_t offset;
		fread(&offset, sizeof(off_t), 1, infile_index);

        index->length = length;
        index->name = name;
        index->offset = offset;
        indexes[i] = index;  
    };

    char *lineptr = NULL;	// retrieved line
	size_t n = 0;		

    // if it came from the terminal directly find the cards
    if (isatty(0)) { 
	    char *name = argv[1];
        find_card(name, infile_cards);
    }
    // if it came from input.txt then strip \n by overwriting strstr pointer
    // before finding the card
	else {
        while ((getline(&lineptr, &n, stdin)) != -1) {
            printf(">> %s", lineptr);
            char *pos = strstr(lineptr, "\n");
            strcpy(pos, "\0");
            if (strcmp(lineptr, "q") == 0) break;
            else find_card(lineptr, infile_cards); 
        }

    }

    // close files
    fclose(infile_cards);
	fclose(infile_index);    


    return 0;
}

void find_card(char *name, FILE *infile_cards) {
	for (int i=0; i<total_cards; i++){
		INDEX_T *index = indexes[i];
		if (strcmp(name, index->name) == 0) {
            // create card
		    CARD_T *card = realloc(NULL, sizeof(CARD_T));
            // seek card's file position from the start of cards.bin 
            fseeko(infile_cards, index->offset, SEEK_SET);

            int length = 0;
            char *text;

            // read in the card's details and assign them into the card
            fread(&card->id, sizeof(unsigned), 1, infile_cards);
            fread(&card->cost, sizeof(unsigned), 1, infile_cards);
            fread(&card->type, sizeof(Type), 1, infile_cards);
            fread(&card->class, sizeof(Class), 1, infile_cards);
            fread(&card->rarity, sizeof(Rarity), 1, infile_cards);
            fread(&length, sizeof(int), 1, infile_cards);

            text = malloc(sizeof(char)*length);
            fread(text, sizeof(char), length, infile_cards);
            fread(&card->attack, sizeof(unsigned), 1, infile_cards);
            fread(&card->health, sizeof(unsigned), 1, infile_cards);

            card->text = text;		
            card->name = name;

            print_card(card);
            return;
		}
	}
    // if it never finds the card, it will automatically deem it as not found
    printf("./search: '%s' not found!\n", name);
    return;
}

// copied from parser.c
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