/* lex1.c         14 Feb 01; 31 May 12       */

/* This file contains code stubs for the lexical analyzer.
 Rename this file to be lexanc.c and fill in the stubs.    */

/* Copyright (c) 2001 Gordon S. Novak Jr. and
 The University of Texas at Austin. */

/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "token.h"
#include "lexan.h"

#define MAX_SIZE 15

/* This file will work as given with an input file consisting only
 of integers separated by blanks:
 make lex1
 lex1
 12345 123    345  357
 */

/* Skip blanks and whitespace.  Expand this function to skip comments too. */
void skipblanks() {
	int c;
	int c1;
	while ((c = peekchar()) != EOF) {
		if (c == ' ' || c == '\n' || c == '\t') {
			getchar();
			continue;
		} else if (c == '{') {
			getchar();
			while ((c = peekchar()) != EOF && c != '}') {
				getchar();
			}
			getchar();
		} else if (c == '(' && (c1 = peek2char()) != EOF && c1 == '*') {
			getchar();
			getchar();
			while ((c = peekchar()) != EOF && (c1 = peek2char()) != EOF) {
				getchar();
				if (c == '*' && c1 == ')') {
					getchar();
					break;
				}
			}
		} else {
			break;
		}

	}
}

int delimiterLookUp(char * str) {
	static char *delimiter[] = { "  ", ",", ";", ":", "(", ")", "[", "]", ".." };
	int i = 0;
	int size = sizeof(delimiter) / sizeof(char *);

	/* We skip first element as it is blank */
	for (i = 1; i < size; i++) {
		if (strcmp(str, delimiter[i]) == 0) {
			return i;
		}
	}
	return 0;
}

int reserveLookUp(char * str) {
	static char *reserve[] = { " ", "array", "begin", "case", "const", "do",
			"downto", "else", "end", "file", "for", "function", "goto", "if",
			"label", "nil", "of", "packed", "procedure", "program", "record",
			"repeat", "set", "then", "to", "type", "until", "var", "while",
			"with" };
	int i = 0;
	int size = sizeof(reserve) / sizeof(char *);

	/* We skip first element as it is blank */
	for (i = 1; i < size; i++) {
		if (strcmp(str, reserve[i]) == 0) {
			return i;
		}
	}
	return 0;
}

int operatorLookUp(char * str) {
	static char *operator[] = { " ", "+", "-", "*", "/", ":=", "=", "<>", "<",
			"<=", ">=", ">", "^", ".", "and", "or", "not", "div", "mod", "in",
			"if", "goto", "progn", "label", "funcall", "aref", "program",
			"float" };
	int i = 0;
	int size = sizeof(operator) / sizeof(char *);

	/* We skip first element as it is blank */
	for (i = 1; i < size; i++) {
		if (strcmp(str, operator[i]) == 0) {
			return i;
		}
	}
	return 0;
}
/* Get identifiers and reserved words */
TOKEN identifier(TOKEN tok) {
	int c;
	int counter = 0;
	char str[MAX_SIZE + 1];
	int lookup_val = 0;
	//First character of an identifier has to be Alpha
	if ((c = peekchar()) != EOF && CHARCLASS[c] == ALPHA) {
		c = getchar();
		str[counter++] = c;
		while ((c = peekchar()) != EOF
				&& (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC)) {
			c = getchar();
			if (counter < MAX_SIZE) {
				str[counter++] = c;
			}
		}
		str[counter] = 0;
		if ((lookup_val = reserveLookUp(str)) != 0) {
			tok->tokentype = RESERVED;
			tok->whichval = lookup_val;
		} else if ((lookup_val = operatorLookUp(str)) != 0) {
			tok->tokentype = OPERATOR;
			tok->whichval = lookup_val;
		} else {
			tok->tokentype = IDENTIFIERTOK;
			strcpy(tok->stringval, str);
		}
	}
	return (tok);
}

TOKEN getstring(TOKEN tok) {
	int c;
	int c2;
	int counter = 0;
	char str[MAX_SIZE + 1];
	//First character of an identifier has to be a '
	if ((c = peekchar()) != EOF && c == '\'') {
		c = getchar();
		while ((c = peekchar()) != EOF) {
			c2 = peek2char();
			if (c == '\'' && c2 != '\'') {
				break;
			} else if (c == '\'' && c2 == '\'') {
				c = getchar();
				if (counter < MAX_SIZE) {
					str[counter++] = c;
				}
				getchar();
			} else {
				c = getchar();
				if (counter < MAX_SIZE) {
					str[counter++] = c;
				}
			}
		}
		getchar();
		str[counter] = 0;
		tok->tokentype = STRINGTOK;
		strcpy(tok->stringval, str);
	}
	return tok;
}

TOKEN special(TOKEN tok) {
	int c;
	int c2;
	char str[3];
	int lookup_val = 0;
	str[0] = 0;
	if ((c = peekchar()) != EOF) {
		if ((c2 = peek2char()) != EOF && CHARCLASS[c2] == SPECIAL) {
			str[0] = c;
			str[1] = c2;
			str[2] = 0;
			if (delimiterLookUp(str) != 0 || operatorLookUp(str) != 0) {
				getchar();
				getchar();
			} else {
				getchar();
				str[1] = 0;
			}
		} else {
			str[0] = c;
			str[1] = 0;
			getchar();
		}
	}
	if ((lookup_val = delimiterLookUp(str)) != 0) {
		tok->tokentype = DELIMITER;
		tok->whichval = lookup_val;
	} else if ((lookup_val = operatorLookUp(str)) != 0) {
		tok->tokentype = OPERATOR;
		tok->whichval = lookup_val;
	} else {
		fprintf(stderr, "Invalid Token %s\n", str);
	}
	return tok;
}

/* Get and convert unsigned numbers of all types. */
TOKEN number(TOKEN tok) {
	/*
	 long num;
	 int c, charval;
	 num = 0;
	 while ((c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC) {
	 c = getchar();
	 charval = (c - '0');
	 num = num * 10 + charval;
	 }
	 tok->tokentype = NUMBERTOK;
	 tok->datatype = INTEGER;
	 tok->intval = num;
	 return (tok);
	 */
	double num = 0;
	long numOfDigitsBeforeDecimal = 0;
	long numOfDigitsAfterDecimal = 0;
	long exponent = 0;
	int isNegExponent = 0;
	int c;
	int c2;
	int charval;
	double significand;
	int unsignedOverflow = 0;
	double numAfterDigits = 0;
	//consume characters as long as it is '0' and not a digit other than 0 or .
	while ((c = peekchar()) != EOF && c == '0') {
		getchar();
	}
	//now you have the the first non '0' character.
	//Consume and add integer values to num till you encounter a . or e
	while ((c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC) {
		//2147483647
		c = getchar();
		charval = (c - '0');
		if (num >= 214748364 && charval > 7) {
			unsignedOverflow = 1;
		}
		num = num * 10 + charval;
		numOfDigitsBeforeDecimal++;
	}
	if ((c = peekchar()) != EOF && c != '.' && c != 'e') {
		tok->tokentype = NUMBERTOK;
		tok->datatype = INTEGER;
		if (unsignedOverflow == 1) {
			printf("Integer number out of range\n");
		} else {
			tok->intval = num;
		}
		//tok->intval = num;
		return (tok);
	}
	if ((c = peekchar()) != EOF && ((c2 = peek2char()) != EOF) && c == '.'
			&& c2 == '.') {
		tok->tokentype = NUMBERTOK;
		tok->datatype = INTEGER;
		if (unsignedOverflow == 1) {
			printf("Integer number out of range\n");
		} else {
			tok->intval = num;
		}
		//tok->intval = num;
		return (tok);
	}

	if ((c = peekchar()) != EOF && (c2 = peek2char()) != EOF && (c == '.')
			&& CHARCLASS[c2] == NUMERIC) {
		getchar();
		while ((c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC) {
			c = getchar();
			//if ((numOfDigitsAfterDecimal + numOfDigitsBeforeDecimal) < MAX_SIZE) {
			charval = (c - '0');
			num = num * 10 + charval;
			numOfDigitsAfterDecimal++;
			//}
		}
	}
	if ((c = peekchar()) != EOF && c == 'e') {
		getchar();
		if ((c = peekchar()) != EOF && c == '-') {
			getchar();
			isNegExponent = 1;
		} else if ((c = peekchar()) != EOF && c == '+') {
			getchar();
		}
		while ((c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC) {
			c = getchar();
			charval = (c - '0');
			exponent = exponent * 10 + charval;
		}
	}
	if (isNegExponent == 1) {
		exponent = exponent * -1;
	}

	significand = num / pow(10, numOfDigitsAfterDecimal);
	if (numOfDigitsBeforeDecimal > 1) {
		significand /= pow(10, numOfDigitsBeforeDecimal - 1);
		exponent +=(numOfDigitsBeforeDecimal - 1);
	} else {
		while (significand < 1) {
			significand *=10;
			exponent--;
		}
	}

	tok->tokentype = NUMBERTOK;
	tok->datatype = REAL;

	if ((significand > 1.175495 && exponent == -38) || (exponent < -38)) {
		printf("Floating number out of range\n");
	} else if ((significand > 3.402823 && exponent == 38) || (exponent > 38)) {
		printf("Floating number out of range\n");
	} else {
		tok->realval = significand * pow(10, exponent);
	}
	return tok;

}

