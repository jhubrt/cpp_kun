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

#include <conio.h>

#include "interface\dynamic\kun\reflect\id.h"
#include "interface\dynamic\kun\reflect\metaclass.h"
#include "interface\dynamic\kun\reflect\registry.h"
#include "interface\dynamic\kun\reflect\unittestbase.h"
#include "interface\dynamic\kun\display\grafxdevice.h"

#include "implement\kun\ui\assetlib.h"

#include "implement\kun\ui\widgetmanager.h"
#include "implement\kun\ui\button.h"
#include "implement\kun\ui\popmenu.h"
#include "implement\kun\ui\canvas.h"
#include "implement\kun\ui\graphicinstanceinterface.h"

#include "interface\dynamic\kun\reflect\binstream.h"
#include "interface\dynamic\kun\reflect\jsonwriterstream.h"
#include "implement\kun\reflect\jsonreader.h"

#include <time.h>


#define UI_TEST_FOLDER "..\\..\\test\\"

//---------------------------------------------------------------------------
// Unit test class
//---------------------------------------------------------------------------
namespace kun
{
	grafx::DeviceInterface* g_drawer = nullptr;

	namespace ui
	{
		class GraphicInstance : public GraphicInstanceInterface
		{
		public:
			virtual void pushCommand(u32 _color, const Coord& _pos, const Coord& _size)
			{
				g_drawer->SetForegroundColor (_color);
				g_drawer->FillRect ((int)_pos.m_x, (int)_pos.m_y, (int)_size.m_x, (int)_size.m_y);
			}
		};

		void debugDraw(float _x, float _y, float _w, float _h, const char* _text)
		{
			g_drawer->SetForegroundColor(0xFFFF00);

			g_drawer->Rect((int)_x, (int)_y, (int)_w, (int)_h);

			if (_text != nullptr)
			{
				//g_drawer->Text((int)_x, (int)_y, _text);
			}
		}
	}
	
	namespace test
	{
		namespace ui
		{
			struct App : kref::Polymorph
			{
				App() : kref::Polymorph(this) {}

				static bool TestUI_display(reflect::Polymorph* _this, const kui::Message& _message)
				{
					printf("method called on %p\n", _this);
					printf("sender is %p (%s)\n", _message.m_sender, typeid(*_message.m_sender).name());
					printf("message type %d\n", _message.m_type);

					return false;
				}
			};

			struct UnitTest
			{
				static void TestSerializeUI()
				{
					{
						kui::Config config;


						config.m_config = std::make_unique<kui::CanvasConfig>();
						config.m_eventsmap.emplace_back ("po.pa", kref::StringCRC("assign"), "pa.po", kref::StringCRC("open"));
						config.m_eventsmap.emplace_back ("po.pi", kref::StringCRC("assign"), "pa.pi", kref::StringCRC("close"));

						auto& canvasconfig = *config.m_config.get();

						canvasconfig.m_x = 0;
						canvasconfig.m_y = 0;
						canvasconfig.m_w = 640;
						canvasconfig.m_h = 480;

						auto button1 = std::make_unique<kui::ButtonConfig>();

						button1->m_x = 10;
						button1->m_y = 10;
						button1->m_w = 20;
						button1->m_h = 20;
						button1->m_type = 1;
						button1->m_text = "testtext";

						canvasconfig.m_children.push_back(button1.get());

						auto popmenu = std::make_unique<kui::PopMenuConfig>();

						popmenu->m_popmenus.resize(2);
						popmenu->m_buttons.resize(3);

						canvasconfig.m_children.push_back(popmenu.get());

						kref::StdFile file(UI_TEST_FOLDER "uitest.json", kref::StdFile::WRITE);
						kref::JSonWriterStream stream(file);

						kref::Serializer serializer(stream, kref::SerializeClassUltraLight);

						kref::Serialize(serializer, config);
					}

					{
						kref::Stream* jsonstream = nullptr;

						{
							kref::StdFile tracefileout ("tracefileui.log", kref::StdFile::WRITE);
							jsonstream = kref::JsonRead(UI_TEST_FOLDER "uitest.json", &tracefileout);
						}

						kref::StdFile tracefile ("tracefileui.log", kref::StdFile::READ);
						kref::Stream* stream = jsonstream->CreateTracer(nullptr, &tracefile, kref::TracerBinStream::Check::STRONG);

						void* object;

						//kref::Deserializer deserializer(*stream, kref::DeserializeClass, kref::DeserializeClassBodyonly);
						kref::Deserializer deserializer(*stream, kref::DeserializeClass, kref::DeserializeClassBodyonlyUltraLight);

						auto metaclass = kun::SinglePtr<kref::Registry>::Get()->TryDeserializeClass(deserializer, object);
						assert(metaclass != nullptr);

						delete jsonstream;
						delete stream;
					}
				}

				static void TestUI()
				{
					auto registry = kun::SinglePtr<kref::Registry>::Get();

					auto gfxdevice = registry->FindMetaClass(kref::StringCRC("kun::grafx::SDLDevice"))->NewAbstract<kun::grafx::DeviceInterface>();

					g_drawer = gfxdevice;

					gfxdevice->init();

					{
						kui::AssetLibrary* assetLibrary = nullptr;

						{
							kref::Stream* stream = kref::JsonRead(UI_TEST_FOLDER "uiassetlib.json");
							assert(stream != nullptr);

							void* object;

							kref::Deserializer deserializer(*stream, kref::DeserializeClass, kref::DeserializeClassBodyonlyUltraLight);

							auto metaclass = kun::SinglePtr<kref::Registry>::Get()->TryDeserializeClass(deserializer, object);
							assert(metaclass != nullptr);
							assert(kref::MetaClass<kui::AssetLibrary>::IsType(*metaclass));

							assetLibrary = static_cast<kui::AssetLibrary*>(object);

							assetLibrary->Init(gfxdevice);
							assetLibrary->LoadAssets();
						}

						auto font			 = gfxdevice->AddFont ("C:\\Windows\\Fonts\\Consola.ttf", 16);
						auto graphicInstance = new kui::GraphicInstance;
						auto shared			 = new kui::Shared(*graphicInstance, *gfxdevice, *assetLibrary);

						kui::WidgetManager manager(640, 480, shared);
						u32 mouseDevice;
						u32 keyboardDevice;

						{
							kui::InputDevice inputDevice(kui::InputDeviceType::MOUSE);
							inputDevice.m_validate = 1;

							mouseDevice = manager.addInputDevice(inputDevice);
						}

						{
							kui::InputDevice inputDevice(kui::InputDeviceType::KEYBOARD);

							inputDevice.m_validate		= grafx::Keycode::RETURN;
							inputDevice.m_navigateDown	= grafx::Keycode::DOWN;
							inputDevice.m_navigateUp	= grafx::Keycode::UP;
							inputDevice.m_navigateLeft	= grafx::Keycode::LEFT;
							inputDevice.m_navigateRight = grafx::Keycode::RIGHT;

							keyboardDevice = manager.addInputDevice(inputDevice);
						}
						
						{
							kref::Stream* stream = kref::JsonRead(UI_TEST_FOLDER "uiconfig.json");

							void* object;

							kref::Deserializer deserializer(*stream, kref::DeserializeClass, kref::DeserializeClassBodyonlyUltraLight);

							auto metaclass = kun::SinglePtr<kref::Registry>::Get()->TryDeserializeClass(deserializer, object);
							assert(metaclass != nullptr);
							assert(kref::MetaClass<kui::Config>::IsType(*metaclass));

							kui::Skin skin;
							App app;

							skin.m_objectsMap.insert (std::make_pair(reflect::StringCRC("app"), &app));

							skin.m_widgetManager = &manager;
							skin.m_font = font;

							auto config	= safe_cast<kui::Config*>(static_cast<kref::Polymorph*>(object));
							config->Create(manager.getRoot(), skin);

							delete stream;
						}

						s32 lx = 0, ly = 0, lk = 0;

						clock_t start = clock();
						bool run = true;

						do
						{
							long x = 0, y = 0, k = 0;

							g_drawer->GetMouse(&x, &y, &k, nullptr);

							kui::InputEvent inputEvent;

							inputEvent.m_event.m_deviceId = mouseDevice;

							if ((x != lx) || (y != ly))
							{
								inputEvent.m_event.m_type = kui::InputEvent::Type::AXIS_2D_CHANGED;
								inputEvent.m_event2D.m_x = (float)x;
								inputEvent.m_event2D.m_y = (float)y;

								manager.pushInputEvent(inputEvent);

								lx = x;
								ly = y;
							}

							if (k != lk)
							{
								if ((k & 1) != (lk & 1))
								{
									inputEvent.m_event.m_type = k & 1 ? kui::InputEvent::Type::KEY_PRESS : kui::InputEvent::Type::KEY_RELEASE;
									inputEvent.m_eventKey.m_keyCode = 1;
									manager.pushInputEvent(inputEvent);
								}

								if ((k & 2) != (lk & 2))
								{
									inputEvent.m_event.m_type = k & 2 ? kui::InputEvent::Type::KEY_PRESS : kui::InputEvent::Type::KEY_RELEASE;
									inputEvent.m_eventKey.m_keyCode = 2;
									manager.pushInputEvent(inputEvent);
								}

								lk = k;
							}

							if (g_drawer->IsKeyHit())
							{
								inputEvent.m_event.m_deviceId = keyboardDevice;
								inputEvent.m_eventKey.m_keyCode = g_drawer->GetKey();
								inputEvent.m_event.m_type = kui::InputEvent::Type::KEY_PRESS;
								manager.pushInputEvent(inputEvent);

								inputEvent.m_event.m_type = kui::InputEvent::Type::KEY_RELEASE;
								manager.pushInputEvent(inputEvent);

								switch (inputEvent.m_eventKey.m_keyCode)
								{
								case 27:
									run = false;
									break;

								case 'D':
								case 'd':
								{
									kui::DebugWidgetDumper dumper(UI_TEST_FOLDER "tree.txt");

									manager.getRoot()->m_node->visit(dumper);
									manager.DebugDumpActiveWidgetsList(UI_TEST_FOLDER "flat.txt");
								}
								break;
								}
							}

							g_drawer->SetForegroundColor (0, 192, 0, 255);
							g_drawer->Clear ();

							clock_t end = clock();
							float dt = (float)(end - start) / (float)CLOCKS_PER_SEC;
							start = end;

							manager.update(dt);

							g_drawer->SetForegroundColor (0, 128, 0, 255);

							const int width = (int)manager.getRoot()->m_node->getSize().m_x;
							const int height = (int)manager.getRoot()->m_node->getSize().m_y;

							for (int i = 0; i < width; i += 10)
							{
								g_drawer->Line(i, 0, i, height);
							}

							for (int i = 0; i < height; i += 10)
							{
								g_drawer->Line(0, i, width, i);
							}
								
							g_drawer->Present();
						}
						while (run);

						delete assetLibrary;

#						ifdef _DEBUG
						kref::DebugNamesCRC::ms_instance.DumpCRCNames("crcnames.txt");
#						endif
					}

					delete gfxdevice;
				}

			};

		} // ui::
	} // test::
} // kun::

// Unit test class registration
KREF_CLASS(kun::test::ui::UnitTest);

KREF_CLASS_SUPERS_BEGIN(kun::test::ui::UnitTest)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::test::ui::UnitTest)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::test::ui::UnitTest)
KREF_CLASS_FUNCTION(kun::test::ui::UnitTest, TestSerializeUI, kun::reflect::TestPrototype),
KREF_CLASS_FUNCTION(kun::test::ui::UnitTest, TestUI,		  kun::reflect::TestPrototype),
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::test::ui::UnitTest>::Register()
{
	kref::Registry::Register<kun::test::ui::UnitTest>();

	KREF_CLASS_ATTRIBUTES(kun::reflect::UnitTestAttr("kui"));
}


KREF_CLASS(kun::test::ui::App);

KREF_CLASS_SUPERS_BEGIN(kun::test::ui::App)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::test::ui::App)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::test::ui::App)
KREF_CLASS_FUNCTION(kun::test::ui::App, TestUI_display,  kui::EventCallPrototype	),
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::test::ui::App>::Register()
{
	kref::Registry::Register<kun::test::ui::App>();
}
