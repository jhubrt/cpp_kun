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

#include "stdafx.h"

#include "interface/dynamic/kun/reflect/metaclass.h"
#include "interface/dynamic/kun/reflect/registry.h"
#include "interface/dynamic/kun/display/grafxdevice.h"

#include "SDL.h"
#include "SDL_ttf.h"

#include <vector>


namespace kun
{
    namespace grafx
    {
		static void InitPixelFormats(PixelFormats& _o)
		{
			_o.UNKNOWN	   = SDL_PIXELFORMAT_UNKNOWN;
			_o.INDEX1LSB   = SDL_PIXELFORMAT_INDEX1LSB;
			_o.INDEX1MSB   = SDL_PIXELFORMAT_INDEX1MSB;
			_o.INDEX4LSB   = SDL_PIXELFORMAT_INDEX4LSB;
			_o.INDEX4MSB   = SDL_PIXELFORMAT_INDEX4MSB;
			_o.INDEX8	   = SDL_PIXELFORMAT_INDEX8;
			_o.RGB332	   = SDL_PIXELFORMAT_RGB332;
			_o.RGB444	   = SDL_PIXELFORMAT_RGB444;
			_o.RGB555	   = SDL_PIXELFORMAT_RGB555;
			_o.BGR555	   = SDL_PIXELFORMAT_BGR555;
			_o.ARGB4444    = SDL_PIXELFORMAT_ARGB4444;
			_o.RGBA4444    = SDL_PIXELFORMAT_RGBA4444;
			_o.ABGR4444    = SDL_PIXELFORMAT_ABGR4444;
			_o.BGRA4444    = SDL_PIXELFORMAT_BGRA4444;
			_o.ARGB1555    = SDL_PIXELFORMAT_ARGB1555;
			_o.RGBA5551    = SDL_PIXELFORMAT_RGBA5551;
			_o.ABGR1555    = SDL_PIXELFORMAT_ABGR1555;
			_o.BGRA5551    = SDL_PIXELFORMAT_BGRA5551;
			_o.RGB565	   = SDL_PIXELFORMAT_RGB565;
			_o.BGR565	   = SDL_PIXELFORMAT_BGR565;
			_o.RGB24	   = SDL_PIXELFORMAT_RGB24;
			_o.BGR24	   = SDL_PIXELFORMAT_BGR24;
			_o.RGB888	   = SDL_PIXELFORMAT_RGB888;
			_o.RGBX8888    = SDL_PIXELFORMAT_RGBX8888;
			_o.BGR888	   = SDL_PIXELFORMAT_BGR888;
			_o.BGRX8888    = SDL_PIXELFORMAT_BGRX8888;
			_o.ARGB8888    = SDL_PIXELFORMAT_ARGB8888;
			_o.RGBA8888    = SDL_PIXELFORMAT_RGBA8888;
			_o.ABGR8888    = SDL_PIXELFORMAT_ABGR8888;
			_o.BGRA8888    = SDL_PIXELFORMAT_BGRA8888;
			_o.ARGB2101010 = SDL_PIXELFORMAT_ARGB2101010;
			_o.YV12		   = SDL_PIXELFORMAT_YV12;
			_o.IYUV		   = SDL_PIXELFORMAT_IYUV;
			_o.YUY2		   = SDL_PIXELFORMAT_YUY2;
			_o.UYVY		   = SDL_PIXELFORMAT_UYVY;
			_o.YVYU		   = SDL_PIXELFORMAT_YVYU;
		};

		static void InitTextureAccess(TextureAccess& _o)
		{
			_o.STATIC		= SDL_TEXTUREACCESS_STATIC;
			_o.STREAMING	= SDL_TEXTUREACCESS_STREAMING;
			_o.RENDERTARGET = SDL_TEXTUREACCESS_TARGET;
		}

        class SDLDevice : public DeviceInterface
        {
			friend class kref::MetaClass<SDLDevice>;

			SDL_Window*             m_window;
			SDL_Renderer*           m_renderer;
			SDL_Surface*            m_windowSurface;

            std::vector<TTF_Font*>	m_fonts;

			SDL_Color		        m_foregroundColor;
			SDL_Color		        m_backgroundColor;

			int				        m_result;
			std::string		        m_error;

			u8					    m_keys[1024];
			std::vector<s32>	    m_keyhits;

			static PixelFormats		ms_pixelFormats;
			static TextureAccess	ms_textureAccess;

        public:

            SDLDevice()
                : m_window   (nullptr)
                , m_renderer (nullptr)
            {
				memset(m_keys, 0, sizeof(m_keys));
			}

			const PixelFormats&  GetPixelFormats  () const override { return ms_pixelFormats; }
			const TextureAccess& GetTextureAccess () const override { return ms_textureAccess; }


            bool init () override
            {
                SDL_Init(SDL_INIT_VIDEO);
               
                SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_SHOWN, &m_window, &m_renderer);

                if ( m_window == nullptr ) 
                {
                    printf ("SDL_Init: %s\n", SDL_GetError());
                    return false;
                }

                m_windowSurface = SDL_GetWindowSurface(m_window); 

                if ( TTF_Init() == -1 ) 
                {
                    printf("TTF_Init: %s\n", TTF_GetError());
                    return false;
                }
                    
                SetForegroundColor (128,128,128,128);
                SetBackgroundColor (0,0,0,0);

				Font* font = AddFont("C:\\Windows\\Fonts\\Consola.ttf", 50);
  
				Text (font, 10,100,"tout ca, tout ca...");

				InitPixelFormats(ms_pixelFormats);
				InitTextureAccess(ms_textureAccess);
				
                SDL_RenderPresent(m_renderer);

                return true;
            }

            ~SDLDevice()
            {
				for (auto f : m_fonts)
				{
					TTF_CloseFont(f);
				}

				m_fonts.clear();

                TTF_Quit();
                SDL_Quit();
            }

			Font* AddFont(const char* _filename, u32 _size) override
			{
				TTF_Font* font = TTF_OpenFont(_filename, _size);

				m_fonts.push_back(font);

				return reinterpret_cast<Font*>(font);
			}

			void Text (Font* _font, s32 _x, s32 _y, const char* _text) override
			{
				SDL_Rect rect;

				m_result = TTF_SizeText(reinterpret_cast<TTF_Font*>(_font), _text, &rect.w, &rect.h);
				
				SDL_Surface* surface = TTF_RenderUTF8_Shaded (reinterpret_cast<TTF_Font*>(_font), _text, m_foregroundColor, m_backgroundColor);
				SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

				rect.x = _x;
				rect.y = _y;

				SDL_RenderCopy(m_renderer, texture, nullptr, &rect);

				SDL_DestroyTexture(texture);
				SDL_FreeSurface(surface);
			}

			Texture* CreateTextureFromText (Font* _font, const char* _text) override
			{
				SDL_Rect rect;

				m_result = TTF_SizeText(reinterpret_cast<TTF_Font*>(_font), _text, &rect.w, &rect.h);

				SDL_Surface* surface = TTF_RenderUTF8_Shaded (reinterpret_cast<TTF_Font*>(_font), _text, m_foregroundColor, m_backgroundColor);
				Texture* texture = reinterpret_cast<Texture*>(SDL_CreateTextureFromSurface(m_renderer, surface));
				SDL_FreeSurface(surface);
				
				return texture;
			}

			void GetTextExtents(Font* _font, const char* _text, u32& _w, u32& _h) override
			{
				SDL_Rect rect;

				m_result = TTF_SizeText(reinterpret_cast<TTF_Font*>(_font), _text, &rect.w, &rect.h);

				_w = rect.w;
				_h = rect.h;
			}

			void CopyTexture (Texture* _texture, s32 _x, s32 _y) override
			{
				SDL_Rect rect;

				rect.x = _x; rect.y = _y;
				SDL_QueryTexture(reinterpret_cast<SDL_Texture*>(_texture), nullptr, nullptr, &rect.w, &rect.h);
				SDL_RenderCopy(m_renderer, reinterpret_cast<SDL_Texture*>(_texture), nullptr, &rect);
			}

			void Clear() override
			{
				SDL_RenderClear(m_renderer);
			}

			void SetForegroundColor(u8 _r, u8 _g, u8 _b, u8 _a) override
			{
				m_foregroundColor.r = _r; m_foregroundColor.g = _g; m_foregroundColor.b = _b; m_foregroundColor.a = _a; 				
				SDL_SetRenderDrawColor(m_renderer, _r, _g, _b, _a);
			}

			void SetBackgroundColor(u8 _r, u8 _g, u8 _b, u8 _a) override
			{
				m_backgroundColor.r = _r; m_backgroundColor.g = _g; m_backgroundColor.b = _b; m_backgroundColor.a = _a; 				
			}

			void Rect(int _x, int _y, int _w, int _h) override
			{
				SDL_Rect rect;

				rect.x = _x;
				rect.y = _y;
				rect.w = _w;
				rect.h = _h;

				SDL_RenderDrawRect(m_renderer, &rect);
			}

			void FillRect(int _x, int _y, int _w, int _h) override
			{
				SDL_Rect rect;

				rect.x = _x;
				rect.y = _y;
				rect.w = _w;
				rect.h = _h;

				SDL_RenderFillRect(m_renderer, &rect);
			}

			void Line(int _x1, int _y1, int _x2, int _y2) override
			{
				SDL_RenderDrawLine(m_renderer, _x1, _y1, _x2, _y2);
			}

			Texture* CreateTexture(u32 _format, s32 _accessmode, u32 _width, u32 _height) override
			{
				return reinterpret_cast<Texture*>(SDL_CreateTexture (m_renderer, _format, _accessmode, _width, _height));
			}

			Texture* CreateTextureFromBMP(const char* _filename) override
			{
				SDL_Surface* surface = SDL_LoadBMP(_filename);
				
				if (surface != nullptr)
				{ 
					Texture* texture = reinterpret_cast<Texture*>(SDL_CreateTextureFromSurface (m_renderer, surface));
					SDL_FreeSurface(surface);
					
					return texture;
				}
				else
				{
					m_error = SDL_GetError();

					return nullptr;
				}
			}

			void SetAlpha(Texture* _texture, u8 _alpha) override
			{
				SDL_SetTextureAlphaMod(reinterpret_cast<SDL_Texture*>(_texture), _alpha);
			}

			void UpdateStaticTexture(Texture* _texture, void* _pixels, u32 _pitch) override
			{
				m_result = SDL_UpdateTexture(reinterpret_cast<SDL_Texture*>(_texture), nullptr, _pixels, _pitch);
			}

			void UpdateStaticTexture(Texture* _texture, void* _pixels, u32 _pitch, u32 _x, u32 _y, u32 _w, u32 _h) override
			{
				SDL_Rect rect;

				rect.x = _x; rect.y = _y; rect.w = _w; rect.h = _h;

				m_result = SDL_UpdateTexture(reinterpret_cast<SDL_Texture*>(_texture), nullptr, _pixels, _pitch);
			}

			void DestroyTexture(Texture* _texture) override
			{
				SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(_texture));
			}

			void Present () override
			{
				SDL_RenderPresent(m_renderer);
				SDL_PumpEvents();

				int numkeys;

				const Uint8* keys = SDL_GetKeyboardState(&numkeys);
				assert(numkeys <= sizeof(m_keys));

				for (int t = 0; t < numkeys; t++)
				{
					if ((m_keys[t] == 0) && (keys[t] != 0))
					{		
						s32 key = 0;

						switch (t)
						{
						case SDL_SCANCODE_LEFT:		key = Keycode::LEFT;	break;
						case SDL_SCANCODE_RIGHT:	key = Keycode::RIGHT;	break;
						case SDL_SCANCODE_UP:		key = Keycode::UP;		break;
						case SDL_SCANCODE_DOWN:		key = Keycode::DOWN;	break;
						case SDL_SCANCODE_RETURN:
						case SDL_SCANCODE_RETURN2:	key = Keycode::RETURN;  break;

						default:
							key = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(t));
						}
						
						m_keyhits.push_back(key);
					}
					
					m_keys[t] = keys[t];
				}
			}

			void GetMouse (s32* _x, s32* _y, s32* _k, s32* _z) override
			{
				int x, y;
				
				u32 sdlbutton = SDL_GetMouseState (&x, &y);

				if (_x != nullptr)
					*_x = x;

				if (_y != nullptr)
					*_y = y;

				if (_k != nullptr)
				{
					*_k  =  (sdlbutton & SDL_BUTTON_LEFT ) != 0;
					*_k |= ((sdlbutton & SDL_BUTTON_RIGHT) != 0) << 1;
				}
			}

			bool IsKeyHit() override
			{
				return m_keyhits.size() > 0;
			}

			s32 GetKey() override
			{
				s32 key = 0;

				if (m_keyhits.size())
				{
					key = m_keyhits[0];
					m_keyhits.erase(m_keyhits.begin());
				}

				return key;
			}

		};

		PixelFormats	SDLDevice::ms_pixelFormats;
		TextureAccess	SDLDevice::ms_textureAccess;
    }
}


template<>void kref::MetaClass<kun::grafx::SDLDevice>::Register()
{
	kref::Registry::Register<kun::grafx::SDLDevice>();
}


KREF_CLASS(kun::grafx::SDLDevice);

KREF_CLASS_SUPERS_BEGIN(kun::grafx::SDLDevice)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::grafx::SDLDevice)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::grafx::SDLDevice)
KREF_CLASS_METHODS_END
