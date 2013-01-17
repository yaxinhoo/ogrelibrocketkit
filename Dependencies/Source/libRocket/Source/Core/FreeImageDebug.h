
/** 
使用FreeImage写FreeType2字体图片
*/  

#ifndef FREEIMAGEDEBUG_H
#define FREEIMAGEDEBUG_H

#include <iostream>   
#include <string>   

#include <FreeImage.h>   
#include <ft2build.h>
#include FT_FREETYPE_H   
#include FT_GLYPH_H   
////////////////////////////////////////////////////////////////////////////////   
//----------------------------------------------------------------------------//   
// 初始化函数   
bool Init(const std::string &fontFileName);  
// 写到图片   
bool DrawToImage(wchar_t uchar,int fw,int fh,const std::string &imgFileName);  
// 清理资源   
void DeInit(void);  

void SaveToFile(FT_Bitmap* abmp,const std::string &fileName);

void ByteToFile(void* bytes,int w,int h,const std::string &fileName);

#endif