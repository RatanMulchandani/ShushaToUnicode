/* Name: uhindi.c
 *
 * Author : Ratan Mulchandani
 *
 * start date: 27 Jan 2021
 * second rewritten version 14 feb 2021
 * version 0.5
 *
 *
 */

#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>      /* wint_t */
#include <ctype.h>
#include <stdlib.h>

// prototypes
void convertShushToUtf16();
void encodeToUtf8(wint_t codeToUse);
void adjustForTopR();
void adjustForSideR();
int isVowel(int indexOffSet);
void shushaToUnicode();
void doubleUnicode(wint_t codeToUse);
void singleUnicode(wint_t codeToUse);
int checkswapForM();
void singleUnicodeWithNextCamma(wint_t codeToUse);
void nextCodeWithIflagCheck();


//read shusha character one word at a time end of word is nil terminated. It is utf8 coded
unsigned char shushaBuf[500]; // code from file
int shushaIndex;

// convert shusha utf8 to shusha utf16. Nil teminated
wint_t shushaUtf16[1000]; // file byte code to utf16 but still shusha code
int shushaUtf16Index;

// devnagiri utf16,  converted code from shushutf16. Nil teminated
wint_t unicodeBuf[1000];// utf16 converted to standard devnagari utf16
int unicodeIndex;

// final devnagari utf8 code.. Nil teminated
unsigned char utf8Buffer[2000]; // devnagari utf8 code
int utf8Index;

// shusha vowel list
unsigned char shushaVowelList[] = "aIOoUu,"; // small i is prefix vowel or comma.
unsigned char halfVaran[] = "%#HYyxXvsSqQnNmljJgGcbBE@<F";
int iFlag; // small ि adjustment indicator
int halantChar = 0;
wint_t shushaUtf16Code;
int wordCount;


//
int main(int argc, char *argv[])
{
	//needed to display unicode characters
	setlocale(LC_ALL, ""); // unicode display requirement

	// minimum input file is needed
	if( argc < 2)
	{
		printf("Missing input file\n");
		return 1;
	}
// read mode open is needed
	FILE *inFp = fopen(argv[1], "r");
	if(inFp == NULL)
	{
		printf("Unable to open input file\n");
		return 1;
	}

// read full file but one word at time.
     wordCount = 0;
	// end of file indicator to terminate input file reading loop
	 int doneFlag = 0;

	 //current read byte from file
	 int charRead;

	 //read full file till eof. eof indicated flag
	 while(doneFlag == 0)
	 {
		 // collect word with leading white spaces
		 // index to input shusha code word collecting buffer initialised and buffer nil teminated
		 shushaIndex = 0;
		 shushaBuf[shushaIndex] = 0x00;
		 //read leading white spaces
		 while(doneFlag == 0)
		 {
			 charRead = fgetc(inFp);
			 if(charRead == EOF)
			 {
				 doneFlag = 1;
				 break;
			 }
			 shushaBuf[shushaIndex++] = charRead;
			 shushaBuf[shushaIndex] = 0x00;
			 if(isspace(charRead) == 0) // start of word with alphabet
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
			shushaBuf[shushaIndex++] = charRead;
			shushaBuf[shushaIndex] = 0x00;
	 		 if(isspace(charRead) != 0)
	 		 {
				 break;
	 		 }
		 }
		 // convert read shusha utf8 codes in to utf16 shusha code
		 convertShushToUtf16();

		 // this fuction converts shusha hindi code to unicode 16 bit hindicode one word at a time
		shushaToUnicode();

		// to uf8 from devnagari unicode
		int outIndex;
		// unicode to utf8 convertion only hindi
		outIndex = 0;
		utf8Index = 0;
		while(unicodeBuf[outIndex] != 0x0000)
		{
			wint_t shushaUtf16CodeTemp;
			// correct for आ ओ औ
			shushaUtf16Code = unicodeBuf[outIndex];
			//combined few devnagari varan with matra
			switch(shushaUtf16Code)
			{
			case 0x0905:  // अ followed by ा ो ौ
				shushaUtf16CodeTemp = unicodeBuf[outIndex + 1];
				switch(shushaUtf16CodeTemp)
				{
				case 0x093E: // it is to replaced with आ
					shushaUtf16Code = 0x0906;
					outIndex++;
					shushaUtf16CodeTemp = unicodeBuf[outIndex + 1];
					if(shushaUtf16CodeTemp == 0x0945) // if ॅ this then change to ॉ
					{
						shushaUtf16Code = 0x0911;
						outIndex++;
					}
					break;
				case 0x094B: // it is ओ
					shushaUtf16Code = 0x0913;
					outIndex++;
					break;
				case 0x094C: // it is औ
					shushaUtf16Code = 0x0914;
					outIndex++;
					break;
				default:
					break;
				}
			case 0x093E:
				shushaUtf16CodeTemp = unicodeBuf[outIndex + 1];
				if(shushaUtf16CodeTemp == 0x0945) // if ॅ this then change to ॉ
				{
					shushaUtf16Code = 0x0949; // convert to ॉ
					outIndex++;
				}
				break;
			case 0x090F: // ै ए े  correction
				shushaUtf16CodeTemp = unicodeBuf[outIndex + 1];
				if( (shushaUtf16CodeTemp == 0x0947) || (shushaUtf16CodeTemp == 0x0948))// if ॅ this then change to ॉ
				{
					shushaUtf16Code = 0x0910; // convert to एे
					outIndex++;
				}
				break;
			default:
				break;

			}
			encodeToUtf8(shushaUtf16Code);
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
	shushaUtf16Index++;
	shushaUtf16Code = shushaUtf16[shushaUtf16Index];
// save the converted unicode
	unicodeBuf[unicodeIndex++] = codeToUse;
	if(iFlag)
	{
		unicodeBuf[unicodeIndex++] = 0x093F; // This is ि मात्रा   कोड
		iFlag = 0;
	}
}
//
void doubleUnicode(wint_t codeToUse)
{
	halantChar = 0;
	unicodeBuf[unicodeIndex++] = codeToUse;
	// make available next code
	shushaUtf16Index++;
	shushaUtf16Code = shushaUtf16[shushaUtf16Index];
	if(shushaUtf16Code == 0x0061) // a ा
	{
		shushaUtf16Index++;
		shushaUtf16Code = shushaUtf16[shushaUtf16Index];
		if(shushaUtf16Code == 0x002C) // ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
		{
			unicodeBuf[unicodeIndex++] = 0x093C;
			shushaUtf16Index++;
			shushaUtf16Code = shushaUtf16[shushaUtf16Index];
		}
		if(shushaUtf16Code == 0x0060) //   `       ्र   0x094D(्)  0x0930(र )
		{
			unicodeBuf[unicodeIndex++] = 0x094D;
			unicodeBuf[unicodeIndex++] = 0x0930;
			shushaUtf16Index++;
			shushaUtf16Code = shushaUtf16[shushaUtf16Index];
		}
	}
	else
	{
		unicodeBuf[unicodeIndex++] = 0x094D;
		halantChar = 1;// it is half varan
	}
    // ि की मात्रा  तो नहीं लगानी?
	if( (iFlag) && (unicodeBuf[unicodeIndex - 1] != 0x094D))
	{
		unicodeBuf[unicodeIndex++] = 0x093F;
		iFlag = 0;
	}
}
// index in input to shusha to utf16 converted word, intialised and buffer nil terminated
void convertShushToUtf16()
{
	unsigned char currentByte;
	unsigned char secondByte;
	unsigned char tempByte1;
	unsigned char tempByte2;
	unsigned char tempByte3;
	wint_t currentUtf16;

	shushaUtf16Index = 0x00;
	shushaUtf16[shushaUtf16Index] = 0x0000;

	int i = 0;
	currentByte = shushaBuf[i];
	 while( currentByte) // nil byte end of word
	 {
		 // check if it has 8th bit zero. if yes then it is seven bit ascii
		 if((currentByte  & 0x80) == 0x00 ) // it is single byte
		 {
			 currentUtf16 = 0x0000 + currentByte;
			 shushaUtf16[shushaUtf16Index++] = currentUtf16;
			 shushaUtf16[shushaUtf16Index] = 0x0000;
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
				 secondByte = shushaBuf[i];
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
				 shushaUtf16[shushaUtf16Index++] = currentUtf16;
				 shushaUtf16[shushaUtf16Index] = 0x0000;
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
					 secondByte = shushaBuf[i];
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
					 secondByte = shushaBuf[i];
					 // get last six bits (5,4,3,2,1,0) of ls byte
					 tempByte2 = secondByte & 0x3F;
					 // combined both to get all 8 bits of ls byte
					 tempByte3 = tempByte3 | tempByte2;
					 // merge both bytes to get full utf16 code
					 currentUtf16 = currentUtf16 | tempByte3;

					 // save the same
					 shushaUtf16[shushaUtf16Index++] = currentUtf16;
					 shushaUtf16[shushaUtf16Index]   = 0x0000;
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
		currentByte = shushaBuf[i];
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
	if(shushaUtf16Index < 2)
	{
		return;
	}
	tempCode1 = shushaUtf16[shushaUtf16Index - 1]; // last char in buffer check for -
	tempCode2 = shushaUtf16[shushaUtf16Index - 2]; // next last check for [
	if( (tempCode1 == 0x002D) && (tempCode2 == 0x005B)) // will be handled in unicode devnagari code conversion
	{
		return;
	}
	tempCount = 2; // this offset is where we want to search back till we find character without ha
	if ((tempCode1 != 0x002D) || (shushaUtf16Index < 2))
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
				if( halfVaran[j] == shushaUtf16[shushaUtf16Index - (tempCount + 1)] )
				{
					 tempCount++;
					 break;
				}
				 j++;
			 }
			for(int i = 2; i <= tempCount; i++)
			{
				shushaUtf16[shushaUtf16Index  - (i - 1)] = shushaUtf16[shushaUtf16Index  - i];
			}
			shushaUtf16[shushaUtf16Index  - tempCount] = 0x002D;
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
	if(shushaUtf16Index < 2)
	{
		return;
	}
	tempCode1 = shushaUtf16[shushaUtf16Index - 1]; // last char in buffer check for `
	tempCode2 = shushaUtf16[shushaUtf16Index - 2]; // vowel to exchange if present

	if ((tempCode1 != 0x0060) || (shushaUtf16Index < 2))
	{
		return;
	}
	// half varan followed by a and then `
	int j;
	j = 0;
	if(shushaUtf16Index > 2) // make sure it has aleast thre chars
	{
		while(halfVaran[j]) // search for match in array of half chars
		{
			if( halfVaran[j] == shushaUtf16[shushaUtf16Index - 3] )
			{
				halfVaranFound = 1;
				break;
			}
			j++;
		}
	}
	if ( (isVowel(2) == 1) &&  (halfVaranFound == 0))// it is  vowel. check for half char
	{
		shushaUtf16[shushaUtf16Index - 2] = tempCode1;
		shushaUtf16[shushaUtf16Index - 1] = tempCode2;
	}
}

//char is not a vowel ि is not considered because it appears before consonent
int isVowel(int indexOffSet)
{
	wint_t tempChar;
	if((shushaUtf16Index - indexOffSet) < 0)
	{
		return 0;
	}
	tempChar = shushaUtf16[shushaUtf16Index - indexOffSet];
	int i = 0;
	while(shushaVowelList[i])
	{

		if(tempChar == (wint_t)shushaVowelList[i])
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
void encodeToUtf8(wint_t codeToUse)
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
	shushaUtf16Index++;
	shushaUtf16Code = shushaUtf16[shushaUtf16Index];
	wint_t oCurrection = shushaUtf16[shushaUtf16Index + 1];
	switch(shushaUtf16Code)
	{
		case 0x0061: //a
			switch (oCurrection)
			{
			case 0x006F :  // 6F treat it as ो  small o
				unicodeBuf[unicodeIndex++] =  0x094B;
				shushaUtf16Index++;
				break;
			case 0x004F :  // 4F treat it as ौ capital O
				unicodeBuf[unicodeIndex++] =  0x094C;
				shushaUtf16Index++;
				break;
			default :
				unicodeBuf[unicodeIndex++] = 0x093E;
			}
			return 0;
		case 0x0049: //   I   ी   0x0940( ी)
			unicodeBuf[unicodeIndex++] = 0x0940;
			return 0;
		case 0x004F: //ै की मात्रा 0x0948
			unicodeBuf[unicodeIndex++] = 0x0948;
			return 0;
		case 0x006F: ////   o  े    0x0947( े  )
			unicodeBuf[unicodeIndex++] = 0x0947;
			return 0;
		case 0x0055: //   U    ू    0x0942( ू )
			unicodeBuf[unicodeIndex++] = 0x0942;
			return 0;
		case 0x0075: //   u   ु  0x0941(   ु  )
			unicodeBuf[unicodeIndex++] = 0x0941;
			return 0;
		case 0x0052: //    R     ृ    0x0943(  ृ)
			unicodeBuf[unicodeIndex++] = 0x0943;
			return 0;
		default:
			//printf(" No voval 0x%02x\n", shushaUtf16Code);
			shushaUtf16Index--;
			return 0;
	}
}
//
void singleUnicodeWithNextCamma(wint_t codeToUse)
{
	halantChar = 0;
	// make available next code
	shushaUtf16Index++;
	shushaUtf16Code = shushaUtf16[shushaUtf16Index];
	unicodeBuf[unicodeIndex++] = codeToUse;
		if(iFlag)
		{
			if(shushaUtf16Code == 0x002C) // ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
			{
				unicodeBuf[unicodeIndex++] = 0x093C;
				shushaUtf16Index++;
				shushaUtf16Code = shushaUtf16[shushaUtf16Index];
			}
			if(shushaUtf16Code == 0x60) //     `       ्र   0x094D(्)  0x0930(र )
			{
				unicodeBuf[unicodeIndex++] =  0x094D;
				unicodeBuf[unicodeIndex++] =  0x0930;
				shushaUtf16Index++;
				shushaUtf16Code = shushaUtf16[shushaUtf16Index];
			}

			unicodeBuf[unicodeIndex++] = 0x093F;
			iFlag = 0;
		}
}
//
void nextCodeWithIflagCheck()
{
	if( iFlag)
	{
		unicodeBuf[unicodeIndex++] = 0x093F;
		iFlag = 0;
	}
	halantChar = 0;
	shushaUtf16Index++;
	shushaUtf16Code = shushaUtf16[shushaUtf16Index];
}
// main fuction with big switch. Can not think any other algorithm
void shushaToUnicode()
{
	int iTemp;
	//printf("In code converter\n");
	shushaUtf16Index = 0;
	unicodeIndex = 0;
	wint_t oCurrection;
	halantChar = 0;
	// get first code of word being converted
	shushaUtf16Code = shushaUtf16[shushaUtf16Index];

	//end of word is nil terminated
	while (shushaUtf16Code != 0x0000 )
	{
		//
		switch(shushaUtf16Code)
		{
		case 0x0021:    // !   ॐ   0x0950
			singleUnicode(0x0950);
			break;
		case 0x0023: //   #   ख्                 0x0916(ख) 0x094D(्)
			doubleUnicode( 0x0916);
			break;
		case 0x0024: //  $   रू                    0x0930(र)  0x0942(ू)   रू
			unicodeBuf[unicodeIndex++] = 0x0930;
			singleUnicode(0x0942);
			break;
		case 0x0025 : //  %  त्                     0x0924(त)    0x094D(्)
			doubleUnicode(0x0924);
			break;
		case 0x0026: //  &  ज्ञ                     0x091C(ज)  0x094D(्)   0x091E(ञ)  ज्ञ
			unicodeBuf[unicodeIndex++] = 0x091C;
			unicodeBuf[unicodeIndex++] = 0x094D;
			singleUnicode(0x091E);
			break;
		case 0x0028: //  (  ह्य   "ह्  य"         0x0939(ह)  0x094D(्) 0x092F(य)
			unicodeBuf[unicodeIndex++] =  0x0939;
			unicodeBuf[unicodeIndex++] =  0x094D;
			unicodeBuf[unicodeIndex++] =  0x092F;
		    nextCodeWithIflagCheck();
			break;
		case 0x0029: //  )   हृ " ह ृ"           0x0939(ह)  0x0943(ृ)
			unicodeBuf[unicodeIndex++] =  0x0939;
			unicodeBuf[unicodeIndex++] =  0x0943;
			nextCodeWithIflagCheck();
			break;
		case 0x002A: //  *  ह्                     0x0939(ह)  0x094D(्)
			doubleUnicode(0x0939);
			break;
		case 0x002B: //   +  ट्ट                   0x091F(ट)  0x094D(्)  0x091F(ट)
			singleUnicode(0x091F);
			unicodeBuf[unicodeIndex++] =  0x094D;
			unicodeBuf[unicodeIndex++] =  0x091F;
			break;
		case 0x002C: //    ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
			if(halantChar == 1)
			{
				shushaUtf16Index++;
				shushaUtf16Code = shushaUtf16[shushaUtf16Index];
				if(shushaUtf16Code == 0x61) // a ा
				{
					shushaUtf16Index++;
					shushaUtf16Code = shushaUtf16[shushaUtf16Index];
					unicodeIndex--;
					unicodeBuf[unicodeIndex++] = 0x093C;
				}
				else
				{
					shushaUtf16Index--;
					singleUnicode(0x093C);
				}
			}
			else
			{
				//printf("\n(0)0x%02x 0x%02x\n", shushaIndex , shushaBuf[shushaIndex]);
				if( (shushaUtf16Index == 0x0000) || (shushaUtf16[shushaUtf16Index - 1] == 0x002C) || (shushaUtf16[shushaUtf16Index - 1] == 0x0020))
				{
					singleUnicode(0x002E);
					//printf("\n(1)0x%02x 0x%02x\n", shushaUtf16 , shushaUtf16[shushaUtf16Index]);
				}
				else
				{
					singleUnicode(0x093C);
					//printf("\n(2)0x%02x 0x%02x\n", shushaUtf16 , shushaUtf16[shushaUtf16Index]);
				}
			}
			halantChar = 0;
			break;
		case 0x002D: //    -  र् ऊपर अाने वाला जैसे र्र    0x0930(र)  0x094D(्)
			halantChar = 0;
			shushaUtf16Index++;
			shushaUtf16Code = shushaUtf16[shushaUtf16Index];
			unicodeBuf[unicodeIndex++] =  0x0930;
			unicodeBuf[unicodeIndex++] =  0x094D;
			break;
		case 0x002E: //     . डॉट  । पूर्णविराम            0x964(।)
			singleUnicode(0x0964);
			break;
		case 0x002F: //      /   ( ट्र  ट)   नीचे अाया र     0x091F(ट)  0x094D(्)  0x0930(र)
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0930);
			break;
		case 0x003A: //     :    ः   0x0903
			if(shushaUtf16Index == 0x0000)
			{
				singleUnicode(0x003A);
			}
			else
			{
				singleUnicode(0x0903);
			}
			break;
		case 0x003B: //      ;    लृ नहीं पता सही है?  0x090C
			singleUnicode(0x090C);
			break;
		case 0x003C: //     <   त्       0x0924(त)  0x094D(्)
			doubleUnicode(0x0924);
			break;
		case 0x003D: //     =   ङ           0x0919(ङ)
			singleUnicode(0x0919);
			break;
		case 0x003E: //     >  क्त          0x915(क) 0x094D(्)   0x0924(त)
			unicodeBuf[unicodeIndex++] = 0x0915;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0924);
			break;
		case 0x003F: //    ?   ऋ          0x90B(ऋ)
			singleUnicode(0x090B);
			break;
		case 0x0040: // @  क्            0x915(क) 0x094D(्)
			doubleUnicode(0x0915);
			break;
		case 0x0041: //  A  अ           0x905(अ)
			singleUnicode(0x0905);
			break;
		case 0x0042: //  B   भ्          0x092D(भ)  0x094D(्)
			doubleUnicode(0x092D);
			break;
		case 0x0043: //   C  छ         0x091B(छ)
			singleUnicodeWithNextCamma(0x091B);
			break;
		case 0x0044: //   D  ड         0x0921(ड)
			singleUnicodeWithNextCamma(0x0921);
			break;
		case 0x0045: //    E   श्र्      0x936(श)  0x094D(्)  0x930(र)  0x094D(्)
			unicodeBuf[unicodeIndex++] =  0x0936;
			unicodeBuf[unicodeIndex++] =  0x094D;
			doubleUnicode(0x0930);
			break;
		case 0x0046: //    F    फ्     0x092B(फ)  0x094D(्)
			doubleUnicode(0x092B);
			break;
		case 0x0047: //    G   घ्    0x0918(घ)  0x094D(्)
			doubleUnicode(0x0918);
			break;
		case 0x0048: //    H  ञ    0x091E(ञ)
			singleUnicode(0x091E);
			break;
		case 0x0049: //   I   ी   0x0940( ी)
			singleUnicode(0x0940);
			break;
		case 0x004A: //   J    झ्     0x091D(झ)  0x094D(्)
			doubleUnicode(0x091D);
			break;
		case 0x004B: //  K   ख     0x0916(ख)
			singleUnicodeWithNextCamma(0x0916);
			break;
		case 0x004C:  // L   ळ     0x0933( ळ)
			singleUnicode(0x0933);
			break;
		case 0x004D: //    M ं     0x0902( ं)
			if(checkswapForM() == 0)
			{
				if(iFlag)
					{
						unicodeBuf[unicodeIndex++] = 0x093F;
						iFlag = 0;
					}
				singleUnicode(0x0902);
			}
			break;
		case 0x004E: //   N  ण्   0x0923(ण)  0x094D(्)
			doubleUnicode(0x0923);
			break;
		case 0x004F: //    O   ै  0x0948(  ै)
			singleUnicode(0x0948);
			break;
		case 0x0050: //   P    प्   0x092A(प)  0x094D(्)
			doubleUnicode(0x092A);
			break;
		case 0x0051: //    Q  ध्     0x0927(ध)  0x094D(्)
			doubleUnicode(0x0927);
			break;
		case 0x0052: //    R     ृ    0x0943(  ृ)
			singleUnicode(0x0943);
			break;
		case 0x0053: //    S    श्    0x0936(श)  0x094D(्)
			doubleUnicode(0x0936);
			break;
		case 0x0054: //   T     ट     0x091F(   ट )
			singleUnicodeWithNextCamma(0x091F);
			break;
		case 0x0055: //   U    ू    0x0942( ू )
			singleUnicode(0x0942);
			break;
		case 0x0056: //    V   द्य    "द् य"    0x0926(द)  0x094D(्)  0x092F(य )
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x092F);
			break;
		case 0x0057: //    W     द्व       0x0926(द)  0x094D(्)  0x0935(व )
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0935);
			break;
		case 0x0058: //    X श्   0x0936(श)  0x094D(्) पक्का नहीं है जो कर रहे है सही है या नहीं क्योंकि S भी यही कर रहा है।
			doubleUnicode(0x0936);
			break;
		case 0x0059: //   Y  ष्      0x0937( ष् )
			doubleUnicode(0x0937);
			break;
		case 0x005A: //   Z   ढ    0x0922( ढ  )
			singleUnicodeWithNextCamma(0x0922);
			break;
		case 0x005B: //  [      इ   0x0907( इ  )  [- ई
			if(shushaUtf16[shushaUtf16Index + 1] == 0x002D)
			{
				shushaUtf16Index++;
				singleUnicode(0x0908);
			}
			else
			{
				singleUnicode(0x0907);
			}
			break;
		case 0x005C: //  \      ्   0x094D(  ्  )
			singleUnicode(0x094D);
			break;
		case 0x005D: //  ]   उ  0x0909(   उ  )
			singleUnicode(0x0909);
			break;
		case 0x005E: //  ^   ॅ    0x0945(  ॅ  )
			singleUnicode(0x0945);
			break;
		case 0x005F: //  _   द्द    द् द  0x0926(द)  0x094D(्)  0x0926(द )
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0926);
			break;
		case 0x0060: //     `       ्र   0x094D(्)  0x0930(र )
			iTemp = iFlag;
			if(unicodeBuf[unicodeIndex - 1] == 0x094D)
			{
				singleUnicode(0x0930);
			}
			else
			{
				unicodeBuf[unicodeIndex++] =  0x094D;
				singleUnicode(0x0930);
			}
			if( (iTemp == 1) && (isVowel(0) == 1))
			{
				shushaUtf16Index++;
				shushaUtf16Code = shushaUtf16[shushaUtf16Index];
			}
			break;
		case 0x0061: //    a   ा     0x093E( ा )
			oCurrection = shushaUtf16[shushaUtf16Index + 1];
			switch (oCurrection)
			{
			case 'o' :  // 6F treat it as ो
				unicodeBuf[unicodeIndex++] =  0x094B;
				shushaUtf16Index++;
				shushaUtf16Index++;
				shushaUtf16Code = shushaUtf16[shushaUtf16Index];
				break;
			case 'O' :  // 6F treat it as ो
				unicodeBuf[unicodeIndex++] =  0x094C;
				shushaUtf16Index++;
				shushaUtf16Index++;
				shushaUtf16Code = shushaUtf16[shushaUtf16Index];
				break;
			default :
				singleUnicode(0x093E);
			}
			break;
		case 0x0062: //    b    ब्   0x092C(ब)  0x094D(्)
			doubleUnicode(0x092C);
			break;
		case 0x0063: //   c   च्    0x091A(च)  0x094D(्)
			doubleUnicode(0x091A);
			break;
		case 0x0064: //     d  द   0x0926( द)
			singleUnicodeWithNextCamma(0x0926);
			break;
		case 0x0065: //    e   ए   0x090F(ए )
			singleUnicode(0x090F);
			break;
		case 0x0066: //    f   फ   0x092B(फ)
			singleUnicodeWithNextCamma(0x092B);
			break;
		case 0x0067: //    g   ग्   0x0917(ग)  0x094D(्)
			doubleUnicode(0x0917);
			break;
		case 0x0068: //    h    ह    0x0939( ह  )
			singleUnicode(0x0939);
			break;
		case 0x0069: //    i   ि  0x093F( ि  )
			//singleUnicode(0x093F);
			iFlag =  1;
			shushaUtf16Index++;
			shushaUtf16Code = shushaUtf16[shushaUtf16Index];
			break;
		case 0x006A: //   j   ज्     0x091C(ज)  0x094D(्)
			doubleUnicode(0x091C);
			break;
		case 0x006B: //   k  क    0x0915(क )
			singleUnicodeWithNextCamma(0x0915);
			break;
		case 0x006C: //   l ल्   0x0932(ल)  0x094D(्)
			doubleUnicode(0x0932);
			break;
		case 0x006D: //  m म्    0x092E(म)  0x094D(्)
			doubleUnicode(0x092E);
			break;
		case 0x006E: //   n   न्     0x0928(न)  0x094D(्)
			doubleUnicode(0x0928);
			break;
		case 0x006F: //   o  े    0x0947( े  )
			singleUnicode(0x0947);
			break;
		case 0x0070: //   p  प      0x092A(  प )
			singleUnicodeWithNextCamma(0x092A);
			break;
		case 0x0071: //   q   थ्    0x0925(थ)  0x094D(्)
			doubleUnicode(0x0925);
			break;
		case 0x0072: //    r  र     0x0930(  र  )
			singleUnicode(0x0930);
			break;
		case 0x0073: //   s  स्     0x0938(स)  0x094D(्)
			doubleUnicode(0x0938);
			break;
		case 0x0074: //   t   त   0x0924(  त  )
			singleUnicodeWithNextCamma(0x0924);
			break;
		case 0x0075: //   u   ु  0x0941(   ु  )
			singleUnicode(0x0941);
			break;
		case 0x0076: //  v   व्  0x0935(व)  0x094D(्)
			doubleUnicode(0x0935);
			break;
		case	0x0077: //   w  द्ध  0x0926(द)  0x094D(्)  0x0927(ध)
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0927);
			break;
		case	0x0078: //  x क्ष्   0x0915(क)  0x094D(्)  0x0937(ष)  0x094D(्)
			unicodeBuf[unicodeIndex++] =  0x0915;
			unicodeBuf[unicodeIndex++] =  0x094D;
			doubleUnicode(0x0937);
			break;
		case 0x0079: //   y  य्  0x092F(य)  0x094D(्)
			doubleUnicode(0x092F);
			break;
		case 0x007A: //  z  ठ      0x0920( ठ  )
			singleUnicode(0x0920);
			break;
		case 0x007B: //    {    ठ्ठ  0x0920( ठ  )  0x094D(्)   0x0920( ठ  )
			unicodeBuf[unicodeIndex++] =  0x0920;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0920);
			break;
		case 0x007C: //    |   अोम मे लगने वाला S जैसा संकेत 0x093D(   )
			singleUnicode(0x093D);
			break;
		case 0x007D: //    }   ऊ   0x090A( ऊ  )
			singleUnicode(0x090A);
			break;
		case 0x007E: //     ~  त्र  0x0924( त  )  0x094D(्)   0x0930( र  )
			unicodeBuf[unicodeIndex++] =  0x0924;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0930);
			break;
		case 0x00B2: //!  178     0xD1 dec 0209
			singleUnicode(0x0021);
			break;
		case 0x00B8: // , comma   184
			singleUnicode(0x002C);
			break;
		case 0x00BA: // 186 0xC2 0xBA   full stop . 0x002E
			singleUnicode(0x002E);
			break;
		case 0x00B9: // hyphen -
			singleUnicode(0x002D);
			break;
		case 0x00B3: //(  0xb3 dec 179
			singleUnicode(0x0028);
			break;
		case 0x00B4: //)  0xb4 dec 180
			singleUnicode(0x0029);
			break;
		case 0x00C0: //192 Slash(/) 0x002F C0
			singleUnicode(0x002F);
			break;
		case 0x00C2: //194 semicolon ; 0x003B C2
			singleUnicode(0x003B);
			break;
		case 0x00C4: //196 equal = 0x003D  C4
			singleUnicode(0x003D);
			break;
		case 0x00C6: // ? 0xC6 dec 0198
			singleUnicode(0x003F);
			break;
		case 0x00C9: // रु 191 0x0930(  र  )   0x0942(ु)
			singleUnicode(0x0930);
			unicodeBuf[unicodeIndex++] =  0x0942;
			break;
		case 0x00CB :  //0x0915(क ) 0x094D(्) 0x0930(  र  )
			unicodeBuf[unicodeIndex++] =  0x0915;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0930);
			break;
		case 0x00CC :  //0x0915(क ) 0x094D(्) 0x0930(  र  )
			unicodeBuf[unicodeIndex++] =  0x0915;
			singleUnicode(0x0943);
					break;
		case 0x00CD :  // 205 cd    0x092B(फ ) 0x094D(्) 0x0930(  र  )
			unicodeBuf[unicodeIndex++] =  0x092B;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0930);
			break;
		case	0x00D0 : //208  0xD0   ँ चन्द्र बिन्दू  0x901(ँ)
			singleUnicode(0x0901);
			break;
		case 0x00D1: //!  0xD1 dec 0209
			singleUnicode(0x0021);
			break;
		case 0x00D4: //फ़  0xD4 dec 0212
			singleUnicode(0x095E);
			break;
		default:
			singleUnicode(shushaUtf16Code); // no conversion needed for full rwo byte code
			break;
		}
	}
	unicodeBuf[unicodeIndex] = 0x0000;
	return ;
}

// end of file

