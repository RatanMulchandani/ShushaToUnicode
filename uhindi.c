/* Name: uhindi.c
 *
 * Author : Ratan Mulchandani
 *
 * start date: 27 Jan 2021
 * second rewritten version 14 feb 2021
 * version 0.6 now adding comments and flow information 5 March 21
 *
 *
 */

#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>      /* wint_t */
#include <ctype.h>
#include <stdlib.h>
#include "DevanagariCodePoints.h"
#include "OtherCodePoints.h"
#include "ShushaKeyMapping.h"

// command line syntax
// ShushaToUnicode <input file>
// output is on standard output needed to be directed to output file as
// command line entry: ShushaToUnicode inputfile > outputfile

// I am  assuming  input file having shusha code  is in utf8 . Most of key codes are 7 bit assci with few as
// 8 bits.  As I don't have shusha technical details I have collected code relation with devganari verns(वर्ण)
// from web pages available in shusha on abhivyakti.org. The difference among shusha, shusha02 and shusha05 are also
// unclear to me. Also used keyboard layout found on web to get key vs hindi chars.

// I have used  https://www.pramukhfontconverter.com/hindi/shusha-to-unicode  web service to check
// conversion or to understand shusha codes

// prototypes
// shusha utf8 is converted to shushautf16 as internally program works on 16 bit codes
void convertShushaUtf8ToShushaUtf16();

// utf16 is converted to  utf8 for saving and/or display on standard output
void encodeUtf16ToUtf8(wint_t codeToUse);

//the r which comes on top of character (र्र ) needs to be adjusted due to different way it is used
// in shusha and unicode hindi
void adjustForTopR();

// r coming on side of character(प्र) has to be handled differently
void adjustForSideR();

// check if character at  input offset is a vowel excluding small i.
// small i is prefix to consonant.
int isVowel(int indexOffSet);

// converts shusha utf16 to devnagari/hindi utf16 code.
//mapping needs some processing and adding more than one code
void shushaUtf16ToHindiUtf16();

// for some single shusha code we need two unicodes
void doubleUnicode(wint_t codeToUse);

// single code mapping
void singleUnicode(wint_t codeToUse);

// dot on top of character(ं) is to be moved after matras in unicode. Shusha puts is before matras
int checkswapForM();

//comma is used as dot at the bottom of char as in ज़
// Adjusting for side r as in प्र
void singleUnicodeWithNextCammaAndSideR(wint_t codeToUse);

//Check if small i was there. This is indicated by var iFlag.
void nextCodeWithIflagCheck();

// process input file one word at a time
int processInputShushaFile( FILE *inFp);

//read shusha character one word at a time end of word is nil terminated. It is utf8 coded
unsigned char shushaUtf8InBuf[500]; // code from file
int shushaUtf8InBufIndex;

// convert shusha utf8 to shusha utf16. Nil terminated
wint_t shushaUtf16Buf[1000]; // file byte code to utf16 but still shusha code
int shushaUtf16BufIndex;

// devnagiri utf16,  converted code from shushutf16. Nil teminated
wint_t hindiUtf16Buf[1000];// shusha utf16 converted to standard devnagari utf16
int hindiUtf16BufIndex;

// final devnagari utf8 code.. Nil teminated
unsigned char utf8Buffer[2000]; // devnagari utf8 code
int utf8Index;

// shusha vowel list
unsigned char shushaPostfixVowelList[] = "aIOoUu,"; // small i is prefix vowel or comma.
unsigned char halfVaran[] = "%#HYyxXvsSqQnNmljJgGcbBE@<F"; // shusha uses these chars as hindi char with halant
int iFlag; // small ि adjustment indicator
int halantChar = 0; // halant char indicator
wint_t shushaUtf16Code; // keep the code shusha in utf16 form

//
int main(int argc, char *argv[])
{
	//needed to display unicode characters
	setlocale(LC_ALL, ""); // unicode display requirement

	// minimum input file is needed
	if( argc < 2)
	{
		printf("Missing input file\n");
		printf("ShushaToUnicode inputfile > outputfile\n");
		return 1;
	}
// read mode open is needed
	FILE *inFp = fopen(argv[1], "r");
	if(inFp == NULL)
	{
		printf("Unable to open input file <%s>\n", argv[1]);
		return 1;
	}

// process input file convert susha to hindi unicode
	return  processInputShushaFile(inFp);
}

// read full file but one word at time.
// end of file indicator to terminate input file reading loop
int processInputShushaFile( FILE *inFp)
{
// flag used to indicate end of file when file is fully read
	 int doneFlag = 0;

	 //current read byte from file
	 int charRead;

	 //read full file till eof. eof indication flag is doneFlag
	 while(doneFlag == 0)
	 {
		 // collect word with leading white spaces
		 // index to input shusha code-word collecting buffer, initialised and buffer is nil teminated
		 shushaUtf8InBufIndex = 0;
		 shushaUtf8InBuf[shushaUtf8InBufIndex] = 0x00;
		 //read leading white spaces
		 while(doneFlag == 0)
		 {
			 charRead = fgetc(inFp);
			 if(charRead == EOF)
			 {
				 doneFlag = 1;
				 break;
			 }
			 shushaUtf8InBuf[shushaUtf8InBufIndex++] = charRead;// save read current char
			 shushaUtf8InBuf[shushaUtf8InBufIndex] = 0x00;
			 if(isspace(charRead) == 0) // break when it is not a space i.e. it is now start of word with alphabet
			 {
				 break;
			 }
		 }
		 // continue to read till again find white char. word is assumed between to white spaces
		 while(doneFlag == 0)
		 {
			 charRead = fgetc(inFp);
			 if(charRead == EOF)
			 {
				 doneFlag = 1;
		 		 break;
		 	 }
			 //save it and nil terminate next location
			shushaUtf8InBuf[shushaUtf8InBufIndex++] = charRead;
			shushaUtf8InBuf[shushaUtf8InBufIndex] = 0x00;
			// continue if not space char
	 		 if(isspace(charRead) != 0)
	 		 {
				 break;
	 		 }
		 }
		 // we have now one word read from input file
		 // first we will convert this  read shusha utf8 codes in to utf16 shusha code
		 convertShushaUtf8ToShushaUtf16();

		 // this function converts shusha hindi code to unicode 16 bit hindi code one word at a time
		shushaUtf16ToHindiUtf16();

		// the devnagari unicode utf16  is converted to uf8 hindi code
		int outIndex;
		// unicode to utf8 convertion only hindi
		outIndex = 0;
		utf8Index = 0;
		while(hindiUtf16Buf[outIndex] != NULL_CHARACTER)
		{
			wint_t shushaUtf16CodeTemp;

			// correct for आ ओ औ. This is needed as some implementation do not display अ ा combination correctly
			// unicode gives to way to make आ. आ as one unicode or  अ ा to make आ
			// we will replace  अ ा with आ . Same for ओ औ
			shushaUtf16Code = hindiUtf16Buf[outIndex];
			//combined few devnagari varan with matra
			switch(shushaUtf16Code)
			{
			case LETTER_A:  // अ followed by ा ो ौ
				shushaUtf16CodeTemp = hindiUtf16Buf[outIndex + 1];
				switch(shushaUtf16CodeTemp)
				{
				case VOWEL_SIGN_AA: // it is to be replaced with आ
					shushaUtf16Code = LETTER_AA;
					outIndex++;
					shushaUtf16CodeTemp = hindiUtf16Buf[outIndex + 1];
					if(shushaUtf16CodeTemp == VOWEL_SIGN_CHANDRA_E) // if ॅ this then change to ॉ
					{
						shushaUtf16Code = LETTER_CHANRA_O ;
						outIndex++;
					}
					break;
				case VOWEL_SIGN_O: // it is ओ
					shushaUtf16Code = LETTER_O;
					outIndex++;
					break;
				case VOWEL_SIGN_AU: // it is औ
					shushaUtf16Code = LETTER_AU;
					outIndex++;
					break;
				default:
					break;
				}
				break;
			case VOWEL_SIGN_AA:
				shushaUtf16CodeTemp = hindiUtf16Buf[outIndex + 1];
				if(shushaUtf16CodeTemp == VOWEL_SIGN_CHANDRA_E) // if ॅ this then change to ॉ
				{
					shushaUtf16Code = VOWEL_SIGN_CHANRA_O; // convert to ॉ
					outIndex++;
				}
				break;
			case LETTER_E: // ै ए े  correction
				shushaUtf16CodeTemp = hindiUtf16Buf[outIndex + 1];
				if( (shushaUtf16CodeTemp == VOWEL_SIGN_E) || (shushaUtf16CodeTemp == VOWEL_SIGN_AI))// if ॅ this then change to ॉ
				{
					shushaUtf16Code = LETTER_AI; // convert to एे
					outIndex++;
				}
				break;
			default:
				break;

			}
			encodeUtf16ToUtf8(shushaUtf16Code);
			outIndex++;
		}
				//  will replace for writing directly to file
		for(int i = 0; i < utf8Index; i++)
		{
					printf("%c", utf8Buffer[i]);
		}

		if(doneFlag == 1)
		{
			break;
		}
	}
	fclose(inFp);
	return 0;
}
// single shusha code to single unicode
void singleUnicode(wint_t codeToUse)
{
	halantChar = 0; // indicates it is char with halant. Single code does not have halant

	// make available next code
	shushaUtf16BufIndex++;
	shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
// save the converted unicode
	hindiUtf16Buf[hindiUtf16BufIndex++] = codeToUse;
	if(iFlag)
	{
		hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_I ; // This is ि मात्रा   कोड
		iFlag = 0;
	}
}
//
void doubleUnicode(wint_t codeToUse)
{
	halantChar = 0;
	hindiUtf16Buf[hindiUtf16BufIndex++] = codeToUse;
	// make available next code
	shushaUtf16BufIndex++;
	shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
	if(shushaUtf16Code == 0x0061) // a ा
	{
		shushaUtf16BufIndex++;
		shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
		if(shushaUtf16Code == COMMA_IS_SIGN_NUKTA) // ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
		{
			hindiUtf16Buf[hindiUtf16BufIndex++] = SIGN_NUKTA;
			shushaUtf16BufIndex++;
			shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
		}
		if(shushaUtf16Code == 0x0060) //   `       ्र   0x094D(्)  0x0930(र )
		{
			hindiUtf16Buf[hindiUtf16BufIndex++] = SIGN_VIRAMA;
			hindiUtf16Buf[hindiUtf16BufIndex++] = LETTER_RA;
			shushaUtf16BufIndex++;
			shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
		}
	}
	else
	{
		hindiUtf16Buf[hindiUtf16BufIndex++] = SIGN_VIRAMA;
		halantChar = 1;// it is half varan
	}
    // ि की मात्रा  तो नहीं लगानी?
	if( (iFlag) && (hindiUtf16Buf[hindiUtf16BufIndex - 1] != SIGN_VIRAMA))
	{
		hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_I ;
		iFlag = 0;
	}
}
// index in input to shusha to utf16 converted word, intialised and buffer nil terminated
void convertShushaUtf8ToShushaUtf16()
{
	unsigned char currentByte;
	unsigned char secondByte;
	unsigned char tempByte1;
	unsigned char tempByte2;
	unsigned char tempByte3;
	wint_t currentUtf16;

	shushaUtf16BufIndex = 0x00;
	shushaUtf16Buf[shushaUtf16BufIndex] = NULL_CHARACTER;

	int i = 0;
	currentByte = shushaUtf8InBuf[i];
	 while( currentByte) // nil byte end of word
	 {
		 // check if it has 8th bit zero. if yes then it is seven bit ascii
		 if((currentByte  & 0x80) == 0x00 ) // it is single byte
		 {
			 currentUtf16 = 0x0000 + currentByte;
			 shushaUtf16Buf[shushaUtf16BufIndex++] = currentUtf16;
			 shushaUtf16Buf[shushaUtf16BufIndex] = NULL_CHARACTER;
			 //printf("0x%04X ", currentUtf16);
		 }
		 else
		 {
			// check if it is two bytes by masking with 0xE0 and looking for 0xC0
			 if((currentByte  & 0xE0) == 0xC0)
			 {
				 // get top five bits from first byte
				 tempByte1 = currentByte & 0x1F;
				 i++;
				 secondByte = shushaUtf8InBuf[i];
				 //get lower six bits from second byte
				 tempByte2 = secondByte & 0x3F;
				 // move lower 2 bit to position 6 and 7 to be added to form lower byte of two bytes 0x07ff max value code
				 tempByte3 = (tempByte1 << 6) & 0xC0;
				 // full lower byte
				 tempByte2 = tempByte2 | tempByte3;
				 // get higher 3 bits
				 currentUtf16 = (tempByte1 >> 2) & 0x07;
				 // form 16 bit unicode by first moving higher three bit in code word to higher position
				 currentUtf16  = (currentUtf16  << 8) & 0x0700;
				 // add lower byte to form full unicode
				 currentUtf16  = (currentUtf16  | tempByte2) & 0x07FF;
				 shushaUtf16Buf[shushaUtf16BufIndex++] = currentUtf16;
				 shushaUtf16Buf[shushaUtf16BufIndex] = NULL_CHARACTER;
				// printf("0x%04X ", currentUtf16);
			 }
			 else
			 {
				 // check if it is three bytes by masking with 0xF0 and looking for 0xE0
				 if((currentByte  & 0xF0) == 0xE0)
				{

					 // get top four bits(7,6,5,4) of MS bytes from first byte
					 tempByte1  = currentByte & 0x0F;
					 tempByte1 = (tempByte1 << 4 ) & 0xF0;
					 // read next byte of utf8 code
					 i++;
					 secondByte = shushaUtf8InBuf[i];
					 //get lower four bits(3,2,1,0) of MS byte  and higher 2 bits (7,6) from second byte
					 tempByte2 = secondByte & 0x3F;
					 tempByte3 = (tempByte2 >> 2) & 0x0F;

					 // combined ms nibble and ls nibble of ms byte
					 tempByte1 = tempByte1 | tempByte3;
					 //place it in higher byte of utf16 code
					 currentUtf16  = tempByte1;
					 currentUtf16  = (currentUtf16  << 8) & 0xFF00;

					 // get higher two bits(7,6) of ls byte
					 tempByte3 = tempByte2 & 0x03;
					 tempByte3 = (tempByte3 << 6) & 0xC0;
					 // third byte of utf8
					 i++;
					 secondByte = shushaUtf8InBuf[i];
					 // get last six bits (5,4,3,2,1,0) of ls byte
					 tempByte2 = secondByte & 0x3F;
					 // combined both to get all 8 bits of ls byte
					 tempByte3 = tempByte3 | tempByte2;
					 // merge both bytes to get full utf16 code
					 currentUtf16 = currentUtf16 | tempByte3;

					 // save the same
					 shushaUtf16Buf[shushaUtf16BufIndex++] = currentUtf16;
					 shushaUtf16Buf[shushaUtf16BufIndex]   = NULL_CHARACTER;
					// printf("0x%04X ", currentUtf16);

				 }
				 else
				 {
					 // it is Four bytes. Currently we will print error message and exit. Will handle after finishing this tool
					 printf("Four byte utf8 not decoded yet\n");
					 exit(currentByte);
				 }
			 }
		 }
		 i++;
		currentByte = shushaUtf8InBuf[i];
		adjustForTopR();
		adjustForSideR();
	 }
	// printf("\n");

}
// symbol - is used for putting top ( र्र  ) . It need some fixing in unicode to work properly
void adjustForTopR()
{
	wint_t  tempCode1;
	wint_t  tempCode2;
	int tempCount;
	if(shushaUtf16BufIndex < 2)
	{
		return;
	}
	tempCode1 = shushaUtf16Buf[shushaUtf16BufIndex - 1]; // last char in buffer check for -
	tempCode2 = shushaUtf16Buf[shushaUtf16BufIndex - 2]; // next last check for [
	if( (tempCode1 == HYPHEN_MINUS_IS_HALF_RA) && (tempCode2 == 0x005B)) // will be handled in unicode devnagari code conversion
	{
		return;
	}
	tempCount = 2; // this offset is where we want to search back till we find character without ha
	if ((tempCode1 != HYPHEN_MINUS_IS_HALF_RA) || (shushaUtf16BufIndex < 2))
	{
		return;
	}
	while(1)
	{
		if (isVowel(tempCount) == 0)// it is not vowel. check for half char
		{
			int j;
			j = 0;
    		while(halfVaran[j]) // search for match in array of half chars
			 {
				if( halfVaran[j] == shushaUtf16Buf[shushaUtf16BufIndex - (tempCount + 1)] )
				{
					 tempCount++;
					 break;
				}
				 j++;
			 }
			for(int i = 2; i <= tempCount; i++)
			{
				shushaUtf16Buf[shushaUtf16BufIndex  - (i - 1)] = shushaUtf16Buf[shushaUtf16BufIndex  - i];
			}
			shushaUtf16Buf[shushaUtf16BufIndex  - tempCount] = HYPHEN_MINUS_IS_HALF_RA;
			break;
		 }
		else
		{
			tempCount++;
		 }
	}
}

// symbol` is used for putting bottom ( र ) . It need some fixing in unicode to work properly
void adjustForSideR()
{
	//0x60     `
	wint_t  tempCode1;
	wint_t  tempCode2;
	int halfVaranFound = 0;
	if(shushaUtf16BufIndex < 2)
	{
		return;
	}
	tempCode1 = shushaUtf16Buf[shushaUtf16BufIndex - 1]; // last char in buffer check for `
	tempCode2 = shushaUtf16Buf[shushaUtf16BufIndex - 2]; // vowel to exchange if present

	if ((tempCode1 != 0x0060) || (shushaUtf16BufIndex < 2))
	{
		return;
	}
	// half varan followed by a and then `
	int j;
	j = 0;
	if(shushaUtf16BufIndex > 2) // make sure it has aleast thre chars
	{
		while(halfVaran[j]) // search for match in array of half chars
		{
			if( halfVaran[j] == shushaUtf16Buf[shushaUtf16BufIndex - 3] )
			{
				halfVaranFound = 1;
				break;
			}
			j++;
		}
	}
	if ( (isVowel(2) == 1) &&  (halfVaranFound == 0))// it is  vowel. check for half char
	{
		shushaUtf16Buf[shushaUtf16BufIndex - 2] = tempCode1;
		shushaUtf16Buf[shushaUtf16BufIndex - 1] = tempCode2;
	}
}

//char is not a vowel ि is not considered because it appears before consonent
int isVowel(int indexOffSet)
{
	wint_t tempChar;
	if((shushaUtf16BufIndex - indexOffSet) < 0)
	{
		return 0;
	}
	tempChar = shushaUtf16Buf[shushaUtf16BufIndex - indexOffSet];
	int i = 0;
	while(shushaPostfixVowelList[i])
	{

		if(tempChar == (wint_t)shushaPostfixVowelList[i])
		{
			return 1;
		}
		else
		{
			i++;
		}
	}
	return 0;
}

//utf16 to utf8 conversion. surrogates not handled
void encodeUtf16ToUtf8(wint_t codeToUse)
{
	unsigned char tempByte1;
	unsigned char tempByte2;
	wint_t  tempCode = codeToUse;
	// single byte code. No processing needed
	if(tempCode < 0x0080)
	{
		tempByte1 =  tempCode & 0x007F;
		utf8Buffer[utf8Index++] =  tempByte1;
		//printf("0x%02X ", tempByte1);
		return;
	}
	// two byte code. assemble in two bytes with first byte as 110x xxxx and second byte as 10xx xxxx
	if(tempCode < 0x0800)
	{
		tempByte1 = 0xC0; // start indicator of  first byte of two byte utf8
		tempByte2 = (tempCode >> 6) & 0x1F;
		tempByte1 = tempByte1 | tempByte2;
		utf8Buffer[utf8Index++] =  tempByte1;
		//printf("0x%02X ", tempByte1);

		tempByte1 = 0x80; // start indicator of second byte utf8
		tempByte2 = tempCode  & 0x3F;
		tempByte1 = tempByte1 | tempByte2;
		utf8Buffer[utf8Index++] =  tempByte1;
		//printf("0x%02X ", tempByte1);

		return;
	}
	//No need to check as it is max ffff which can be accomodated in three bytes
	tempByte1 = 0xE0; // start indicator of  first byte of three byte utf8
	tempByte2 = (tempCode >> 12) & 0x0F; // get highest 4 bits 15, 14, 13, 12
	tempByte1 = tempByte1 | tempByte2; // add to first byte with start signature of 0xE0
	utf8Buffer[utf8Index++] =  tempByte1;
	//printf("0x%02X ", tempByte1);

	tempByte1 = 0x80; // start indicator of second byte utf8
	tempByte2 = (tempCode >> 6) & 0x3F;// get bit 11, 10, 9, 8, 7, 6 ie six bits to go with next byte
	tempByte1 = tempByte1 | tempByte2;
	utf8Buffer[utf8Index++] =  tempByte1;
	//printf("0x%02X ", tempByte1);

	tempByte1 = 0x80; // start indicator of second byte utf8
	tempByte2 = tempCode  & 0x3F; // get lowest six bit and add to third byte.
	tempByte1 = tempByte1 | tempByte2;
	utf8Buffer[utf8Index++] =  tempByte1;
	//printf("0x%02X ", tempByte1);
	return;

}
//
int checkswapForM()
{
	// make available next code
	shushaUtf16BufIndex++;
	shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
	wint_t oCurrection = shushaUtf16Buf[shushaUtf16BufIndex + 1];
	switch(shushaUtf16Code)
	{
		case 0x0061: //a
			switch (oCurrection)
			{
			case 0x006F :  // 6F treat it as ो  small o
				hindiUtf16Buf[hindiUtf16BufIndex++] =  VOWEL_SIGN_O;
				shushaUtf16BufIndex++;
				break;
			case 0x004F :  // 4F treat it as ौ capital O
				hindiUtf16Buf[hindiUtf16BufIndex++] =  VOWEL_SIGN_AU;
				shushaUtf16BufIndex++;
				break;
			default :
				hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_AA;
			}
			return 0;
		case 0x0049: //   I   ी   0x0940( ी)
			hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_II ;
			return 0;
		case 0x004F: //ै की मात्रा 0x0948
			hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_AI;
			return 0;
		case 0x006F: ////   o  े    0x0947( े  )
			hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_E;
			return 0;
		case 0x0055: //   U    ू    0x0942( ू )
			hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_UU;
			return 0;
		case 0x0075: //   u   ु  0x0941(   ु  )
			hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_U;
			return 0;
		case 0x0052: //    R     ृ    0x0943(  ृ)
			hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_VOCALIC_R;
			return 0;
		default:
			//printf(" No voval 0x%02x\n", shushaUtf16Code);
			shushaUtf16BufIndex--;
			return 0;
	}
}
//
void singleUnicodeWithNextCammaAndSideR(wint_t codeToUse)
{
	halantChar = 0;
	// make available next code
	shushaUtf16BufIndex++;
	shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
	hindiUtf16Buf[hindiUtf16BufIndex++] = codeToUse;
		if(iFlag)
		{
			if(shushaUtf16Code == COMMA_IS_SIGN_NUKTA) // ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
			{
				hindiUtf16Buf[hindiUtf16BufIndex++] = SIGN_NUKTA;
				shushaUtf16BufIndex++;
				shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
			}
			if(shushaUtf16Code == 0x0060) //     `       ्र   0x094D(्)  0x0930(र )
			{
				hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
				hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_RA;
				shushaUtf16BufIndex++;
				shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
			}

			hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_I ;
			iFlag = 0;
		}
}
//
void nextCodeWithIflagCheck()
{
	if( iFlag)
	{
		hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_I ;
		iFlag = 0;
	}
	halantChar = 0;
	shushaUtf16BufIndex++;
	shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
}
// main fuction with big switch. Can not think any other algorithm
void shushaUtf16ToHindiUtf16()
{
	int iTemp;
	//printf("In code converter\n");
	shushaUtf16BufIndex = 0;
	hindiUtf16BufIndex = 0;
	wint_t oCurrection;
	halantChar = 0;
	// get first code of word being converted
	shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];

	//end of word is nil terminated
	while (shushaUtf16Code != NULL_CHARACTER )
	{
		//
		switch(shushaUtf16Code)
		{
		case EXCLAIMATION_MARK_IS_OM :    // !   ॐ   0x0950
			singleUnicode(DEVANAGARI_OM );
			break;
		case HASHTAG_IS_HALF_KHA : //   #   ख् 0x0916(ख) 0x094D(्)
			doubleUnicode( LETTER_KHA);
			break;
		case DOLLAR_SIGN_IS_RAU : //  $   रू  0x0930(र)  0x0942(ू)
			hindiUtf16Buf[hindiUtf16BufIndex++] = LETTER_RA;
			singleUnicode(VOWEL_SIGN_UU);
			break;
		case PERCENT_SIGN_IS_HALF_TA : //  %  त्   0x0924(त)    0x094D(्)
			doubleUnicode(LETTER_TA);
			break;
		case AMPERSAND_IS_JANYA : //  &  ज्ञ    0x091C(ज)  0x094D(्)  0x091E(ञ)
			hindiUtf16Buf[hindiUtf16BufIndex++] = LETTER_JA;
			hindiUtf16Buf[hindiUtf16BufIndex++] = SIGN_VIRAMA;
			singleUnicode(LETTER_NYA);
			break;
		case LEFT_PARENTHESIS_IS_HAYA : //  (  ह्य   "ह्  य"         0x0939(ह)  0x094D(्) 0x092F(य)
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_HA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_YA;
		    nextCodeWithIflagCheck();
			break;
		case RIGHT_PARENTHESIS_IS_HAVOCALICR : //  )   हृ " ह ृ"    0x0939(ह)  0x0943(ृ)
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_HA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  VOWEL_SIGN_VOCALIC_R;
			nextCodeWithIflagCheck();
			break;
		case ASTERICK_IS_HALF_HA : //  *  ह्   0x0939(ह)  0x094D(्)
			// This does not match with unicode combination. But I don't know what is correct answer.
			doubleUnicode(LETTER_HA);
			break;
		case PLUS_SIGN_IS_HALF_TTATTA: //   +  ट्ट                   0x091F(ट)  0x094D(्)  0x091F(ट)
			singleUnicode(LETTER_TTA);
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_TTA;
			break;
		case COMMA_IS_SIGN_NUKTA: //    ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
			if(halantChar == 1)
			{
				shushaUtf16BufIndex++;
				shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
				if(shushaUtf16Code == 0x0061) // a ा
				{
					shushaUtf16BufIndex++;
					shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
					hindiUtf16BufIndex--;
					hindiUtf16Buf[hindiUtf16BufIndex++] = SIGN_NUKTA;
				}
				else
				{
					shushaUtf16BufIndex--;
					singleUnicode(SIGN_NUKTA);
				}
			}
			else
			{
				//printf("\n(0)0x%02x 0x%02x\n", shushaUtf8InBufIndex , shushaUtf8InBuf[shushaUtf8InBufIndex]);
				if( (shushaUtf16BufIndex == NULL_CHARACTER) || (shushaUtf16Buf[shushaUtf16BufIndex - 1] == COMMA_IS_SIGN_NUKTA)
						|| (shushaUtf16Buf[shushaUtf16BufIndex - 1] == 0x0020))
				{
					singleUnicode(FULL_STOP);
					//printf("\n(1)0x%02x 0x%02x\n", shushaUtf16Buf , shushaUtf16Buf[shushaUtf16BufIndex]);
				}
				else
				{
					singleUnicode(SIGN_NUKTA);
					//printf("\n(2)0x%02x 0x%02x\n", shushaUtf16Buf , shushaUtf16Buf[shushaUtf16BufIndex]);
				}
			}
			halantChar = 0;
			break;
		case HYPHEN_MINUS_IS_HALF_RA: //    -  र् ऊपर अाने वाला जैसे र्र    0x0930(र)  0x094D(्)
			halantChar = 0;
			shushaUtf16BufIndex++;
			shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_RA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			break;
		case FULL_STOP_IS_DANDA: //     . डॉट  । पूर्णविराम            0x0964(।)
			singleUnicode( DANDA);
			break;
		case SLASH_IS_SIGN_VIRAMA_RA: //      /     नीचे अाया र    0x094D(्)  0x0930(र)
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_RA);
			break;
		case COLON_IS_SIGN_VISARGA : //     :    ः   0x0903
			if(shushaUtf16BufIndex == 0)
			{
				singleUnicode(COLON);
			}
			else
			{
				singleUnicode(SIGN_VISARGA);
			}
			break;
		case SEMICOLON_IS_LETTER_VOCALIC_R: //      ;    लृ नहीं पता सही है?  0x090C
			singleUnicode(LETTER_VOCALIC_L);
			break;
		case LESSTHAN_SIGN_IS_HALF_TA: //     <   त्       0x0924(त)  0x094D(्)
			doubleUnicode(LETTER_TA);
			break;
		case EQUAL_SIGN_IS_NGA: //     =   ङ           0x0919(ङ)
			singleUnicode(LETTER_NGA);
			break;
		case GREATERTHAN_IS_HALF_KATA: //     >  क्त          0x0915(क) 0x094D(्)   0x0924(त)
			hindiUtf16Buf[hindiUtf16BufIndex++] = LETTER_KA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_TA);
			break;
		case QUESTIONMARK_IS_LETTERVOCALIC_R: //    ?   ऋ          0x090B(ऋ)
			singleUnicode(LETTER_VOCALIC_R);
			break;
		case AT_SIGN_IS_HALK_KA: // @  क्            0x0915(क) 0x094D(्)
			doubleUnicode(LETTER_KA);
			break;
		case LATIN_CAP_A_IS_LETTER_A: //  A  अ           0x0905(अ)
			singleUnicode(LETTER_A);
			break;
		case 0x0042: //  B   भ्          0x092D(भ)  0x094D(्)
			doubleUnicode(LETTER_BHA);
			break;
		case 0x0043: //   C  छ         0x091B(छ)
			singleUnicodeWithNextCammaAndSideR(LETTER_CHA);
			break;
		case 0x0044: //   D  ड         0x0921(ड)
			singleUnicodeWithNextCammaAndSideR(LETTER_DDA);
			break;
		case 0x0045: //    E   श्र्      0x0936(श)  0x094D(्)  0x0930(र)  0x094D(्)
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_SHA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			doubleUnicode(LETTER_RA);
			break;
		case 0x0046: //    F    फ्     0x092B(फ)  0x094D(्)
			doubleUnicode(LETTER_PHA);
			break;
		case 0x0047: //    G   घ्    0x0918(घ)  0x094D(्)
			doubleUnicode(LETTER_GHA);
			break;
		case 0x0048: //    H  ञ    0x091E(ञ)
			singleUnicode(LETTER_NYA);
			break;
		case 0x0049: //   I   ी   0x0940( ी)
			singleUnicode(VOWEL_SIGN_II );
			break;
		case 0x004A: //   J    झ्     0x091D(झ)  0x094D(्)
			doubleUnicode(LETTER_JHA);
			break;
		case 0x004B: //  K   ख     0x0916(ख)
			singleUnicodeWithNextCammaAndSideR(LETTER_KHA);
			break;
		case 0x004C:  // L   ळ     0x0933( ळ)
			singleUnicode(LETTER_KHA);
			break;
		case 0x004D: //    M ं     0x0902( ं)
			if(checkswapForM() == 0)
			{
				if(iFlag)
					{
						hindiUtf16Buf[hindiUtf16BufIndex++] = VOWEL_SIGN_I ;
						iFlag = 0;
					}
				singleUnicode(SIGN_ANUSUVRA);
			}
			break;
		case 0x004E: //   N  ण्   0x0923(ण)  0x094D(्)
			doubleUnicode(LETTER_NNA);
			break;
		case 0x004F: //    O   ै  0x0948(  ै)
			singleUnicode(VOWEL_SIGN_AI);
			break;
		case 0x0050: //   P    प्   0x092A(प)  0x094D(्)
			doubleUnicode(LETTER_PA );
			break;
		case 0x0051: //    Q  ध्     0x0927(ध)  0x094D(्)
			doubleUnicode(LETTER_DHA);
			break;
		case 0x0052: //    R     ृ    0x0943(  ृ)
			singleUnicode(VOWEL_SIGN_VOCALIC_R);
			break;
		case 0x0053: //    S    श्    0x0936(श)  0x094D(्)
			doubleUnicode(LETTER_SHA);
			break;
		case 0x0054: //   T     ट     0x091F(   ट )
			singleUnicodeWithNextCammaAndSideR(LETTER_TTA);
			break;
		case 0x0055: //   U    ू    0x0942( ू )
			singleUnicode(VOWEL_SIGN_UU);
			break;
		case 0x0056: //    V   द्य    "द् य"    0x0926(द)  0x094D(्)  0x092F(य )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_DA ;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_YA);
			break;
		case 0x0057: //    W     द्व       0x0926(द)  0x094D(्)  0x0935(व )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_DA ;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_VA );
			break;
		case 0x0058: //    X श्   0x0936(श)  0x094D(्) पक्का नहीं है जो कर रहे है सही है या नहीं क्योंकि S भी यही कर रहा है।
			doubleUnicode(LETTER_SHA);
			break;
		case 0x0059: //   Y  ष्      0x0937( ष् )
			doubleUnicode(LETTER_SSA );
			break;
		case 0x005A: //   Z   ढ    0x0922( ढ  )
			singleUnicodeWithNextCammaAndSideR(LETTER_DDHA );
			break;
		case 0x005B: //  [      इ   0x0907( इ  )  [- ई
			if(shushaUtf16Buf[shushaUtf16BufIndex + 1] == HYPHEN_MINUS_IS_HALF_RA)
			{
				shushaUtf16BufIndex++;
				singleUnicode(LETTER_II );
			}
			else
			{
				singleUnicode(LETTER_I);
			}
			break;
		case 0x005C: //  \      ्   0x094D(  ्  )
			singleUnicode(SIGN_VIRAMA);
			break;
		case 0x005D: //  ]   उ  0x0909(   उ  )
			singleUnicode(LETTER_U);
			break;
		case 0x005E: //  ^   ॅ    0x0945(  ॅ  )
			singleUnicode(VOWEL_SIGN_CHANDRA_E);
			break;
		case 0x005F: //  _   द्द    द् द  0x0926(द)  0x094D(्)  0x0926(द )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_DA ;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_DA );
			break;
		case 0x0060: //     `       ्र   0x094D(्)  0x0930(र )
			iTemp = iFlag;
			if(hindiUtf16Buf[hindiUtf16BufIndex - 1] == SIGN_VIRAMA)
			{
				singleUnicode(LETTER_RA);
			}
			else
			{
				hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
				singleUnicode(LETTER_RA);
			}
			if( (iTemp == 1) && (isVowel(0) == 1))
			{
				shushaUtf16BufIndex++;
				shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
			}
			break;
		case 0x0061: //    a   ा     0x093E( ा )
			oCurrection = shushaUtf16Buf[shushaUtf16BufIndex + 1];
			switch (oCurrection)
			{
			case 'o' :  // 6F treat it as ो
				hindiUtf16Buf[hindiUtf16BufIndex++] =  VOWEL_SIGN_O;
				shushaUtf16BufIndex++;
				shushaUtf16BufIndex++;
				shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
				break;
			case 'O' :  // 6F treat it as ो
				hindiUtf16Buf[hindiUtf16BufIndex++] =  VOWEL_SIGN_AU;
				shushaUtf16BufIndex++;
				shushaUtf16BufIndex++;
				shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
				break;
			default :
				singleUnicode(VOWEL_SIGN_AA);
			}
			break;
		case 0x0062: //    b    ब्   0x092C(ब)  0x094D(्)
			doubleUnicode(LETTER_BA);
			break;
		case 0x0063: //   c   च्    0x091A(च)  0x094D(्)
			doubleUnicode(LETTER_CA);
			break;
		case 0x0064: //     d  द   0x0926( द)
			singleUnicodeWithNextCammaAndSideR(LETTER_DA );
			break;
		case 0x0065: //    e   ए   0x090F(ए )
			singleUnicode(LETTER_E);
			break;
		case 0x0066: //    f   फ   0x092B(फ)
			singleUnicodeWithNextCammaAndSideR(LETTER_PHA);
			break;
		case 0x0067: //    g   ग्   0x0917(ग)  0x094D(्)
			doubleUnicode(LETTER_GA);
			break;
		case 0x0068: //    h    ह    0x0939( ह  )
			singleUnicode(LETTER_HA);
			break;
		case 0x0069: //    i   ि  0x093F( ि  )
			//singleUnicode(VOWEL_SIGN_I );
			iFlag =  1;
			shushaUtf16BufIndex++;
			shushaUtf16Code = shushaUtf16Buf[shushaUtf16BufIndex];
			break;
		case 0x006A: //   j   ज्     0x091C(ज)  0x094D(्)
			doubleUnicode(LETTER_JA);
			break;
		case 0x006B: //   k  क    0x0915(क )
			singleUnicodeWithNextCammaAndSideR(LETTER_KA);
			break;
		case 0x006C: //   l ल्   0x0932(ल)  0x094D(्)
			doubleUnicode(LETTER_LA);
			break;
		case 0x006D: //  m म्    0x092E(म)  0x094D(्)
			doubleUnicode(LETTER_MA);
			break;
		case 0x006E: //   n   न्     0x0928(न)  0x094D(्)
			doubleUnicode(LETTER_NA);
			break;
		case 0x006F: //   o  े    0x0947( े  )
			singleUnicode(VOWEL_SIGN_E);
			break;
		case 0x0070: //   p  प      0x092A(  प )
			singleUnicodeWithNextCammaAndSideR(LETTER_PA );
			break;
		case 0x0071: //   q   थ्    0x0925(थ)  0x094D(्)
			doubleUnicode(LETTER_THA);
			break;
		case 0x0072: //    r  र     0x0930(  र  )
			singleUnicode(LETTER_RA);
			break;
		case 0x0073: //   s  स्     0x0938(स)  0x094D(्)
			doubleUnicode(LETTER_SA);
			break;
		case 0x0074: //   t   त   0x0924(  त  )
			singleUnicodeWithNextCammaAndSideR(LETTER_TA);
			break;
		case 0x0075: //   u   ु  0x0941(   ु  )
			singleUnicode(VOWEL_SIGN_U);
			break;
		case 0x0076: //  v   व्  0x0935(व)  0x094D(्)
			doubleUnicode(LETTER_VA );
			break;
		case	0x0077: //   w  द्ध  0x0926(द)  0x094D(्)  0x0927(ध)
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_DA ;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_DHA);
			break;
		case	0x0078: //  x क्ष्   0x0915(क)  0x094D(्)  0x0937(ष)  0x094D(्)
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_KA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			doubleUnicode(LETTER_SSA );
			break;
		case 0x0079: //   y  य्  0x092F(य)  0x094D(्)
			doubleUnicode(LETTER_YA);
			break;
		case 0x007A: //  z  ठ      0x0920( ठ  )
			singleUnicode(LETTER_TTHA);
			break;
		case 0x007B: //    {    ठ्ठ  0x0920( ठ  )  0x094D(्)   0x0920( ठ  )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_TTHA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_TTHA);
			break;
		case 0x007C: //    |   अोम मे लगने वाला S जैसा संकेत 0x093D(   )
			singleUnicode(SIGN_AVAGRAHA);
			break;
		case 0x007D: //    }   ऊ   0x090A( ऊ  )
			singleUnicode(LETTER_UU);
			break;
		case 0x007E: //     ~  त्र  0x0924( त  )  0x094D(्)   0x0930( र  )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_TA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_RA);
			break;
		case 0x00B2: //!  178     0xD1 dec 0209
			singleUnicode(EXCLAMATION_MARK);
			break;
		case 0x00B8: // , comma   184
			singleUnicode(COMMA );
			break;
		case 0x00BA: // 186 0xC2 0xBA   full stop . 0x002E
			singleUnicode(FULL_STOP);
			break;
		case 0x00B9: // hyphen -
			singleUnicode(HYPHEN_MINUS);
			break;
		case 0x00B3: //(  0xb3 dec 179
			singleUnicode(0x0028);
			break;
		case 0x00B4: //)  0xb4 dec 180
			singleUnicode(0x0029);
			break;
		case 0x00C0: //192 Slash(/) 0x002F C0
			singleUnicode(SLASH);
			break;
		case 0x00C2: //194 semicolon ; 0x003B C2
			singleUnicode(SEMICOLON);
			break;
		case 0x00C4: //196 equal = 0x003D  C4
			singleUnicode(EQUAL_SIGN);
			break;
		case 0x00C6: // ? 0xC6 dec 0198
			singleUnicode(QUESTION_MARK);
			break;
		case 0x00C9: // रु 191 0x0930(  र  )   0x0942(ु)
			singleUnicode(LETTER_RA);
			hindiUtf16Buf[hindiUtf16BufIndex++] =  VOWEL_SIGN_UU;
			break;
		case 0x00CB :  //0x0915(क ) 0x094D(्) 0x0930(  र  )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_KA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_RA);
			break;
		case 0x00CC :  //0x0915(क ) 0x094D(्) 0x0930(  र  )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_KA;
			singleUnicode(VOWEL_SIGN_VOCALIC_R);
					break;
		case 0x00CD :  // 205 cd    0x092B(फ ) 0x094D(्) 0x0930(  र  )
			hindiUtf16Buf[hindiUtf16BufIndex++] =  LETTER_PHA;
			hindiUtf16Buf[hindiUtf16BufIndex++] =  SIGN_VIRAMA;
			singleUnicode(LETTER_RA);
			break;
		case	0x00D0 : //208  0xD0   ँ चन्द्र बिन्दू  0x901(ँ)
			singleUnicode(SIGN_CHANDRABINDU);
			break;
		case 0x00D1: //!  0xD1 dec 0209
			singleUnicode(EXCLAMATION_MARK);
			break;
		case 0x00D4: //फ़  0xD4 dec 0212
			singleUnicode(LETTER_FA );
			break;
		default:
			singleUnicode(shushaUtf16Code); // no conversion needed for full rwo byte code
			break;
		}
	}
	hindiUtf16Buf[hindiUtf16BufIndex] = NULL_CHARACTER;
	return ;
}

// end of file

