/*
 * ShushaKeyMapping.h
 *
 *  Created on: 14 Mar 2021
 *      Author: ratanmulchandani
 */

#ifndef SHUSHAKEYMAPPING_H_
#define SHUSHAKEYMAPPING_H_

#define EXCLAIMATION_MARK_IS_OM           0x0021 // ! = ॐ   0x0950
#define HASHTAG_IS_HALF_KHA               0x0023 // # = ख्    0x0916(ख) 0x094D(्)
#define DOLLAR_SIGN_IS_RAU                0x0024 // $ = रू    0x0930(र)  0x0942(ू)
#define PERCENT_SIGN_IS_HALF_TA           0x0025 // % = त्    0x0924(त)  0x094D(्)
#define AMPERSAND_IS_JANYA                0x0026 // & = ज्ञ    0x091C(ज)  0x094D(्)  0x091E(ञ)
#define LEFT_PARENTHESIS_IS_HAYA          0x0028 // ( = ह्य    0x0939(ह)   0x094D(्) 0x092F(य)
#define RIGHT_PARENTHESIS_IS_HAVOCALICR   0x0029 // ) = हृ     0x0939(ह)  0x094D(्)  0x0943(ृ)
#define ASTERICK_IS_HALF_HA               0x002A // * = ह्     0x0939(ह)  0x094D(्)
#define PLUS_SIGN_IS_HALF_TTATTA          0x002B // + = ट्ट     0x091F(ट)  0x094D(्)  0x091F(ट)
#define COMMA_IS_SIGN_NUKTA               0x002C // , =  0x093C(़)
#define HYPHEN_MINUS_IS_HALF_RA           0x002D // - =  र् ऊपर अाने वाला जैसे र्र    0x0930(र)  0x094D(्)
#define FULL_STOP_IS_DANDA                0x002E // . डॉट  । पूर्णविराम            0x964(।)
#define SLASH_IS_SIGN_VIRAMA_RA           0x002F //  /  =  नीचे अाया र      0x094D(्)  0x0930(र)
#define COLON_IS_SIGN_VISARGA             0x003A //   :  =   ः   0x0903
#define SEMICOLON_IS_LETTER_VOCALIC_L     0x003B //   ;  =  लृ     0x090C
#define LESSTHAN_SIGN_IS_HALF_TA          0x003C //   < =  त्      0x0924(त)  0x094D(्)
#define EQUAL_SIGN_IS_NGA                 0x003D // =  = ङ        0x0919(ङ)
#define GREATERTHAN_IS_HALF_KATA          0x003E // > = क्त        0x0915(क) 0x094D(्)   0x0924(त)
#define QUESTIONMARK_IS_LETTER_VOCALIC_R  0x003F // ?  = ऋ        0x090B(ऋ)
#define AT_SIGN_IS_HALK_KA                0x0040 // @  = क्        0x915(क) 0x094D(्)
#define LATIN_CAP_A_IS_LETTER_A           0x0041 // A  = अ        0x0905(अ)
#define LATIN_CAP_B_IS_HALF_BHA           0x0042 // B  = भ्        0x092D(भ)  0x094D(्)
#define LATIN_CAP_C_IS_LETTER_CHA         0x0043 // C  = छ        0x091B(छ)
#define LATIN_CAP_D_IS_LETTER_DDA         0x0044 // D  = ड        0x0921(ड)
#define LATIN_CAP_E_IS_HALF_SHA__HALF_RA  0x0045 // E =  श्र्       0x936(श)  0x094D(्)  0x930(र)  0x094D(्)
#define LATIN_CAP_F_IS_HALF_PHA           0x0046 // F =  फ्       0x092B(फ)  0x094D(्)
#define LATIN_CAP_G_IS_HALF_GHA           0x0047 // G =  घ्       0x0918(घ)  0x094D(्)
#define LATIN_CAP_H_IS_LETTER_NYA         0x0048 // H =  ञ       0x091E(ञ)
#define LATIN_CAP_I_IS_VOWEL_SIGN_II      0x0049 // I =  ी       0x0940( ी)
#define LATIN_CAP_J_IS_HALF_JHA           0x004A // J  = झ्     0x091D(झ)  0x094D(्)
#define LATIN_CAP_K_IS_LETTER_KHA         0x004B // K  =  ख     0x0916(ख)
#define LATIN_CAP_L_IS_LETTER_LLA         0x004C // L  =  ळ     0x0933( ळ)
#define LATIN_CAP_M_IS_SIGN_ANUSUVRA      0x004D // M =  ं     0x0902( ं)
#define LATIN_CAP_N_IS_LETTER_NNA         0x004E // N = ण्      0x0923(ण)  0x094D(्)
#define LATIN_CAP_O_IS_VOWEL_SIGN_AI      0x004F // O =  ै  0x0948(  ै)
#define LATIN_CAP_P_IS_LETTER_PA             0x0050 // P =  प्   0x092A(प)  0x094D(्)
#define LATIN_CAP_Q_IS_LETTER_DHA            0x0051 // Q = ध्     0x0927(ध)  0x094D(्)
#define LATIN_CAP_R_IS_VOWEL_SIGN_VOCALIC_R  0x0052 // R = ृ    0x0943(  ृ)
#define LATIN_CAP_S_IS_LETTER_SHA            0x0053 // S = श्    0x0936(श)  0x094D(्)
#define LATIN_CAP_T_IS_LETTER_TTA            0x0054 // T = ट     0x091F(   ट )
#define LATIN_CAP_U_IS_VOWEL_SIGN_UU         0x0055 // U = ू    0x0942( ू )
#define LATIN_CAP_V_IS_HALF_DAYA              0x0056 // V = द्य    "द् य"    0x0926(द)  0x094D(्)  0x092F(य )
#define LATIN_CAP_W_IS_HALF_DAVA              0x0057 // W = द्व       0x0926(द)  0x094D(्)  0x0935(व )
#define LATIN_CAP_X_IS_HALF_SHA              0x0058 // X = श्   0x0936(श)  0x094D(्)
#define LATIN_CAP_Y_IS_LETTER_SSA            0x0059 //  Y = ष     0x0937( ष )
#define LATIN_CAP_Z_IS_LETTER_DDHA           0x005A // Z =  ढ    0x0922( ढ  )
#define LEFT_SQUARE_BRACKET_IS_LETTER_I      0x005B // [ = इ    0x0907( इ  )
#define BACKSLASH_IS_SIGN_VIRAMA              0x005C // \ = ्    0x094D(  ्  )
#define RIGHT_SQUARE_BRACKET_IS_LETTER_U      0x005D // ] =  उ     0x0909(   उ  )
#define CIRCUMFLEX_ACCENT_IS_SIGN_CHANDRA_E   0x005E //  ^ =  ॅ    0x0945(  ॅ  )
#define LOW_LINE_IS_LETTER_DA_VIRAMA_DA       0x005F // _ =  द्द    द् द  0x0926(द)  0x094D(्)  0x0926(द )
#define GRAVE_ACCENT_IS_SIGN_VIRAMA_LETTER_RA  0x0060 //  `  =  ्र   0x094D(्)  0x0930(र )
#define LATIN_SML_A_IS_VOWEL_SIGN_AA           0x0061 // a =  ा     0x093E( ा )
#define LATIN_SML_B_IS_HALF_BA                 0x0062 // b = ब्   0x092C(ब)  0x094D(्)
#define LATIN_SML_C_IS_HALF_CA                 0x0063 // c =  च्    0x091A(च)  0x094D(्)
#define LATIN_SML_D_IS_LETTER_DA               0x0064 //  d = द   0x0926( द)
#define LATIN_SML_E_IS_LETTER_E                0x0065 //  e = ए   0x090F(ए )
#define LATIN_SML_F_IS_LETTER_PHA              0x0066 //  f = फ   0x092B(फ)
#define LATIN_SML_G_IS_HALF_GA                 0x0067 //  g = ग्   0x0917(ग)  0x094D(्)
#define LATIN_SML_H_IS_LETTER_HA               0x0068 //  h = ह    0x0939( ह  )
#define LATIN_SML_I_IS_VOWEL_SIGN_I            0x0069 //  i =  ि  0x093F( ि  )
#define LATIN_SML_J_IS_HALF_JA                 0x006A //  j =  ज्     0x091C(ज)  0x094D(्)
#define LATIN_SML_K_IS_LETTER_KA               0x006B //  k = क    0x0915(क )
#define LATIN_SML_L_IS_HALF_LA                 0x006C //  l = ल्     0x0932(ल)  0x094D(्)
#define LATIN_SML_M_IS_HALF_MA                 0x006D //  m =  म्    0x092E(म)  0x094D(्)
#define LATIN_SML_N_IS_HALF_NA                 0x006E //  n  = न्     0x0928(न)  0x094D(्)
#define LATIN_SML_O_IS_VOWEL_SIGN_E            0x006F //  o  = े    0x0947( े  )
#define LATIN_SML_P_IS_LETTER_PA               0x0070 //  p  = प      0x092A(  प )
#define LATIN_SML_Q_IS_HALF_THA                0x0071 //  q =  थ्    0x0925(थ)  0x094D(्)
#define LATIN_SML_R_IS_LETTER_RA               0x0072 //  r =  र     0x0930(  र  )
#define LATIN_SML_S_IS_HALF_SA                 0x0073 //  s  =  स्     0x0938(स)  0x094D(्)
#define LATIN_SML_T_IS_LETTER_TA               0x0074 //  t =  त   0x0924(  त  )
#define LATIN_SML_U_IS_VOWEL_SIGN_U            0x0075 //  u = ु  0x0941(   ु  )
#define LATIN_SML_V_IS_HALF_VA                 0x0076 //  v =  व्  0x0935(व)  0x094D(्)
#define LATIN_SML_W_IS_LETTER_DA_VIRAMA_DHA    0x0077 //  w = द्ध  0x0926(द)  0x094D(्)  0x0927(ध)
#define LATIN_SML_X_IS_LETTER_KA_VIRAMA_SSA    0x0078 //  x = क्ष्   0x0915(क)  0x094D(्)  0x0937(ष)  0x094D(्)
#define LATIN_SML_Y_IS_HALF_YA                 0x0079 //  y  = य्  0x092F(य)  0x094D(्)
#define LATIN_SML_Z_IS_LETTER_TTHA             0x007A // z =  ठ      0x0920( ठ  )
#define LEFTCURLYBRAKET_IS_LETTER_TTHA_VIRAMA_TTHA 0x007B // {= ठ्ठ  0x0920( ठ  )  0x094D(्)   0x0920( ठ  )
#define VERTICAL_BAR_IS_SIGN_AVAGRAHA              0x007C //   | =  अोम मे लगने वाला S जैसा संकेत 0x093D(   )
#define RIGHTCURLYBRAKET_IS_LETTER_UU              0x007D //  } =  ऊ   0x090A( ऊ  )
#define TILDE_IS_LETTER_TA_VIRAMA_RA               0x007E // ~ = त्र  0x0924( त  )  0x094D(्)   0x0930( र  )
#define SUPERSCRIPT_TWO_IS_EXCLAMATION_MARK        0x00B2 // != 0x0021 dec 178 utf8-C2B2
#define SUPERSCRIPT_THREE_IS_LEFT_PARENTHESIS      0x00B3 // (= 0x0028 dec 179 utf8-C2B3
#define ACUTE_ACCENT_IS_RIGHT_PARENTHESIS          0x00B4 // ) = 0x0029 dec 180 utf8-C2B4
#define ACUTE_ACCENT_IS_ASTERISK                   0x00B5 // * = 0x002A dec 181 utf8-C2B5
#define CADILLA_IS_COMMA                           0x00B8 // , = 0x002C comma  dec 184 utf8-C2B8
#define SUPERSCRIPT_ONE_IS_HYPHEN_MINUS            0x00B9 // hyphen - = 0x002D dec 185 utf8-C2B9
#define MASCULINE_ORDINAL_IS_FULL_STOP             0x00BA // full stop . 0x002E dec 186utf8-C2BA
#define RIGHTDOUBLEANGLE_IS_LESS_THAN_SIGN         0x00BB // < 0x003C dec 187 utf8-C2BB
#define ONEQUATERFRACTION_IS_GREATER_THAN_SIGN     0x00BC // > 0x003E dec 188 utf8-C2BC
#define LATIN_A_WITH_GRAVE_IS_SLASH                0x00C0 // Slash(/) 0x002F dec 192 0x00C0 utf8-C380
#define LATIN_A_WITH_CIRCUMFLEX_IS_SEMICOLON       0x00C2 // dec 194 semicolon ; 0x003B utf8-C382
#define LATIN_A_WITH_DIAERESIS_IS_EQUAL_SIGN       0x00C4 // dec 196 equal = 0x003D  utf8-C384
#define LATIN_CAPITAL_LETTER_AE_IS_QUESTION_MARK   0x00C6 // ? 0x003F 0xC6 dec 0198 utf8-C386
#define LETTER_E_WITH_ACUTE_IS_LETTER_RA_SIGNUU    0x00C9 // रु 191 0x0930(र) 0x0942(ु) utf8-C389
#define LETTER_E_WITH_DIAERESIS_IS_KAVIRAMARA      0x00CB //0x0915(क) 0x094D(्) 0x0930(र) utf8-C38B
#define LATIN_I_WITH_GRAVE_IS_KASIGNVOCAKICR       0x00CC //0x0915(क)  0x0943(ृ) utf8-C38C
#define LATIN_I_WITH_ACUTE_IS_PHAVIRAMARA          0x00CD // 205 cd    0x092B(फ ) 0x094D(्) 0x0930(र) utf8-C38D
#define LATIN_LETTER_ETH_IS_SIGN_CHANDRABINDU      0x00D0 //208  0xD0   ँ चन्द्र बिन्दू  0x901(ँ) utf8-C390
#define LATIN_LETTER_NTILDE_IS_EXCLAMATION_MARK    0x00D1 //!  0xD1 dec 0209 utf8-C391
#define LATIN_O_WITH_CIRCUMFLEX_IS_LETTER_FA       0x00D4 //फ़  0xD4 dec 0212 utf8-C394
#endif /* SHUSHAKEYMAPPING_H_ */


