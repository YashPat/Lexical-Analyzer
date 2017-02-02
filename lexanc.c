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
#include "token.h"
#include "lexan.h"

#define MAX_SIZE 15

/* This file will work as given with an input file consisting only
 of integers separated by blanks:
 make lex1
 lex1
 12345 123    345  357
 */

/*
 * powers table
 */
const double powers[] = { 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
		1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21,
		1e22, 1e23, 1e24, 1e25, 1e26, 1e27, 1e28, 1e29, 1e30, 1e31, 1e32, 1e33,
		1e34, 1e35, 1e36, 1e37, 1e38, 1e39, 1e40, 1e41, 1e42, 1e43, 1e44, 1e45,
		1e46, 1e47, 1e48, 1e49, 1e50, 1e51, 1e52, 1e53, 1e54, 1e55, 1e56, 1e57,
		1e58, 1e59, 1e60, 1e61, 1e62, 1e63, 1e64, 1e65, 1e66, 1e67, 1e68, 1e69,
		1e70, 1e71, 1e72, 1e73, 1e74, 1e75, 1e76, 1e77, 1e78, 1e79, 1e80, 1e81,
		1e82, 1e83, 1e84, 1e85, 1e86, 1e87, 1e88, 1e89, 1e90, 1e91, 1e92, 1e93,
		1e94, 1e95, 1e96, 1e97, 1e98, 1e99, 1e100 };

/* Skip blanks and whitespace.  Expand this function to skip comments too. */
void skipblanks() {
	int c;
	int c1;
	while ((c = peekchar()) != EOF) {
		if (c == ' ' || c == '\n' || c == '\t') { /*check if space, new line, or tab*/
			getchar();
			continue;
		} else if (c == '{') { /*Handle comments of type { }*/
			getchar();
			while ((c = peekchar()) != EOF && c != '}') { /*look for matching right bracket*/
				getchar();
			}
			getchar();
		} else if (c == '(' && (c1 = peek2char()) != EOF && c1 == '*') { /*Handle comments of type (* *) */
			getchar();
			getchar();
			while ((c = peekchar()) != EOF && (c1 = peek2char()) != EOF) {
				getchar();
				if (c == '*' && c1 == ')') { /* check for closing *) */
					getchar();
					break;
				}
			}
		} else {
			break;
		}

	}
}
/*
 * Delimited look up. Returns the delimiter whichval if the str is a delimiter.
 * Return 0 if it is not.
 */
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
/*
 * Reserved word look up. Returns the reserved word whichval if the str is a reserved word.
 * Return 0 if it is not.
 */
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
/*
 * Operator look up. Returns the operator whichval if the str is an operator.
 * Return 0 if it is not.
 */
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
		/* Read the identifier and store it in str */
		while ((c = peekchar()) != EOF
				&& (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC)) {
			c = getchar();
			if (counter < MAX_SIZE) {
				str[counter++] = c;
			}
		}
		str[counter] = 0;
		/* if else statement to look if a reserve word, operator, or identifier*/
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
/*
 * Reads a string in and returns a token.
 */
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
			} else if (c == '\'' && c2 == '\'') {/*Check to see if two ' are in a row, and to only use one ' */
				/*example 'he''llo' should be he'llo*/
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
/*
 * Reads the input and determines if the next token is an operator or delimiter
 */
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
			if (delimiterLookUp(str) != 0 || operatorLookUp(str) != 0) { /*check to see if a delimiter or operator character of length 2*/
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
	double backup_num;
	/* consume characters as long as it is '0' and not a digit other than 0 or . */
	while ((c = peekchar()) != EOF && c == '0') {
		getchar();
	}
	/* now you have the the first non '0' character.*/
	/* Consume and add integer values to num till you encounter a . or e */
	while ((c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC) {
		c = getchar();
		charval = (c - '0');
		if (num >= 214748364 && charval > 7) {
			unsignedOverflow = 1;
			if (num == 214748364) {
				backup_num = num;
			} else {
				backup_num = 999999999;
			}
		}
		num = num * 10 + charval;
		numOfDigitsBeforeDecimal++;
	}
	if ((c = peekchar()) != EOF && c != '.' && c != 'e') {
		tok->tokentype = NUMBERTOK;
		tok->datatype = INTEGER;
		if (unsignedOverflow == 1) {
			printf("Integer number out of range\n");
			tok->intval = backup_num;
		} else {
			tok->intval = num;
		}
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
		return (tok);
	}
	/* We now know we have a float not an integer  */
	/*Checking for decimal point and read the decimal portion. */
	if ((c = peekchar()) != EOF && (c2 = peek2char()) != EOF && (c == '.')
			&& CHARCLASS[c2] == NUMERIC) {
		getchar();
		while ((c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC) {
			c = getchar();
			charval = (c - '0');
			num = num * 10 + charval;
			numOfDigitsAfterDecimal++;
		}
	}
	/* check for exponential character and its sign */
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
	/* If negative exponent flag is set, change the exponent. */
	if (isNegExponent == 1) {
		exponent = exponent * -1;
	}
	/* Calculate the significand and adjust the exponent*/
	significand = num / powers[numOfDigitsAfterDecimal];
	if (numOfDigitsBeforeDecimal > 1) {
		significand /= powers[numOfDigitsBeforeDecimal - 1];
		exponent += (numOfDigitsBeforeDecimal - 1);
	} else {
		while (significand < 1) {
			significand *= 10;
			exponent--;
		}
	}

	tok->tokentype = NUMBERTOK;
	tok->datatype = REAL;

	/* Handle for overflow or underflow, else return the value */
	if ((significand > 1.175495 && exponent == -38) || (exponent < -38)) {
		printf("Floating number out of range\n");
	} else if ((significand > 3.402823 && exponent == 38) || (exponent > 38)) {
		printf("Floating number out of range\n");
	} else if (exponent >= 0) {
		tok->realval = significand * powers[exponent];
	} else {
		tok->realval = significand / powers[-exponent];
	}
	return tok;

}
