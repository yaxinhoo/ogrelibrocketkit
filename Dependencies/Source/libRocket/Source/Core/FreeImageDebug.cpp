/** 
使用FreeImage写FreeType2字体图片 
*/  
#include "FreeImageDebug.h"

// 全局句柄   
FT_Library g_ftLib;  
FT_Face g_ftFace;

/*
////////////////////////////////////////////////////////////////////////////////   
//----------------------------------------------------------------------------//   
int main(int argc,char **argv)  
{  
	using namespace std;  
	if ( argc < 2 || !Init(argv[1]) )  
	{  
		cout<<"初始化失败!"<<endl;  
		return -1;  
	}  
	DrawToImage(L'家',48,48,"jia.jpg");  

	DeInit();  
	return 0;  
}
*/

////////////////////////////////////////////////////////////////////////////////   
//----------------------------------------------------------------------------//   
bool Init(const std::string &fontFileName)  
{  
	// 初始化字体库   
	if ( FT_Init_FreeType(&g_ftLib) ) { return false; }  
	if ( FT_New_Face(g_ftLib,fontFileName.c_str(),0,&g_ftFace) ) { return false; }  
	// FreeImage   
	FreeImage_Initialise();  
	return true;  
}  
//----------------------------------------------------------------------------//   
bool DrawToImage(wchar_t uchar,int fw,int fh,const std::string &fileName)  
{  
	// 设置字体大小   
	FT_Set_Char_Size(g_ftFace,fw<<6,fh<<6,96,96);  
	// 加载并渲染字体GLYPH   
	FT_Load_Glyph(g_ftFace,FT_Get_Char_Index(g_ftFace,uchar),FT_LOAD_DEFAULT);  
	FT_Glyph glyph;  
	FT_Get_Glyph(g_ftFace->glyph,&glyph);  
	FT_Render_Glyph(g_ftFace->glyph,FT_RENDER_MODE_NORMAL);  
	// 拷贝到图片写入文件   
	FT_Bitmap *pBmp = &g_ftFace->glyph->bitmap;  
	int w = pBmp->width;  
	int h = pBmp->rows;  
	FIBITMAP *fib = FreeImage_Allocate(w,h,24);  
	RGBQUAD rgb;  
	for ( int i=0;i<h;++i )  
	{  
		for ( int j=0;j<w;++j )  
		{  
			rgb.rgbRed = pBmp->buffer[i*w+j];  
			rgb.rgbGreen = pBmp->buffer[i*w+j];  
			rgb.rgbBlue = pBmp->buffer[i*w+j];  
			FreeImage_SetPixelColor(fib,j,h-i,&rgb);  
		}  
	}  
	FreeImage_Save(FIF_JPEG,fib,fileName.c_str());  
	FreeImage_Unload(fib);  
	return true;  
}
//----------------------------------------------------------------------------//
void SaveToFile(FT_Bitmap* abmp,const std::string &fileName)
{
    int w = abmp->width;
	int h = abmp->rows;
	FIBITMAP *fib = FreeImage_Allocate(w,h,24);
	RGBQUAD rgb;
	for ( int i=0;i<h;++i )
	{
		for ( int j=0;j<w;++j )
		{
			rgb.rgbRed = abmp->buffer[i*w+j];
			rgb.rgbGreen = abmp->buffer[i*w+j];
			rgb.rgbBlue = abmp->buffer[i*w+j];
			FreeImage_SetPixelColor(fib,j,h-i,&rgb);
		}
	}
	FreeImage_Save(FIF_JPEG,fib,fileName.c_str());
	FreeImage_Unload(fib);
}

//----------------------------------------------------------------------------//
void ByteToFile(void* bytes,int w,int h,const std::string &fileName)
{
    FIBITMAP *fib = FreeImage_Allocate(w,h,24);
	RGBQUAD rgb;
    BYTE* bbytes=(BYTE*)bytes;
	for ( int i=0;i<h;++i )
	{
		for ( int j=0;j<w;++j )
		{
			rgb.rgbRed = (BYTE)bbytes[i*w+j];
			rgb.rgbGreen = (BYTE)bbytes[i*w+j];
			rgb.rgbBlue = (BYTE)bbytes[i*w+j];
			FreeImage_SetPixelColor(fib,j,h-i,&rgb);
		}
	}
	FreeImage_Save(FIF_JPEG,fib,fileName.c_str());
	FreeImage_Unload(fib);
}

//----------------------------------------------------------------------------//   
void DeInit()  
{  
	FT_Done_Face(g_ftFace);  
	FT_Done_FreeType(g_ftLib);  
	FreeImage_DeInitialise();  
}  