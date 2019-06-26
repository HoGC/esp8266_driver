#include "c_types.h"			// 变量类型
#include "osapi.h"  			// os_XXX
#include "driver/data.h"


uint8 ICACHE_FLASH_ATTR get_josn_num(const char *pdata, const char *typ,char *dat) {
	uint8 ret;
	//const char needle[10] = typ;
	char *PA;
	char *PB;
	PA = os_strstr(pdata, typ);
	if(PA != NULL){
		PA =PA+os_strlen(typ)+2;
		PB = os_strchr(PA, ',');
		dat = os_strncpy(dat,PA,os_strlen(PA) - os_strlen(PB));
		dat[(os_strlen(PA) - os_strlen(PB))]='\0';
		ret = 1;
	}else{
		ret = 0;
	}
	return ret;
}


/*
 // remarks : 提取json数据
 // pdata: json数据
 //	typ - 键
 //	dat - 数据
 // return value:
*/
uint8 ICACHE_FLASH_ATTR get_josn_str(const char *pdata, const char *typ,char *dat) {
	//const char needle[10] = typ;
	uint8 ret;
	char *PA;
	char *PB;
	PA = os_strstr(pdata, typ);
	if(PA != NULL){
		PA =PA+os_strlen(typ)+3;
		PB = os_strchr(PA, '"');
		dat = os_strncpy(dat,PA,os_strlen(PA) - os_strlen(PB));
		dat[(os_strlen(PA) - os_strlen(PB))]='\0';
		ret = 1;
	}else{
		ret = 0;
	}
	return ret;
}

/*
 // remarks : 将16进制数转化为字符串
 // parameter(s): [OUT] pbDest - 存放目标字符串
 //	pbSrc - 输入16进制数的起始地址
 //	nLen - 16进制数的字节数
 // text - 字符串字母大小写
 // return value:
*/
void ICACHE_FLASH_ATTR HexToStr(char *pbDest, char *pbSrc, int nLen, int text) {
	char ddl, ddh;
	int i;

	switch (text) {
	case 0:
		for (i = 0; i < nLen; i++) {
			ddh = 48 + pbSrc[i] / 16;
			ddl = 48 + pbSrc[i] % 16;
			if (ddh > 57)
				ddh = ddh + 7;
			if (ddl > 57)
				ddl = ddl + 7;
			pbDest[i * 2] = ddh;
			pbDest[i * 2 + 1] = ddl;
		}
		break;
	case 1:
		for (i = 0; i < nLen; i++) {
			ddh = 48 + pbSrc[i] / 16;
			ddl = 48 + pbSrc[i] % 16;
			if (ddh > 57)
				ddh = ddh + 7 + 32;
			if (ddl > 57)
				ddl = ddl + 7 + 32;
			pbDest[i * 2] = ddh;
			pbDest[i * 2 + 1] = ddl;
		}
		break;

	}

	pbDest[nLen * 2] = '\0';
}

/*
 // remarks : 将字符串转化为16进制数
 // parameter(s): [OUT] pbDest - 输出缓冲区
 //	[IN] pbSrc - 字符串
 //	[IN] nLen - 16进制数的字节数(字符串的长度/2)
 // return value:
 // remarks : 将字符串转化为16进制数
*/
void ICACHE_FLASH_ATTR StrToHex(char *pbDest, char *pbSrc, int nLen) {
	char h1, h2;
	char s1, s2;
	int i;

	for (i = 0; i < nLen; i++) {
		h1 = pbSrc[2 * i];
		h2 = pbSrc[2 * i + 1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;

		pbDest[i] = s1 * 16 + s2;
	}
}
/*
int ICACHE_FLASH_ATTR atoi (char s[]){
	int i,n,sign;
	for(i=0;isspace(s[i]);i++)//跳过空白符;
		sign=(s[i]=='-')?-1:1;
	if(s[i]=='+'||s[i]=='-')//跳过符号
		i++;
	for(n=0;isdigit(s[i]);i++)
		n=10*n+(s[i]-'0');//将数字字符转换成整形数字
	return sign *n;
}
*/

