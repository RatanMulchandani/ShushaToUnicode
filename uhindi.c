/*
 * Author : Ratan Mulchandani
 *
 * start date: 27 Jan 2021
 *
 * version 0.1
 *
 */

#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>      /* wint_t */
#include <ctype.h>

// prototypes
int shushaToUnicode();
void encodeToUtf8(wint_t codeToUse);
int notVowel(int tempCount);

// \ को फाईल \\ मे बदलना होगा जब तक regex se ठीक करूँ

//global  for easy programming
unsigned char shushaBuf[500];
wint_t unicodeBuf[1000];
unsigned char utf8Buffer[2000];
unsigned char shushaFirstByte;
int shushaIndex;
int unicodeIndex;
int utf8Index;
int iFlag; // small ि adjustment indicator
wint_t temp3ByteUnicode = 0; // proper decoder will be good idea
int halantChar = 0;

// hindi code only utf8 conversion ron 16 bit unicode
void encodeToUtf8(wint_t codeToUse)
{
	wint_t  tempCode = codeToUse & 0xFF00;
	if( tempCode != 0)
	{
		switch(codeToUse)
		{
		case 0x2014: // बाद मे समझेगे
			utf8Buffer[utf8Index++] =  0xE2;
			utf8Buffer[utf8Index++] =  0x80;
			utf8Buffer[utf8Index++] =  0x94;
			break;
		case 0x2013: // बाद मे समझेगे
			utf8Buffer[utf8Index++] =  0xE2;
			utf8Buffer[utf8Index++] =  0x80;
			utf8Buffer[utf8Index++] =  0x93;
			break;
		default:
			utf8Buffer[utf8Index++] =  0xE0;
			unsigned char temp = ((codeToUse & 0xC0) >> 6) & 0x03;
			utf8Buffer[utf8Index++] =  0xA4 + temp;
			temp = codeToUse & 0x3F;
			utf8Buffer[utf8Index++] =  0x80 + temp;
			break;
		}
	}
	else
	{
		utf8Buffer[utf8Index++] =  codeToUse & 0x007F;
	}
}
//char is not a vowel ि is not considered because it appears before consonent
int notVowel(int tempCount)
{
	char tempChar;
	tempChar = shushaBuf[shushaIndex - tempCount];
	switch(tempChar)
	{
		case 0x61:
			return 1;
		case 0x49: //   I   ी   0x0940( ी)
			return 1;
		case 0x4F: //ै की मात्रा 0x0948
			return 1;
		case 0x6F: ////   o  े    0x0947( े  )
			return 1;
		case 0x55: //   U    ू    0x0942( ू )
			return 1;
		case 0x75: //   u   ु  0x0941(   ु  )
			return 1;
		default:
			return 0;
	}
}
//
int main(int argc, char *argv[])
{
	setlocale(LC_ALL, ""); // unicode display requirement

	if( argc < 2)
	{
		printf("Missing input file\n");
		return 1;
	}

	FILE *inFp = fopen(argv[1], "r");
	if(inFp == NULL)
	{
		printf("Unable to open input file\n");
		return 1;
	}

	 int doneFlag = 0;
	 int charRead;
	 //read a word and null terminate it
	 while(doneFlag == 0)
	 {
// collect word with leading white spaces
		 shushaIndex = 0;
		 shushaBuf[shushaIndex] = 0x00;
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
			 if(isspace(charRead) == 0)
			 {
				 break;
			 }
		 }
// continue collecting word till next white space
		 int tempCount;
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

    		 tempCount = 2;
			 if( (charRead == 0x2D) && (shushaBuf[shushaIndex - 2] != '[')) // र् ऊपर जाने वाला
			 {
				 while(1)
				 {
					 if(notVowel(tempCount) == 0)
					 {
						 for(int i = 2; i <= tempCount; i++)
						 {
							 shushaBuf[shushaIndex - (i - 1)] = shushaBuf[shushaIndex - i];
						 }
						 shushaBuf[shushaIndex - tempCount] = 0x2D;
						 break;
					 }
					 else
					 {
						 tempCount++;
					 }
				 }

			 }
		 }

	 	 // this fuction converts shusha to unicode 16 bit
		 shushaToUnicode();


		int outIndex;
		// unicode to utf8 convertion only hindi
		outIndex = 0;
		while(unicodeBuf[outIndex] != 0x0000)
		{
			encodeToUtf8((wint_t)unicodeBuf[outIndex]);
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
	halantChar = 0;
	shushaIndex++;
	shushaFirstByte = shushaBuf[shushaIndex];
	unicodeBuf[unicodeIndex++] = codeToUse;
	if(iFlag)
	{
		unicodeBuf[unicodeIndex++] = 0x093F;
		iFlag = 0;
	}
}
//
void doubleUnicode(wint_t codeToUse)
{
	halantChar = 0;
	unicodeBuf[unicodeIndex++] = codeToUse;
	shushaIndex++;
	shushaFirstByte = shushaBuf[shushaIndex];
	if(shushaFirstByte == 0x61) // a ा
	{
		shushaIndex++;
		shushaFirstByte = shushaBuf[shushaIndex];
		if(shushaFirstByte == 0x2C) // ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
		{
			unicodeBuf[unicodeIndex++] = 0x093C;
			shushaIndex++;
			shushaFirstByte = shushaBuf[shushaIndex];
		}
	}
	else
	{
		unicodeBuf[unicodeIndex++] = 0x094D;
		halantChar = 1;
	}

	if( (iFlag) && (unicodeBuf[unicodeIndex - 1] != 0x094D))
	{
		unicodeBuf[unicodeIndex++] = 0x093F;
		iFlag = 0;
	}
}
//
int checkswapForM()
{
	shushaIndex++;
	shushaFirstByte = shushaBuf[shushaIndex];
	char oCurrection = shushaBuf[shushaIndex + 1];
	switch(shushaFirstByte)
	{
		case 0x61:
			switch (oCurrection)
			{
			case 'o' :  // 6F treat it as ो
				unicodeBuf[unicodeIndex++] =  0x094B;
				shushaIndex++;
				break;
			case 'O' :  // 6F treat it as ो
				unicodeBuf[unicodeIndex++] =  0x094C;
				shushaIndex++;
				break;
			default :
				unicodeBuf[unicodeIndex++] = 0x093E;
			}
			return 0;
		case 0x49: //   I   ी   0x0940( ी)
			unicodeBuf[unicodeIndex++] = 0x0940;
			return 0;
		case 0x4F: //ै की मात्रा 0x0948
			unicodeBuf[unicodeIndex++] = 0x0948;
			return 0;
		case 0x6F: ////   o  े    0x0947( े  )
			unicodeBuf[unicodeIndex++] = 0x0947;
			return 0;
		case 0x55: //   U    ू    0x0942( ू )
			unicodeBuf[unicodeIndex++] = 0x0942;
			return 0;
		case 0x75: //   u   ु  0x0941(   ु  )
			unicodeBuf[unicodeIndex++] = 0x0941;
			return 0;
		case 0x52: //    R     ृ    0x0943(  ृ)
			unicodeBuf[unicodeIndex++] = 0x0943;
			return 0;
		default:
			//printf(" No voval 0x%02x\n", shushaFirstByte);
			shushaIndex--;
			return 0;
	}
}
//
void singleUnicodeWithNextCamma(wint_t codeToUse)
{
	halantChar = 0;
		shushaIndex++;
		shushaFirstByte = shushaBuf[shushaIndex];
		unicodeBuf[unicodeIndex++] = codeToUse;
		if(iFlag)
		{
			if(shushaFirstByte == 0x2C) // ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
			{
				unicodeBuf[unicodeIndex++] = 0x093C;
				shushaIndex++;
				shushaFirstByte = shushaBuf[shushaIndex];
			}
			if(shushaFirstByte == 0x60) //     `       ्र   0x094D(्)  0x0930(र )
			{
				unicodeBuf[unicodeIndex++] =  0x094D;
				unicodeBuf[unicodeIndex++] =  0x0930;
				shushaIndex++;
				shushaFirstByte = shushaBuf[shushaIndex];
			}
			unicodeBuf[unicodeIndex++] = 0x093F;
			iFlag = 0;
		}

}
// मै 0x0d को end of line की तरह काम मे लाऊँगा। लाईन पढ्ने के बाद यहाँ जो लाईन अाएगी  उसका अंत ठीठठाक होगा
int shushaToUnicode( )
{
	shushaIndex = 0;
	unicodeIndex = 0;
	utf8Index = 0;
	char oCurrection;
	halantChar = 0;

	shushaFirstByte = shushaBuf[shushaIndex];
	//shushaFirstByte != 0x0D) && (shushaFirstByte != 0x0A ) &&
	while (shushaFirstByte != 0x00 )
	{

		switch(shushaFirstByte)
		{
		case 0x21:    // !   ॐ   0x0950
			singleUnicode(0x0950);
			break;
		case 0x23: //   #   ख्                 0x0916(ख) 0x094D(्)
			doubleUnicode( 0x0916);
			break;
		case 0x24: //  $   रू                    0x0930(र)  0x0942(ू)   रू
			unicodeBuf[unicodeIndex++] = 0x0930;
			singleUnicode(0x0942);
			break;
		case 0x25 : //  %  त्                     0x0924(त)    0x094D(्)
			doubleUnicode(0x0924);
			break;
		case 0x26: //  &  ज्ञ                     0x091C(ज)  0x094D(्)   0x091E(ञ)  ज्ञ
			unicodeBuf[unicodeIndex++] = 0x091C;
			unicodeBuf[unicodeIndex++] = 0x094D;
			singleUnicode(0x091E);
			break;
		case 0x28: //  (  ह्य   "ह्  य"         0x0939(ह)  0x094D(्) 0x092F(य)
			doubleUnicode(0x0939);
			unicodeBuf[unicodeIndex++] =  0x092F;
			break;
		case 0x29: //  )   हृ " ह ृ"           0x0939(ह)  0x0943(ृ)
			singleUnicode(0x0939);
			unicodeBuf[unicodeIndex++] =  0x0943;
			break;
		case 0x2A: //  *  ह्                     0x0939(ह)  0x094D(्)
			doubleUnicode(0x0939);
			break;
		case 0x2B: //   +  ट्ट                   0x091F(ट)  0x094D(्)  0x091F(ट)
			singleUnicode(0x091F);
			unicodeBuf[unicodeIndex++] =  0x094D;
			unicodeBuf[unicodeIndex++] =  0x091F;
			break;
		case 0x2C: //    ,  काॅमा  नीचे वाला बिंदू मान कर चल रहा हूँ। ox093C(़)
			if(halantChar == 1)
			{
				shushaIndex++;
				shushaFirstByte = shushaBuf[shushaIndex];
				if(shushaFirstByte == 0x61) // a ा
				{
					shushaIndex++;
					shushaFirstByte = shushaBuf[shushaIndex];
					unicodeIndex--;
					unicodeBuf[unicodeIndex++] = 0x093C;
				}
				else
				{
					shushaIndex--;
					singleUnicode(0x093C);
				}
			}
			else
			{
				//printf("\n(0)0x%02x 0x%02x\n", shushaIndex , shushaBuf[shushaIndex]);
				if( (shushaIndex == 00) || (shushaBuf[shushaIndex - 1] == 0x2C) || (shushaBuf[shushaIndex - 1] == 0x20))
				{
					singleUnicode(0x002E);
					//printf("\n(1)0x%02x 0x%02x\n", shushaIndex , shushaBuf[shushaIndex]);
				}
				else
				{
					singleUnicode(0x093C);
					//printf("\n(2)0x%02x 0x%02x\n", shushaIndex , shushaBuf[shushaIndex]);
				}
			}
			halantChar = 0;
			break;
		case 0x2D: //    -  र् ऊपर अाने वाला जैसे र्र    0x0930(र)  0x094D(्)
			halantChar = 0;
			shushaIndex++;
			shushaFirstByte = shushaBuf[shushaIndex];
			unicodeBuf[unicodeIndex++] =  0x0930;
			unicodeBuf[unicodeIndex++] =  0x094D;
			break;
		case 0x2E: //     . डॉट  । पूर्णविराम            0x964(।)
			singleUnicode(0x0964);
			break;
		case 0x2F: //      /   ( ट्र  ट)   नीचे अाया र     0x091F(ट)  0x094D(्)  0x0930(र)
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0930);
			break;
		case 0x3A: //     :    ः   0x0903
			if(shushaIndex == 00)
			{
				singleUnicode(0x003A);
			}
			else
			{
			singleUnicode(0x0903);
			}
			break;
		case 0x3B: //      ;    लृ नहीं पता सही है?  0x090C
			singleUnicode(0x090C);
			break;
		case 0x3C: //     <   त्       0x0924(त)  0x094D(्)
			doubleUnicode(0x0924);
			break;
		case 0x3D: //     =   ङ           0x0919(ङ)
			singleUnicode(0x0919);
			break;
		case 0x3E: //     >  क्त          0x915(क) 0x094D(्)   0x0924(त)
			unicodeBuf[unicodeIndex++] = 0x0915;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0924);
			break;
		case 0x3F: //    ?   ऋ          0x90B(ऋ)
			singleUnicode(0x090B);
			break;
		case 0x40: // @  क्            0x915(क) 0x094D(्)
			doubleUnicode(0x0915);
			break;
		case 0x41: //  A  अ           0x905(अ)
			singleUnicode(0x0905);
			break;
		case 0x42: //  B   भ्          0x092D(भ)  0x094D(्)
			doubleUnicode(0x092D);
			break;
		case 0x43: //   C  छ         0x091B(छ)
			singleUnicodeWithNextCamma(0x091B);
			break;
		case 0x44: //   D  ड         0x0921(ड)
			singleUnicodeWithNextCamma(0x0921);
			break;
		case 0x45: //    E   श्र्      0x936(श)  0x094D(्)  0x930(र)  0x094D(्)
			unicodeBuf[unicodeIndex++] =  0x0936;
			unicodeBuf[unicodeIndex++] =  0x094D;
			doubleUnicode(0x0930);
			break;
		case 0x46: //    F    फ्     0x092B(फ)  0x094D(्)
			doubleUnicode(0x092B);
			break;
		case 0x47: //    G   घ्    0x0918(घ)  0x094D(्)
			doubleUnicode(0x0918);
			break;
		case 0x48: //    H  ञ    0x091E(ञ)
			singleUnicode(0x091E);
			break;
		case 0x49: //   I   ी   0x0940( ी)
			singleUnicode(0x0940);
			break;
		case 0x4A: //   J    झ्     0x091D(झ)  0x094D(्)
			doubleUnicode(0x091D);
			break;
		case 0x4B: //  K   ख     0x0916(ख)
			singleUnicodeWithNextCamma(0x0916);
			break;
		case 0x4C:  // L   ळ     0x0933( ळ)
			singleUnicode(0x0933);
			break;
		case 0x4D: //    M ं     0x0902( ं)
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
		case 0x4E: //   N  ण्   0x0923(ण)  0x094D(्)
			doubleUnicode(0x0923);
			break;
		case 0x4F: //    O   ै  0x0948(  ै)
			singleUnicode(0x0948);
			break;
		case 0x50: //   P    प्   0x092A(प)  0x094D(्)
			doubleUnicode(0x092A);
			break;
		case 0x51: //    Q  ध्     0x0927(ध)  0x094D(्)
			doubleUnicode(0x0927);
			break;
		case 0x52: //    R     ृ    0x0943(  ृ)
			singleUnicode(0x0943);
			break;
		case 0x53: //    S    श्    0x0936(श)  0x094D(्)
			doubleUnicode(0x0936);
			break;
		case 0x54: //   T     ट     0x091F(   ट )
			singleUnicodeWithNextCamma(0x091F);
			break;
		case 0x55: //   U    ू    0x0942( ू )
			singleUnicode(0x0942);
			break;
		case 0x56: //    V   द्य    "द् य"    0x0926(द)  0x094D(्)  0x092F(य )
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x092F);
			break;
		case 0x57: //    W     द्व       0x0926(द)  0x094D(्)  0x0935(व )
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0935);
			break;
		case 0x58: //    X श्   0x0936(श)  0x094D(्) पक्का नहीं है जो कर रहे है सही है या नहीं क्योंकि S भी यही कर रहा है।
			doubleUnicode(0x0936);
			break;
		case 0x59: //   Y  ष्      0x0937( ष् )
			doubleUnicode(0x0937);
			break;
		case 0x5A: //   Z   ढ    0x0922( ढ  )
			singleUnicodeWithNextCamma(0x0922);
			break;
		case 0x5B: //  [      इ   0x0907( इ  )  [- ई
			if(shushaBuf[shushaIndex + 1] == 0x2D)
			{
				shushaIndex++;
				singleUnicode(0x0908);
			}
			else
			{
				singleUnicode(0x0907);
			}
			break;
		case 0x5C: //  \      ्   0x094D(  ्  )
			singleUnicode(0x094D);
			break;
		case 0x5D: //  ]   उ  0x0909(   उ  )
			singleUnicode(0x0909);
			break;
		case 0x5E: //  ^   ॅ    0x0945(  ॅ  )
			singleUnicode(0x0945);
			break;
		case 0x5F: //  _   द्द    द् द  0x0926(द)  0x094D(्)  0x0926(द )
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0926);
			break;
		//case 0x60     `     नही पकड़ मे अाया
		case 0x60: //     `       ्र   0x094D(्)  0x0930(र )
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0930);
			break;
		case 0x61: //    a   ा     0x093E( ा )
			oCurrection = shushaBuf[shushaIndex + 1];
			switch (oCurrection)
			{
			case 'o' :  // 6F treat it as ो
				unicodeBuf[unicodeIndex++] =  0x094B;
				shushaIndex++;
				shushaIndex++;
				shushaFirstByte = shushaBuf[shushaIndex];
				break;
			case 'O' :  // 6F treat it as ो
				unicodeBuf[unicodeIndex++] =  0x094C;
				shushaIndex++;
				shushaIndex++;
				shushaFirstByte = shushaBuf[shushaIndex];
				break;
			default :
				singleUnicode(0x093E);
			}
			break;
		case 0x62: //    b    ब्   0x092C(ब)  0x094D(्)
			doubleUnicode(0x092C);
			break;
		case 0x63: //   c   च्    0x091A(च)  0x094D(्)
			doubleUnicode(0x091A);
			break;
		case 0x64: //     d  द   0x0926( द)
			singleUnicodeWithNextCamma(0x0926);
			break;
		case 0x65: //    e   ए   0x090F(ए )
			singleUnicode(0x090F);
			break;
		case 0x66: //    f   फ   0x092B(फ)
			singleUnicodeWithNextCamma(0x092B);
			break;
		case 0x67: //    g   ग्   0x0917(ग)  0x094D(्)
			doubleUnicode(0x0917);
			break;
		case 0x68: //    h    ह    0x0939( ह  )
			singleUnicode(0x0939);
			break;
		case 0x69: //    i   ि  0x093F( ि  )
			//singleUnicode(0x093F);
			iFlag =  1;
			shushaIndex++;
			shushaFirstByte = shushaBuf[shushaIndex];
			break;
		case 0x6A: //   j   ज्     0x091C(ज)  0x094D(्)
			doubleUnicode(0x091C);
			break;
		case 0x6B: //   k  क    0x0915(क )
			singleUnicodeWithNextCamma(0x0915);
			break;
		case 0x6C: //   l ल्   0x0932(ल)  0x094D(्)
			doubleUnicode(0x0932);
			break;
		case 0x6D: //  m म्    0x092E(म)  0x094D(्)
			doubleUnicode(0x092E);
			break;
		case 0x6E: //   n   न्     0x0928(न)  0x094D(्)
			doubleUnicode(0x0928);
			break;
		case 0x6F: //   o  े    0x0947( े  )
			singleUnicode(0x0947);
			break;
		case 0x70: //   p  प      0x092A(  प )
			singleUnicodeWithNextCamma(0x092A);
			break;
		case 0x71: //   q   थ्    0x0925(थ)  0x094D(्)
			doubleUnicode(0x0925);
			break;
		case 0x72: //    r  र     0x0930(  र  )
			singleUnicode(0x0930);
			break;
		case 0x73: //   s  स्     0x0938(स)  0x094D(्)
			doubleUnicode(0x0938);
			break;
		case 0x74: //   t   त   0x0924(  त  )
			singleUnicodeWithNextCamma(0x0924);
			break;
		case 0x75: //   u   ु  0x0941(   ु  )
			singleUnicode(0x0941);
			break;
		case 0x76: //  v   व्  0x0935(व)  0x094D(्)
			doubleUnicode(0x0935);
			break;
		case	0x77: //   w  द्ध  0x0926(द)  0x094D(्)  0x0927(ध)
			unicodeBuf[unicodeIndex++] =  0x0926;
			unicodeBuf[unicodeIndex++] =  0x094D;

			singleUnicode(0x0927);
			break;
		case	0x78: //  x क्ष्   0x0915(क)  0x094D(्)  0x0937(ष)  0x094D(्)
			unicodeBuf[unicodeIndex++] =  0x0915;
			unicodeBuf[unicodeIndex++] =  0x094D;
			doubleUnicode(0x0937);
			break;
		case 0x79: //   y  य्  0x092F(य)  0x094D(्)
			doubleUnicode(0x092F);
			break;
		case 0x7A: //  z  ठ      0x0920( ठ  )
			singleUnicode(0x0920);
			break;
		case 0x7B: //    {    ठ्ठ  0x0920( ठ  )  0x094D(्)   0x0920( ठ  )
			unicodeBuf[unicodeIndex++] =  0x0920;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0920);
			break;
		case 0x7C: //    |   अोम मे लगने वाला S जैसा संकेत 0x093D(   )
			singleUnicode(0x093D);
			break;
		case 0x7D: //    }   ऊ   0x090A( ऊ  )
			singleUnicode(0x090A);
			break;
		case 0x7E: //     ~  त्र  0x0924( त  )  0x094D(्)   0x0930( र  )
			unicodeBuf[unicodeIndex++] =  0x0924;
			unicodeBuf[unicodeIndex++] =  0x094D;
			singleUnicode(0x0930);
			break;
		case 0xC2: // unicode  है दो बाईट का  8x को हैंडल करने कों
			shushaIndex++;
			shushaFirstByte = shushaBuf[shushaIndex];
			switch (shushaFirstByte)
			{
			case 0xB8: // , comma   184
				singleUnicode(0x002C);
				break;
			case 0xBA: // 186 0xC2 0xBA   full stop . 0x002E
				singleUnicode(0x002E);
				break;
			case 0xB9: // hyphen -
				singleUnicode(0x002D);
				break;
			case 0xb3: //(  0xb3 dec 179
				singleUnicode(0x0028);
				break;
			case 0xb4: //)  0xb4 dec 180
				singleUnicode(0x0029);
				break;
			default:
				break;
			}
			break;

		case 0xC3: // unicode  है दो बाईट का  8x को हैंडल करने को
				shushaIndex++;
				shushaFirstByte = shushaBuf[shushaIndex];
				switch (shushaFirstByte)
				{
					case 0x82: //194 semicolon ; 0x003B C2
						singleUnicode(0x003B);
						break;
					case 0x84: //196 equal = 0x003D  C4
						singleUnicode(0x003D);
						break;
					case 0x86: // ? 0xC6 dec 0198
						singleUnicode(0x003F);
						break;
					case 0x89: // रु 191 0x0930(  र  )   0x0942(ु)
						singleUnicode(0x0930);
						unicodeBuf[unicodeIndex++] =  0x0942;
						break;
					case 0x8B :  //0x0915(क ) 0x094D(्) 0x0930(  र  )
						unicodeBuf[unicodeIndex++] =  0x0915;
						unicodeBuf[unicodeIndex++] =  0x094D;
						singleUnicode(0x0930);
						break;
					case 0x8D :  // 205 cd    0x092B(फ ) 0x094D(्) 0x0930(  र  )
						unicodeBuf[unicodeIndex++] =  0x092B;
						unicodeBuf[unicodeIndex++] =  0x094D;
						singleUnicode(0x0930);
						break;
					case	0x90 : //208  0xD0   ँ चन्द्र बिन्दू  0x901(ँ)
						singleUnicode(0x0901);
						break;
					case 0x91: //!  0xD1 dec 0209
						singleUnicode(0x0021);
						break;
					case 0x94: //फ़  0xD४ dec 02१२
						singleUnicode(0x095E);
						break;
					default:
						break;
				}
		break;

		case 0xE2: // unicode  है तीन बाईट का ठीक से डिकोड करना है
			shushaIndex++;
			shushaFirstByte = shushaBuf[shushaIndex];
			temp3ByteUnicode = shushaFirstByte;
			temp3ByteUnicode  <<= 8;
			shushaIndex++;
			shushaFirstByte = shushaBuf[shushaIndex];
			temp3ByteUnicode += shushaFirstByte;
			switch(temp3ByteUnicode)
				{
				case 0x8094:
					unicodeBuf[unicodeIndex++] =  0x2014;
					break;
				case 0x8093:
					unicodeBuf[unicodeIndex++] =  0x2013;
					break;
				default:
					break;
				}
			shushaIndex++;
			shushaFirstByte = shushaBuf[shushaIndex];
			break;
		default:
			singleUnicode((wint_t)shushaFirstByte);
			break;

		}
	}
	unicodeBuf[unicodeIndex] = 0x0000;
	return 0;
}
// end of file
