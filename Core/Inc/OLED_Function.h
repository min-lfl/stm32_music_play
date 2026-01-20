/**
 * ************************************************************************
 * 
 * @file OLED_Function.h
 * @author zxr
 * @brief OLED功能函数驱动头文件
 * 
 * ************************************************************************
 * @copyright Copyright (c) 2024 zxr 
 * ************************************************************************
 */
#ifndef _OLED_FUNCTION_H_
#define _OLED_FUNCTION_H_

#include "OLED_IIC_Config.h"

//清除指定区域
void OLED_ClearArea(signed short int x, signed short int y, unsigned short int width, unsigned short int height);

//字符串显示函数
void OLED_ShowStr(signed short int x, signed short int y, unsigned char ch[], unsigned char TextSize);
//中文汉字显示函数
void OLED_ShowChinese(signed short int x, signed short int y, unsigned char* ch);
//BMP图片显示函数
void OLED_ShowBMP(signed short int x0,signed short int y0,signed short int L,signed short int H,const unsigned char BMP[]);
//显示音量条
void OLED_ShowVolumeBar(signed short int x, signed short int y, 
                      unsigned short int current, unsigned short int max);
//显示数字
void OLED_ShowNum(signed short int x, signed short int y, unsigned int number, unsigned char digit_count, unsigned char TextSize);

//画一个矩形
void OLED_DrawBox(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char thickness);

//画播放/暂停键
void OLED_PlayPauseIcon(unsigned char x, unsigned char y, uint8_t state);

//显示歌曲名字
void Show_Music_Name(unsigned char x, unsigned char y, uint8_t num);
#endif /* _OLED_FUNCTION_H_ */
