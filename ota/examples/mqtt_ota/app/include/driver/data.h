#ifndef _DATA_H_
#define _DATA_H_

uint8 ICACHE_FLASH_ATTR get_josn_str(const char *pdata, const char *typ,char *dat);
uint8 ICACHE_FLASH_ATTR get_josn_num(const char *pdata, const char *typ,char *dat);
void HexToStr(char *pbDest, char *pbSrc, int nLen, int text);
void StrToHex(char *pbDest, char *pbSrc, int nLen);


#endif /* APP_INCLUDE_MODULES_DATA_H_ */
