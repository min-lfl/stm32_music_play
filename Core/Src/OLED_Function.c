#include "OLED_Function.h"
#include "OLED_Front.h"


/**
 * ************************************************************************
 * @brief 清除指定矩形区域（高效实现）
 * 
 * @param[in] x       起始点横坐标（0～127）
 * @param[in] y       起始点纵坐标（0～63）
 * @param[in] width   清除区域宽度（像素）
 * @param[in] height  清除区域高度（像素）
 * 
 * @note 1. 自动处理超出屏幕边界的情况
 *       2. 仅执行有效区域的清除操作
 *       3. 仅刷新一次屏幕（效率优化）
 * ************************************************************************
 */
void OLED_ClearArea(signed short int x, signed short int y, unsigned short int width, unsigned short int height)
{
    // 1. 检查无效参数（宽度/高度为0直接返回）
    if (width == 0 || height == 0) return;
    
    // 2. 计算有效区域（自动裁剪到屏幕范围内）
    signed short int start_x = (x < 0) ? 0 : (x >= SCREEN_COLUMN) ? SCREEN_COLUMN - 1 : x;
    signed short int start_y = (y < 0) ? 0 : (y >= SCREEN_ROW) ? SCREEN_ROW - 1 : y;
    
    signed short int end_x = (x + width - 1) < 0 ? -1 : (x + width - 1) >= SCREEN_COLUMN ? SCREEN_COLUMN - 1 : x + width - 1;
    signed short int end_y = (y + height - 1) < 0 ? -1 : (y + height - 1) >= SCREEN_ROW ? SCREEN_ROW - 1 : y + height - 1;
    
    // 3. 检查区域是否有效（避免无效循环）
    if (start_x > end_x || start_y > end_y) return;
    
    // 4. 高效清除区域（避免在循环内刷新）
    for (signed short int i = start_y; i <= end_y; i++) {
        for (signed short int j = start_x; j <= end_x; j++) {
            OLED_SetPixel(j, i, 0); // 设置为0（清除像素）
        }
    }
    
    // 5. 单次刷新（效率关键优化）
    OLED_RefreshRAM();
}


/**
 * ************************************************************************
 * @brief 字符串显示函数
 * 
 * @param[in] x  	起始点横坐标（0~127）
 * @param[in] y  	起始点纵坐标（0~63）
 * @param[in] ch  	字符串（通过双引号引入）
 * @param[in] TextSize  字符大小（1:6*8 ；2:8*16）
 * 
 * ************************************************************************
 */
void OLED_ShowStr(signed short int x, signed short int y, unsigned char ch[], unsigned char TextSize)
{ 
	if (x >= 0 && x < SCREEN_COLUMN && y >= 0 && y < SCREEN_ROW) 
	{
		int32_t c = 0;
		unsigned char j = 0;
	
		switch(TextSize)
		{
			case 1:
			{
				while(ch[j] != '\0')
				{
					c = ch[j] - 32;
					if(c < 0)	//无效字符
						break;
					
					if(x >= 125 || (127-x < 6))//一行最大显示字符数：21字节显示,多出两列，不显示 || 剩余列小于6不能显示完整字符，换行显示
					{
						x = 0;
						y += 8;//换行显示
						if(63 - y < 8)	// 不足以显示一行时不显示
							break;
					}
					for(unsigned char m = 0; m < 6; m++)
					{
						for(unsigned char n = 0; n < 8; n++)
						{
							OLED_SetPixel(x+m, y+n, (F6x8[c][m] >> n) & 0x01);
						}
					}
					x += 6;
					j++;
				}
			}break;
			case 2:
			{
				while(ch[j] != '\0')
				{
					c = ch[j] - 32;
					if(c < 0)	//无效字符
						break;
					
					if(x >= 127 || (127-x < 8))//16字节显示 || 剩余列小于8不能显示完整字符，换行显示
					{
						x = 0;
						y += 16;//换行显示
						if(63 - y < 16)	// 不足以显示一行时不显示
							break;
					}
					for(unsigned char m = 0; m < 2; m++)
					{
						for(unsigned char n = 0; n < 8; n++)
						{
							for(unsigned char i = 0; i < 8; i++)
							{
								OLED_SetPixel(x+n, y+i+m*8, (F8X16[c][n+m*8] >> i) & 0x01);
							}
						}	
					}
					x += 8;
					j++;
				}
			}break;
		}
	}
	OLED_RefreshRAM();
}


/**
 * ************************************************************************
 * @brief 优化后的中文汉字显示函数（高效版）
 * 
 * @param[in] x  	起始点横坐标（0～127）
 * @param[in] y  	起始点纵坐标（0～63）
 * @param[in] ch  	汉字字模库索引（GB2312编码字符串）
 * 
 * @note  1. 仅刷新一次屏幕（避免频繁刷新）
 *        2. 自动跳过无效字符（避免死循环）
 *        3. 空格显示优化（直接使用预定义空格字模）
 * ************************************************************************
 */
void OLED_ShowChinese(signed short int x, signed short int y, unsigned char* ch)
{
    if (x < 0 || x >= SCREEN_COLUMN || y < 0 || y >= SCREEN_ROW) 
        return;

    // 预计算常量（避免循环内重复计算）
    const int32_t CHAR_WIDTH = 16;       // 汉字宽度
    const int32_t CHAR_HEIGHT = 16;      // 汉字高度
    const int32_t ROW_HEIGHT = 8;        // 每页高度
    const int32_t PAGE_COUNT = 2;        // 汉字占2页
    const int32_t CHAR_OFFSET = 2;       // GB2312编码占用2字节

    int32_t len = 0;
    while (ch[len] != '\0') {
        // 检查是否需要换行（剩余宽度不足16列）
        if (x >= SCREEN_COLUMN - CHAR_WIDTH || (SCREEN_COLUMN - x) < CHAR_WIDTH) {
            x = 0;
            y += CHAR_HEIGHT;
            if (y >= SCREEN_ROW - CHAR_HEIGHT) break; // 超出屏幕底部
        }

        // 处理空格（0x20）
        if (ch[len] == 0x20) {
            // 直接显示预定义空格（避免遍历字库）
            for (int m = 0; m < PAGE_COUNT; m++) {
                for (int n = 0; n < CHAR_WIDTH; n++) {
                    for (int j = 0; j < ROW_HEIGHT; j++) {
                        // 空格字模使用0x00（全黑），实际显示为透明
                        OLED_SetPixel(x + n, y + j + m * ROW_HEIGHT, 0);
                    }
                }
            }
            x += CHAR_WIDTH;
            len++; // 空格只占1字节
            continue;
        }

        // 尝试匹配汉字（GB2312编码）
        uint8_t char_found = 0;
        for (int i = 0; i < sizeof(F16x16_CN)/sizeof(GB2312_CN); i++) {
            // 检查当前字符是否为有效汉字（需2字节）
            if (ch[len+1] != '\0' && 
                F16x16_CN[i].index[0] == ch[len] && 
                F16x16_CN[i].index[1] == ch[len+1]) {
                
                // 显示汉字（16x16像素）
                for (int m = 0; m < PAGE_COUNT; m++) {
                    for (int n = 0; n < CHAR_WIDTH; n++) {
                        for (int j = 0; j < ROW_HEIGHT; j++) {
                            uint8_t pixel = (F16x16_CN[i].encoder[n + m * CHAR_WIDTH] >> j) & 0x01;
                            OLED_SetPixel(x + n, y + j + m * ROW_HEIGHT, pixel);
                        }
                    }
                }
                x += CHAR_WIDTH;
                len += CHAR_OFFSET; // 跳过2字节
                char_found = 1;
                break;
            }
        }

        // 无效字符处理：跳过当前字符（汉字2字节/空格1字节）
        if (!char_found) {
            len += (ch[len] == 0x20) ? 1 : CHAR_OFFSET;
        }
    }

    // 仅刷新一次屏幕（关键优化点）
    OLED_RefreshRAM();
}


/**
 * ************************************************************************
 * @brief BMP图片显示函数
 * 
 * @param[in] x0  	起始点横坐标（0~127）
 * @param[in] y0  	起始点纵坐标（0~63）
 * @param[in] L  	BMP图片宽度
 * @param[in] H  	BMP图片高度
 * @param[in] BMP  	图片取模地址
 * 
 * @example OLED_ShowBMP(0,0,52,48,(unsigned char *)astronaut_0);
 * ************************************************************************
 */
void OLED_ShowBMP(signed short int x0,signed short int y0,signed short int L,signed short int H,const unsigned char BMP[])
{
	if (x0 >= 0 && x0 < SCREEN_COLUMN && x0+L <= SCREEN_ROW &&\
		y0 >= 0 && y0 < SCREEN_COLUMN && y0+H <= SCREEN_ROW) {
		
		unsigned char *p = (unsigned char *)BMP;
		for(signed short int y = y0; y < y0+H; y+=8)
		{
			for(signed short int x = x0; x < x0+L; x++)
			{
				for(signed short int i = 0; i < 8; i++)
				{
					OLED_SetPixel(x, y+i, ((*p) >> i) & 0x01);
				}
				p++;
			}
		}
	}

	OLED_RefreshRAM();
}



/**
 * ************************************************************************
 * @brief 显示带清晰边框的音量进度条
 * 
 * @param[in] x       进度条左上角横坐标（0～127）
 * @param[in] y       进度条左上角纵坐标（0～63）
 * @param[in] current 当前音量值（0～max_volume）
 * @param[in] max     最大音量值（必须>0）
 * 
 * @note 1. 背景区域（未填充）显示白色背景+黑色边框
 *       2. 填充区域（已填充）显示黑色实心
 *       3. 进度条宽度60像素，高度10像素
 *       4. 严格边界检查，避免越界
 *       5. 与OLED_ShowChinese保持相同坐标处理风格
 * ************************************************************************
 */
void OLED_ShowVolumeBar(signed short int x, signed short int y, 
                      unsigned short int current, unsigned short int max)
{
    // 边界检查：确保坐标在屏幕范围内
    if (x >= 0 && x < SCREEN_COLUMN && y >= 0 && y < SCREEN_ROW) {
        // 安全处理：max必须大于0
        if (max == 0) {
            max = 1; // 避免除零错误
        }
        
        // 计算进度百分比（整数计算避免浮点）
        unsigned short int percentage = (current * 100) / max;
        if (percentage > 100) percentage = 100; // 防止越界
        
        // 进度条总宽度60px，计算填充宽度
        unsigned short int filled_width = (percentage * 60) / 100;
        
        // 计算实际绘制宽度（避免越界）
        unsigned short int draw_width = filled_width;
        if (x + filled_width > SCREEN_COLUMN) {
            draw_width = SCREEN_COLUMN - x;
        }
        
        // 1. 清空整个进度条区域（60x10）为白色背景
        for (unsigned short int i = 0; i < 10; i++) {
            for (unsigned short int j = 0; j < 60; j++) {
                if (x + j < SCREEN_COLUMN && y + i < SCREEN_ROW) {
                    OLED_SetPixel(x + j, y + i, 0); // 白色背景 (0)
                }
            }
        }
        
        // 2. 画进度条外框（黑色边框，1） - 关键修正：用1（黑色）代替0（白色）
        // 顶部边框
        for (unsigned short int j = 0; j < 60; j++) {
            if (x + j < SCREEN_COLUMN && y < SCREEN_ROW) {
                OLED_SetPixel(x + j, y, 1); // 黑色边框 (1)
            }
        }
        // 底部边框
        for (unsigned short int j = 0; j < 60; j++) {
            if (x + j < SCREEN_COLUMN && y + 9 < SCREEN_ROW) { // 注意：高度10，最后一行是y+9
                OLED_SetPixel(x + j, y + 9, 1); // 黑色边框 (1)
            }
        }
        // 左侧边框
        for (unsigned short int i = 0; i < 10; i++) {
            if (x < SCREEN_COLUMN && y + i < SCREEN_ROW) {
                OLED_SetPixel(x, y + i, 1); // 黑色边框 (1)
            }
        }
        // 右侧边框（计算实际边界）
        unsigned short int right_x = x + 59; // 60像素宽，右侧坐标为x+59
        if (right_x >= SCREEN_COLUMN) {
            right_x = SCREEN_COLUMN - 1;
        }
        for (unsigned short int i = 0; i < 10; i++) {
            if (right_x < SCREEN_COLUMN && y + i < SCREEN_ROW) {
                OLED_SetPixel(right_x, y + i, 1); // 黑色边框 (1)
            }
        }
        
        // 3. 绘制填充区域（黑色实心，1）
        for (unsigned short int i = 0; i < 10; i++) {
            for (unsigned short int j = 0; j < draw_width; j++) {
                if (x + j < SCREEN_COLUMN && y + i < SCREEN_ROW) {
                    OLED_SetPixel(x + j, y + i, 1); // 黑色填充 (1)
                }
            }
        }
    }
    OLED_RefreshRAM();
}


/**
 * ************************************************************************
 * @brief 数字显示函数（左对齐，不足位数右边补空格）
 * 
 * @param[in] x           起始点横坐标（0～127）
 * @param[in] y           起始点纵坐标（0～63）
 * @param[in] number      要显示的数字（非负整数）
 * @param[in] digit_count 数字的位数（用于占位，左对齐，不足时右边补空格）
 * @param[in] TextSize    字符大小（1:6*8 ；2:8*16）
 * 
 * @note 1. digit_count 必须 >= 1
 *       2. 若数字位数 > digit_count，则显示最后 digit_count 位（左对齐截断）
 *       3. 负数会按正数处理（无符号整数显示）
 * ************************************************************************
 */
void OLED_ShowNum(signed short int x, signed short int y, unsigned int number, unsigned char digit_count, unsigned char TextSize)
{
    // 确保 digit_count 至少为 1
    if (digit_count < 1) {
        digit_count = 1;
    }
    
    // 限制 digit_count 最大为 20（避免缓冲区溢出）
    if (digit_count > 20) {
        digit_count = 20;
    }

    // 用于存储数字字符串的缓冲区（最大 20 位数字 + 结束符）
    unsigned char num_str[21] = {0};
    int num_digits = 0;
    
    // 处理数字 0 的特殊情况
    if (number == 0) {
        num_str[0] = '0';
        num_digits = 1;
    } else {
        // 将数字转换为字符串（低位在前，高位在后）
        unsigned int temp = number;
        while (temp) {
            num_str[num_digits++] = '0' + (temp % 10);
            temp /= 10;
        }
        
        // 反转字符串（使高位在前）
        for (int i = 0; i < num_digits / 2; i++) {
            char tmp = num_str[i];
            num_str[i] = num_str[num_digits - 1 - i];
            num_str[num_digits - 1 - i] = tmp;
        }
        num_str[num_digits] = '\0'; // 添加字符串结束符
    }

    // 计算实际显示的位数（截断处理）
    int display_digits = (num_digits > digit_count) ? digit_count : num_digits;
    int start_index = (num_digits > digit_count) ? (num_digits - digit_count) : 0;
    
    // 构造左对齐的字符串（后面补空格）
    unsigned char buffer[21] = {0};
    
    // 复制数字部分（从start_index开始取display_digits位）
    for (int i = 0; i < display_digits; i++) {
        buffer[i] = num_str[start_index + i];
    }
    
    // 后面补空格（不足digit_count位时）
    for (int i = display_digits; i < digit_count; i++) {
        buffer[i] = ' ';
    }
    buffer[digit_count] = '\0'; // 确保字符串结束

    // 调用字符串显示函数
    OLED_ShowStr(x, y, buffer, TextSize);
}

/**
 * ************************************************************************
 * @brief 画一个矩形框
 * 
 * @param[in] x        框的左上角横坐标（0～127）
 * @param[in] y        框的左上角纵坐标（0～63）
 * @param[in] width    框的宽度（包括框的粗细，单位：像素）
 * @param[in] height   框的高度（包括框的粗细，单位：像素）
 * @param[in] thickness 框的粗细（单位：像素，至少为1）
 * 
 * ************************************************************************
 */
void OLED_DrawBox(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char thickness)
{
    // 屏幕边界检查
    if (x >= SCREEN_COLUMN || y >= SCREEN_ROW) 
        return;

    // 计算实际显示的宽度和高度（考虑屏幕边界）
    unsigned char width_actual = width;
    unsigned char height_actual = height;
    if (x + width > SCREEN_COLUMN) 
        width_actual = SCREEN_COLUMN - x;
    if (y + height > SCREEN_ROW) 
        height_actual = SCREEN_ROW - y;

    // 检查有效尺寸
    if (width_actual == 0 || height_actual == 0) 
        return;

    // 调整厚度以适应屏幕边界
    unsigned char thickness_top = (thickness > height_actual) ? height_actual : thickness;
    unsigned char thickness_side = (thickness > width_actual) ? width_actual : thickness;

    // 画顶部边框
    for (unsigned char i = 0; i < thickness_top; i++) 
    {
        for (unsigned char j = 0; j < width_actual; j++) 
        {
            OLED_SetPixel(x + j, y + i, 1);
        }
    }

    // 画底部边框
    for (unsigned char i = 0; i < thickness_top; i++) 
    {
        for (unsigned char j = 0; j < width_actual; j++) 
        {
            OLED_SetPixel(x + j, y + height_actual - 1 - i, 1);
        }
    }

    // 画左侧边框
    for (unsigned char i = 0; i < height_actual; i++) 
    {
        for (unsigned char j = 0; j < thickness_side; j++) 
        {
            OLED_SetPixel(x + j, y + i, 1);
        }
    }

    // 画右侧边框
    for (unsigned char i = 0; i < height_actual; i++) 
    {
        for (unsigned char j = 0; j < thickness_side; j++) 
        {
            OLED_SetPixel(x + width_actual - 1 - j, y + i, 1);
        }
    }

    OLED_RefreshRAM();
}


/**
 * ************************************************************************
 * @brief 显示播放/暂停按钮（直径16像素）
 * 
 * @param[in] x       按钮左上角横坐标（0～112）
 * @param[in] y       按钮左上角纵坐标（0～48）
 * @param[in] state   按钮状态（0:暂停 1:播放）
 * 
 * ************************************************************************
 */
void OLED_PlayPauseIcon(unsigned char x, unsigned char y, uint8_t state)
{
		if(state){
			OLED_ShowChinese((signed short int)x,(signed short int)y,(unsigned char*)"丨");
		}else{
			OLED_ShowChinese((signed short int)x,(signed short int)y,(unsigned char*)"乛");
		}
    OLED_RefreshRAM();
}

/**
 * ************************************************************************
 * @brief 传入歌曲序号显示歌曲名
 * 
 * @param[in] x       按钮左上角横坐标（0～112）
 * @param[in] y       按钮左上角纵坐标（0～48）
 * @param[in] num			歌曲序号   
 * 
 * ************************************************************************
 */
void Show_Music_Name(unsigned char x, unsigned char y, uint8_t num){
	OLED_ClearArea((signed short int)x,(signed short int)y,100,16);
	switch(num)
  {
    case 1: //第一首歌，东西南北中
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,(unsigned char*)"东");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"西");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"南");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"北");
			OLED_ShowChinese((signed short int)x+(14*4),(signed short int)y,(unsigned char*)"中");
      break;
    }
		case 2: //第二首歌，动物世界
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"动");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"物");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"世");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"界");
      break;
    }
		case 3: //第三首歌，哥只是个传说
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"哥");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"只");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"是");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"个");
			OLED_ShowChinese((signed short int)x+(14*4),(signed short int)y,(unsigned char*)"传");
			OLED_ShowChinese((signed short int)x+(14*5),(signed short int)y,(unsigned char*)"说");
      break;
    }
		case 4: //第四首歌，明月天涯
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"明");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"月");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"天");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"涯");
      break;
    }
		case 5: //第五首歌，启示录
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"启");
			OLED_ShowChinese((signed short int)x+(16*1),(signed short int)y,(unsigned char*)"示");
			OLED_ShowChinese((signed short int)x+(16*2),(signed short int)y,(unsigned char*)"录");
      break;
    }
		case 6: //第六首歌，起风了
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"起");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"风");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"了");
      break;
    }
		case 7: //第七首歌，山楂树之恋
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"山");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"楂");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"树");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"之");
			OLED_ShowChinese((signed short int)x+(14*4),(signed short int)y,(unsigned char*)"恋");
      break;
    }
		case 8: //第八首歌，天秤座
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"天");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"秤");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"座");
      break;
    }
		case 9: //第九首歌，消愁
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"消");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"愁");
      break;
    }
		case 10: //第十首歌，星河万里
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"星");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"河");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"万");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"里");
      break;
    }
		case 11: //第十一首歌，只要平凡
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"只");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"要");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"平");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"凡");
      break;
    }
		case 12: //第十二首歌，自娱自乐
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"自");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"娱");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"自");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"乐");
      break;
    }
		case 13: //第十三首歌，最初的梦想
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"最");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"初");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"的");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"梦");
			OLED_ShowChinese((signed short int)x+(14*4),(signed short int)y,(unsigned char*)"想");
      break;
    }
		case 14: //第十四首歌，曹操
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"曹");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"操");
      break;
    }
		case 15: //第十五首歌，曾经的你
    {
			OLED_ShowChinese((signed short int)x,(signed short int)y,				(unsigned char*)"曾");
			OLED_ShowChinese((signed short int)x+(14*1),(signed short int)y,(unsigned char*)"经");
			OLED_ShowChinese((signed short int)x+(14*2),(signed short int)y,(unsigned char*)"的");
			OLED_ShowChinese((signed short int)x+(14*3),(signed short int)y,(unsigned char*)"你");
      break;
    }
	
	}
}
