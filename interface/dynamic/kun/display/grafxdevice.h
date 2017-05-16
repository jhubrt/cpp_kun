// The MIT License (MIT)
//
// Copyright(c) 2016-2017 Jerome Hubert
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, 
// copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once

namespace kun
{
    namespace grafx
    {
		struct Keycode
		{
			enum Enum
			{
				LEFT	= 160,
				RIGHT	= 161,
				UP		= 162,
				DOWN	= 163,
				ESC		= 27,
				RETURN	= 13
			};
		};

		class Texture;
		class Font;

		struct PixelFormats
		{
			u32 UNKNOWN; 
			u32 INDEX1LSB;
			u32 INDEX1MSB;
			u32 INDEX4LSB;
			u32 INDEX4MSB;
			u32 INDEX8;
			u32 RGB332;
			u32 RGB444;
			u32 RGB555;
			u32 BGR555;
			u32 ARGB4444; 
			u32 RGBA4444; 
			u32 ABGR4444; 
			u32 BGRA4444; 
			u32 ARGB1555; 
			u32 RGBA5551; 
			u32 ABGR1555; 
			u32 BGRA5551; 
			u32 RGB565;
			u32 BGR565;
			u32 RGB24; 
			u32 BGR24; 
			u32 RGB888;
			u32 RGBX8888; 
			u32 BGR888;
			u32 BGRX8888; 
			u32 ARGB8888; 
			u32 RGBA8888; 
			u32 ABGR8888; 
			u32 BGRA8888; 
			u32 ARGB2101010;
			u32 YV12;
			u32 IYUV;
			u32 YUY2;
			u32 UYVY;
			u32 YVYU;
		};

		struct TextureAccess
		{
			s32 STATIC;
			s32 STREAMING;
			s32 RENDERTARGET;
		};

        class DeviceInterface
        {
        public:
			virtual ~DeviceInterface() {}

			virtual bool init () = 0;

			virtual const PixelFormats&  GetPixelFormats  () const = 0;
			virtual const TextureAccess& GetTextureAccess () const = 0;

			virtual void		SetForegroundColor		(u8 _r, u8 _g, u8 _b, u8 _a)		 = 0;
			virtual void		SetBackgroundColor		(u8 _r, u8 _g, u8 _b, u8 _a)		 = 0;
			virtual void		Clear					()									 = 0;
			virtual void		Line					(int _x1, int _y1, int _x2, int _y2) = 0;
			virtual void		Rect					(int _x, int _y, int _w, int _h)	 = 0;  
			virtual void		FillRect				(int _x, int _y, int _w, int _h)	 = 0;
			
			virtual Texture*	CreateTexture			(u32 _format, s32 _accessmode, u32 _width, u32 _height)	= 0;
			virtual Texture*	CreateTextureFromBMP	(const char* _filename)									= 0;
			virtual void		SetAlpha				(Texture* _texture, u8 _alpha)							= 0;
			virtual void		UpdateStaticTexture		(Texture* _texture, void* _pixels, u32 _pitch)			= 0;
			virtual void		UpdateStaticTexture		(Texture* _texture, void* _pixels, u32 _pitch, u32 _x, u32 _y, u32 _w, u32 _h) = 0;
			virtual void		DestroyTexture			(Texture* _texture)										= 0;

			virtual Font*		AddFont					(const char* _filename, u32 _size)						= 0;
			virtual void		GetTextExtents			(Font* _font, const char* _text, u32& _w, u32& _h)		= 0;
			virtual Texture*	CreateTextureFromText	(Font* _font, const char* _text)						= 0;
			virtual void		Text					(Font* _font, s32 _x, s32 _y, const char* _text)		= 0;
			virtual void		CopyTexture				(Texture* _texture, s32 _x, s32 _y)						= 0;

			virtual void		Present					()										= 0;

			virtual void		GetMouse				(s32* _x, s32* _y, s32* _k, s32* _z)	= 0;
			virtual bool		IsKeyHit				()										= 0;
			virtual s32			GetKey					()										= 0;

			void SetForegroundColor(u32 _color)	{ SetForegroundColor (u8(_color << 24), u8(_color >> 16), u8(_color >> 8), u8(_color)); }
        };
    }
}
