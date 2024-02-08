// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Test/Test>

#include <Mib/CommandLine/AnsiEncoding>
#include <Mib/CommandLine/AnsiEncodingParse>
#include <Mib/CommandLine/Terminal>
#include <Mib/Process/Platform>
#include <Mib/Process/StdIn>
#include <Mib/Cryptography/RandomID>
#include <deque>

namespace NMib::NCommandLine
{
	using namespace NStr;

	class CTerminal_Tests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestSuite("TextCJK")
			{
				CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_Color | EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color24Bit);
				CTerminalSurface Surface(AnsiEncoding, {20, 4});

				// Every codepoint occupies one cell (double width rendering is not supported yet),
				// so CJK text lands as one codepoint per cell
				CTextOptions Options;
				Options.m_DestinationArea = CTerminalArea{{0, 0}, {20, 1}};
				CTerminalArea TextArea = Surface.f_Text("日本語", Options);

				DMibExpect(TextArea.m_Size.m_Width, ==, 3);
				DMibExpect(Surface.f_GetChixel({0, 0}).f_GetCharacter(), ==, ch32(U'日'));
				DMibExpect(Surface.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32(U'本'));
				DMibExpect(Surface.f_GetChixel({2, 0}).f_GetCharacter(), ==, ch32(U'語'));
				DMibExpect(Surface.f_GetChixel({3, 0}).f_GetCharacter(), ==, ch32(' '));

				Options.m_DestinationArea = CTerminalArea{{0, 1}, {20, 1}};
				TextArea = Surface.f_Text("中文字符", Options);

				DMibExpect(TextArea.m_Size.m_Width, ==, 4);
				DMibExpect(Surface.f_GetChixel({0, 1}).f_GetCharacter(), ==, ch32(U'中'));
				DMibExpect(Surface.f_GetChixel({3, 1}).f_GetCharacter(), ==, ch32(U'符'));

				// The rendered escape stream carries the characters back out as UTF-8
				CStr Rendered = Surface.f_Render(ETerminalRenderFlag::mc_None);
				DMibExpect(Rendered.f_Find("日本語") >= 0, ==, true);
				DMibExpect(Rendered.f_Find("中文字符") >= 0, ==, true);
			};

			DMibTestSuite("TextOverflow")
			{
				CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_Color | EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color24Bit);
				CTerminalSurface Surface(AnsiEncoding, {10, 3});

				// "e" followed by U+0301 (combining acute): the combining codepoint does not take a
				// cell of its own, it lands in the overflow of the base character's cell
				CTextOptions Options;
				Options.m_DestinationArea = CTerminalArea{{0, 0}, {10, 1}};
				CTerminalArea TextArea = Surface.f_Text("xe\u0301y", Options);

				DMibExpect(TextArea.m_Size.m_Width, ==, 3);
				DMibExpect(Surface.f_GetChixel({0, 0}).f_GetCharacter(), ==, ch32('x'));
				DMibExpect(Surface.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('e'));
				DMibExpect(Surface.f_GetChixel({2, 0}).f_GetCharacter(), ==, ch32('y'));

				DMibExpect(Surface.f_GetChixel({0, 0}).m_bHasOverflow, ==, false);
				DMibExpect(Surface.f_GetChixel({1, 0}).m_bHasOverflow, ==, true);
				DMibExpect(Surface.f_GetOverflow({0, 0}) == nullptr, ==, true);

				NStr::CStr const *pOverflow = Surface.f_GetOverflow({1, 0});
				DMibExpect(pOverflow != nullptr, ==, true);
				DMibExpect(pOverflow ? *pOverflow : CStr(), ==, CStr("\u0301"));

				// A multi byte CJK base character collects combining overflow the same way
				Options.m_DestinationArea = CTerminalArea{{0, 1}, {10, 1}};
				Surface.f_Text("語\u0301", Options);

				DMibExpect(Surface.f_GetChixel({0, 1}).f_GetCharacter(), ==, ch32(U'語'));

				NStr::CStr const *pCJKOverflow = Surface.f_GetOverflow({0, 1});
				DMibExpect(pCJKOverflow != nullptr, ==, true);
				DMibExpect(pCJKOverflow ? *pCJKOverflow : CStr(), ==, CStr("\u0301"));

				// Rendering emits base and overflow codepoints together
				CStr Rendered = Surface.f_Render(ETerminalRenderFlag::mc_None);
				DMibExpect(Rendered.f_Find("e\u0301") >= 0, ==, true);
				DMibExpect(Rendered.f_Find("語\u0301") >= 0, ==, true);

				// Blit carries overflow into the destination surface
				CTerminalSurface Destination(AnsiEncoding, {10, 3});
				Destination.f_Blit(Surface, Surface.f_Area(), {0, 0});

				DMibExpect(Destination.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('e'));

				NStr::CStr const *pBlitOverflow = Destination.f_GetOverflow({1, 0});
				DMibExpect(pBlitOverflow != nullptr, ==, true);
				DMibExpect(pBlitOverflow ? *pBlitOverflow : CStr(), ==, CStr("\u0301"));

				// BlitMerge carries overflow for cells with glyphs
				CTerminalSurface Merged(AnsiEncoding, {10, 3});
				Merged.f_BlitMerge(Surface, Surface.f_Area(), {0, 0});

				NStr::CStr const *pMergeOverflow = Merged.f_GetOverflow({1, 0});
				DMibExpect(pMergeOverflow != nullptr, ==, true);
				DMibExpect(pMergeOverflow ? *pMergeOverflow : CStr(), ==, CStr("\u0301"));

				// A diff between surfaces differing only in overflow re-emits the cell. The area is
				// two cells wide because a combining character is only kept with its base within a
				// line, and a one cell line has no room for both.
				CTerminalSurface Changed(Surface);
				Options.m_DestinationArea = CTerminalArea{{1, 0}, {2, 1}};
				Changed.f_Text("e\u0300", Options); // U+0300 combining grave

				DMibExpect(Changed.f_GetChixel({1, 0}) == Surface.f_GetChixel({1, 0}), ==, true);
				CStr Diff = Changed.f_DiffRender(Surface, ETerminalRenderFlag::mc_None);
				DMibExpect(Diff.f_Find("e\u0300") >= 0, ==, true);

				// A diff between identical overflow cells emits nothing for them
				CTerminalSurface Same(Surface);
				CStr NoDiff = Same.f_DiffRender(Surface, ETerminalRenderFlag::mc_None);
				DMibExpect(NoDiff.f_Find("e\u0301") >= 0, ==, false);

				// Overwriting the cell with a plain character detaches the overflow
				Options.m_DestinationArea = CTerminalArea{{1, 0}, {1, 1}};
				Changed.f_Text("z", Options);

				DMibExpect(Changed.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('z'));
				DMibExpect(Changed.f_GetChixel({1, 0}).m_bHasOverflow, ==, false);
				DMibExpect(Changed.f_GetOverflow({1, 0}) == nullptr, ==, true);

				// And combining input after the overwrite starts a fresh overflow
				Options.m_DestinationArea = CTerminalArea{{1, 0}, {2, 1}};
				Changed.f_Text("a\u0308", Options); // U+0308 combining diaeresis

				DMibExpect(Changed.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('a'));

				NStr::CStr const *pFreshOverflow = Changed.f_GetOverflow({1, 0});
				DMibExpect(pFreshOverflow != nullptr, ==, true);
				DMibExpect(pFreshOverflow ? *pFreshOverflow : CStr(), ==, CStr("\u0308"));

				// Clear resets the overflow store
				Changed.f_Clear();
				DMibExpect(Changed.f_GetChixel({1, 0}).m_bHasOverflow == false, ==, true);
				DMibExpect(Changed.f_GetOverflow({1, 0}) != nullptr, ==, false);
			};

			// https://github.com/audulus/rui

			DMibTestSuite("General")
			{
				CAnsiEncoding AnsiEncoding(fg_TestAnsiEncodingFlags());
				//CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color | EAnsiEncodingFlag_ColorSgrUsesSemiColon);
				//CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color | EAnsiEncodingFlag_Color24Bit);
				//CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color | EAnsiEncodingFlag_ColorLightBackground);
				//CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color | EAnsiEncodingFlag_ColorLightBackground | EAnsiEncodingFlag_Color24Bit);

				int32 Width = fg_TestTerminalWidth();
				int32 Height = fg_TestTerminalHeight();

				if (Width == 0 || Height == 0)
					return;

				DMibConOut("Height = {}\n\n\n", Height);
				//int32 Width = 286;
				//int32 Height = 102;
				//int32 Width = fg_Max(ConsoleProperties.m_Width, 499);
				//int32 Height = fg_Max(ConsoleProperties.m_Height, 163);
				int32 TextAreaWidth = Width - 20;
				int32 TextAreaHeight = Height - 10;
				//int32 TextAreaWidth = 100;
				//int32 TextAreaHeight = 60;

				CStr Fpss;

				NAtomic::TCAtomic<bool> bTerminate = false;
				auto CleanupTerminate = NProcess::NPlatform::fg_Process_WaitForTermination([&]{bTerminate = true;});
				NProcess::CStdInReader Reader(NProcess::CStdInReaderParams::fs_Create([&](NProcess::EStdInReaderOutputType, const NStr::CStrSecure &){bTerminate = true;}));

				CStr Startup = "{}{}{}"_f << AnsiEncoding.f_Reset() << AnsiEncoding.f_ShowCursor(false) << AnsiEncoding.f_EnableAlternativeScreenBuffer(false);
				DMibConOut("{}\n", Startup);
				NTime::CStopwatch Stopwatch;
				Stopwatch.f_Start();
				fp64 LastTime = Stopwatch.f_GetTime();
				fp64 LastRenderTime = 0.0;
				fp64 LastDrawTime = 0.0;
				NStorage::TCOptional<fp64> LastOutputTime;
				fp64 LastRenderStartTime = 0.0;
				fp64 MaxFrameTime = 0.0;
				umint nFrames = 0;
				umint MaxFrameSize = 0;
				umint LastFrameSize = 0;
				NContainer::TCVector<fp64> FrameTimes;
				NContainer::TCVector<fp64> OutputTimes;
				std::deque<fp64> FrameTimeQueue;
				std::deque<fp64> OutputTimeQueue;

				struct CRenderStats
				{
					fp64 m_RenderTime;
					aint m_FrameSize;
					NStorage::TCOptional<fp64> m_OutputTime;
				};

				NConcurrency::TCFuture<CRenderStats> LastRenderFuture;
				NConcurrency::TCFuture<fp64> LastOutputFuture;

				static constexpr bool c_bDoPipelining = true;
				static constexpr bool c_bDoDiffRender = true;
#if 0
				[[maybe_unused]] bool bFirst = true;
				CTerminalSize Size{Width, Height};
				CTerminalSurface LastSurface(AnsiEncoding, Size);

				for (int32 y = 2; y < 12 && !bTerminate; ++y)
				{
					MaxFrameTime = 0;
					for (int32 x = 0; x < 20 && !bTerminate; ++x)
					{
						++nFrames;
						fp64 Time = Stopwatch.f_GetTime();
						fp64 FrameTime = Time - LastTime;
						LastTime = Time;

						CTerminalSurface Surface(AnsiEncoding, Size);

						CTerminalArea TextDestination{{x, y}, {TextAreaWidth, TextAreaHeight}};

						CTextOptions RenderTextOptions;
						RenderTextOptions.m_Aligment = ETerminalTextAlignment::mc_VerticalCenter | ETerminalTextAlignment::mc_HorizontalCenter;
						RenderTextOptions.m_DestinationArea = TextDestination;
						//Surface.f_Fill(TextDestination, {nullptr, {CTerminalColor::fs_FromRGBA(128, 128, 128), nullptr, nullptr}});
						//Surface.f_GradientHorizontal
						//	(
						//		TextDestination
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(0, 0, 0), nullptr, nullptr}}
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(255, 255, 255), nullptr, nullptr}}
						//	)
						//;
						//Surface.f_GradientVertical
						//	(
						//		TextDestination
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(0, 0, 0), nullptr, nullptr}}
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(255, 255, 255), nullptr, nullptr}}
						//	)
						//;
						//Surface.f_GradientHorizontal
						//	(
						//		TextDestination
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(0, 128, 0), nullptr, nullptr}}
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(128, 0, 128), nullptr, nullptr}}
						//	)
						//;

						//auto Flags = CChixelProperties::fs_UnderlineToFlags(CAnsiEncoding::EUnderline::mc_Double);

						Surface.f_Gradient
							(
								TextDestination
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = CTerminalColor::fs_FromRGBA(255, 0, 0), .m_ForegroundColor = nullptr, .m_ChixelFlags = nullptr}
								}
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = CTerminalColor::fs_FromRGBA(0, 255, 0), .m_ForegroundColor = nullptr, .m_ChixelFlags = nullptr}
								}
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = CTerminalColor::fs_FromRGBA(0, 0, 255), .m_ForegroundColor = nullptr, .m_ChixelFlags = nullptr}
								}
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = CTerminalColor::fs_FromRGBA(255, 255, 0), .m_ForegroundColor = nullptr, .m_ChixelFlags = nullptr}
								}
							)
						;
						//Surface.f_GradientVertical
						//	(
						//		TextDestination
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(0, 128, 0), nullptr, nullptr}}
						//		, {nullptr, {CTerminalColor::fs_FromRGBA(128, 0, 128), nullptr, nullptr}}
						//	)
						//;
						//RenderTextOptions.m_Properties.m_BackgroundColor = CTerminalColor{128, 128, 128};
						RenderTextOptions.m_Properties.m_ChixelFlags = EChixelFlag::mc_Italic;
						RenderTextOptions.m_Properties.f_Weight(CAnsiEncoding::EWeight::mc_Bold);
						RenderTextOptions.m_Properties.f_Underline(CAnsiEncoding::EUnderline::mc_Double);
						RenderTextOptions.m_Properties.m_UnderlineColor = CTerminalColor::fs_FromRGBA(255, 0, 0);
						CTerminalArea TextBox = Surface.f_Text("Testing\nTesting More Lines\nAnd then Some", RenderTextOptions);

						Surface.f_Text
							(
								"Frame {fe3,sj7} ms ({fe0,sj4} fps)    Draw {fe3,sj7} ms    Render {fe3,sj7} ms    Output {fe3,sj7} ms    FrameSize {ns }"_f
								<< (FrameTime * 1000.0)
								<< (fp64(1.0) / FrameTime)
								<< (LastDrawTime * 1000.0)
								<< (LastRenderTime * 1000.0)
								<< (LastOutputTime ? (*LastOutputTime * 1000.0) : fp64::fs_QNan())
								<< LastFrameSize
								, CTextOptions()
							)
						;

						MaxFrameTime = fg_Max(MaxFrameTime, FrameTime);

						Surface.f_Gradient
							(
								TextBox
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = nullptr, .m_ForegroundColor = CTerminalColor::fs_FromRGBA(128, 128, 128), .m_ChixelFlags = nullptr}
								}
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = nullptr, .m_ForegroundColor = CTerminalColor::fs_FromRGBA(255, 128, 128), .m_ChixelFlags = nullptr}
								}
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = nullptr, .m_ForegroundColor = CTerminalColor::fs_FromRGBA(128, 255, 128), .m_ChixelFlags = nullptr}
								}
								,
								{
									.m_Character = nullptr
									, .m_Properties = {.m_BackgroundColor = nullptr, .m_ForegroundColor = CTerminalColor::fs_FromRGBA(128, 128, 255), .m_ChixelFlags = nullptr}
								}
							)
						;

						Surface.f_Box(TextDestination + CTerminalPoint{-1, -1} + CTerminalSize{2, 2});

						LastDrawTime = Stopwatch.f_GetTime() - Time;

						auto fBlockFrame = [&]
							{
								if (LastRenderFuture.f_IsValid())
								{
									auto Stats = fg_Move(LastRenderFuture).f_CallSync();

									if (Stats.m_OutputTime)
										LastOutputTime = Stats.m_OutputTime;

									LastFrameSize = Stats.m_FrameSize;
									LastRenderTime = Stats.m_RenderTime;

									MaxFrameSize = fg_Max(MaxFrameSize, Stats.m_FrameSize);
								}
							}
						;

						auto NewRenderFuture = NConcurrency::fg_ConcurrentDispatch
							(
								[Stopwatch, Surface, &bFirst, &LastSurface, &LastOutputFuture]() mutable -> NConcurrency::TCFuture<CRenderStats>
								{
									(void)bFirst;
									(void)LastSurface;

									auto StartTime = Stopwatch.f_GetTime();
									CStr RenderedString;
									if constexpr (c_bDoDiffRender)
									{
										if (bFirst)
											RenderedString = Surface.f_Render(ETerminalRenderFlag::mc_None);
										else
											RenderedString = Surface.f_DiffRender(LastSurface, ETerminalRenderFlag::mc_None);

										bFirst = false;
										LastSurface = fg_Move(Surface);
									}
									else
										RenderedString = Surface.f_Render(ETerminalRenderFlag::mc_None);

									auto RenderTimeEnd = Stopwatch.f_GetTime();
									auto LastRenderTime = RenderTimeEnd - StartTime;
									auto LastFrameSize = RenderedString.f_GetLen();

									NStorage::TCOptional<fp64> OutputTime;

									auto NewOutputFuture = NConcurrency::fg_ConcurrentDispatch
										(
											[Stopwatch, RenderedString = fg_Move(RenderedString)]() mutable -> fp64
											{
												auto LastRenderStartTime = Stopwatch.f_GetTime();

												NCommandLine::fg_MalterlibConOut(RenderedString.f_GetStr(), RenderedString.f_GetLen());

												auto OutputEndTime = Stopwatch.f_GetTime();
												return OutputEndTime - LastRenderStartTime;
											}
										)
									;

									if (LastOutputFuture.f_IsValid())
										OutputTime = co_await fg_Move(LastOutputFuture);

									LastOutputFuture = fg_Move(NewOutputFuture);

									if constexpr (!c_bDoPipelining)
										OutputTime = co_await fg_Move(LastOutputFuture);

									co_return CRenderStats{.m_RenderTime = LastRenderTime, .m_FrameSize = LastFrameSize, .m_OutputTime = OutputTime};
								}
							)
						;

						fBlockFrame();
						LastRenderFuture = fg_Move(NewRenderFuture);

						if constexpr (!c_bDoPipelining)
							fBlockFrame();
						//Fpss += "{} FPS\n"_f << (fp64(1.0) / FrameTime);
/*						if (x == 0)
						{
							CStr Stats = "{}{} FPS"_f << AnsiEncoding.f_Move(0, 0) << (fp64(1.0) / FrameTime);
							DMibConOut("{}", Stats);
						}*/
					}
				}
				if (LastRenderFuture.f_IsValid())
					fg_Move(LastRenderFuture).f_CallSync();
				if (LastOutputFuture.f_IsValid())
					fg_Move(LastOutputFuture).f_CallSync();
#elif 0
				for (int32 y = Height / 10; (y < Height / 10 + 2) && !bTerminate; ++y)
				{
					MaxFrameTime = 0;
					for (int32 x = (10); x < Width && !bTerminate; ++x)
					{
						++nFrames;
						fp64 Time = Stopwatch.f_GetTime();
						fp64 FrameTime = Time - LastTime;
						LastTime = Time;

						CTerminalSize Size{Width, Height};
						CTerminalSurface Surface(AnsiEncoding, Size);

						int32 RowStart = 0;

						for
							(
								ETerminalBoxType BoxType = ETerminalBoxType::mc_NonBoxDrawing
								; BoxType <= ETerminalBoxType::mc_HeavyDotted3
								; BoxType = (ETerminalBoxType)(BoxType + 1), RowStart += 4
							)
						{
							int32 ColumnStart = 1;
							for
								(
									ETerminalBoxCoverage BoxCoverage = ETerminalBoxCoverage::mc_Full
									; BoxCoverage <= ETerminalBoxCoverage::mc_OpenRight
									; BoxCoverage = (ETerminalBoxCoverage)(BoxCoverage + 1), ColumnStart += 12
								)
							{
								CTerminalArea TextDestination{{x + ColumnStart, y + RowStart}, {11, 3}};

								CTerminalSurface::CTextOptions RenderTextOptions;
								RenderTextOptions.m_Aligment = ETerminalTextAlignment_VerticalCenter | ETerminalTextAlignment_HorizontalCenter;
								RenderTextOptions.m_DestinationArea = TextDestination;

								Surface.f_Box(TextDestination, BoxType, BoxCoverage);
								Surface.f_Text("Testing", RenderTextOptions);
							}
						}
						// ┌─────────┐ ┌─────────╴ ┌─────────┐ ╶─────────┐ ╷         ╷
						// │ Testing │ │ Testing   │ Testing │   Testing │ │ Testing │
						// └─────────┘ └─────────╴ ╵         ╵ ╶─────────┘ └─────────┘

						CTerminalArea TextDestination{{x, y}, {TextAreaWidth, TextAreaHeight}};

						CTerminalSurface::CTextOptions RenderTextOptions;
						RenderTextOptions.m_Aligment = ETerminalTextAlignment_VerticalCenter | ETerminalTextAlignment_HorizontalCenter;
						RenderTextOptions.m_DestinationArea = TextDestination;

						Surface.f_Text
							(
								"Frame {fe0,sj4} fps    Render {fe0,sj4} fps    Draw {fe0,sj4} fps"_f
								<< (fp64(1.0) / FrameTime)
								<< (fp64(1.0) / LastRenderTime)
								<< (fp64(1.0) / LastDrawTime)
								, CTerminalSurface::CTextOptions()
							)
						;

						MaxFrameTime = fg_Max(MaxFrameTime, FrameTime);

						LastDrawTime = Stopwatch.f_GetTime() - Time;

						auto RenderedString = Surface.f_Render(ETerminalRenderFlag_ResetAtEndOfLine);
						LastRenderTime = Stopwatch.f_GetTime() - Time;
						MaxFrameSize = fg_Max(MaxFrameSize, RenderedString.f_GetLen());

						NCommandLine::fg_MalterlibConOut(RenderedString.f_GetStr(), RenderedString.f_GetLen());
					}
				}
#else
				CTerminalSize Size{Width, Height};
				CTerminalSurface Surface(AnsiEncoding, Size);
				CTerminalSurface LastSurface(AnsiEncoding, Size);
				[[maybe_unused]] bool bFirst = true;
				Surface.f_Fill
					(
						CTerminalArea{.m_UpperLeft = CTerminalPoint{.m_CoordX = 0, .m_CoordY = 0}, .m_Size = Size}
						, {nullptr, {CTerminalColor::fs_FromRGBA(0, 0, 128), nullptr, nullptr}}
					)
				;

				int32 y = 0;
				fp64 Time = Stopwatch.f_GetTime();
				fp64 FrameTime = Time - LastTime;
				LastTime = Time;
#if 0
				Surface.f_Box
					(
						CTerminalArea{{50, 50}, {11, 6}}
						,
						{
							.m_Type = ETerminalBoxType::mc_Light
							, .m_bMergeBoxes = true
							, .m_BoxCoverage = ETerminalBoxCoverage::mc_OpenLeft
						}
					)
				;
				Surface.f_Box
					(
						CTerminalArea{{50, 45}, {11, 6}}
						,
						{
							.m_Type = ETerminalBoxType::mc_Heavy
							, .m_bMergeBoxes = true
							, .m_BoxCoverage = ETerminalBoxCoverage::mc_OpenBottom
						}
					)
				;
#elif 0
				//for (int32 y = Height / 10; (y < Height / 10 + 2) && !bTerminate; ++y)
				{
					MaxFrameTime = 0;
					int32 x = 150;
					for (int32 i = 0; (i < 15) && !bTerminate; ++i)
					{
						++nFrames;

						int32 RowStart = 102 - 6;

						NContainer::TCMap<CStr, ETerminalBoxType> RandomDrawOrder;

						for
							(
								ETerminalBoxType BoxType = ETerminalBoxType::mc_NonBoxDrawing
								; BoxType <= ETerminalBoxType::mc_HeavyDotted3
								; BoxType = ETerminalBoxType((int)BoxType + 1)
							)
						{
							RandomDrawOrder[NCryptography::fg_RandomID()] = BoxType;
						}

						//ETerminalBoxType Types[] = {ETerminalBoxType::mc_NonBoxDrawing, ETerminalBoxType::mc_NonBoxDrawingOuter, ETerminalBoxType::mc_NonBoxDrawingRounded};
						//ETerminalBoxType Types[] = {ETerminalBoxType::mc_Double, ETerminalBoxType::mc_Light};
						ETerminalBoxType Types[] = {ETerminalBoxType::mc_Heavy, ETerminalBoxType::mc_Double, ETerminalBoxType::mc_Light};

/*						ETerminalBoxType Types[] =
							{
								ETerminalBoxType::mc_Light
								, ETerminalBoxType::mc_LightRounded
								, ETerminalBoxType::mc_LightDotted1
								, ETerminalBoxType::mc_LightDotted2
								, ETerminalBoxType::mc_LightDotted3
								, ETerminalBoxType::mc_LightDotted1Rounded
								, ETerminalBoxType::mc_LightDotted2Rounded
								, ETerminalBoxType::mc_LightDotted3Rounded
							}
						;*/
						//ETerminalBoxType Types[] = {ETerminalBoxType::mc_Heavy, ETerminalBoxType::mc_HeavyDotted1, ETerminalBoxType::mc_HeavyDotted2, ETerminalBoxType::mc_HeavyDotted3};

						//ETerminalBoxType Types[] = {ETerminalBoxType::mc_Light, ETerminalBoxType::mc_Double};

		/*				ETerminalBoxType Types[] =
							{
								ETerminalBoxType::mc_Light
								, ETerminalBoxType::mc_LightRounded
								, ETerminalBoxType::mc_Heavy
								, ETerminalBoxType::mc_Double
								, ETerminalBoxType::mc_LightDotted1
								, ETerminalBoxType::mc_LightDotted2
								, ETerminalBoxType::mc_LightDotted3
								, ETerminalBoxType::mc_LightDotted1Rounded
								, ETerminalBoxType::mc_LightDotted2Rounded
								, ETerminalBoxType::mc_LightDotted3Rounded
								, ETerminalBoxType::mc_HeavyDotted1
								, ETerminalBoxType::mc_HeavyDotted2
								, ETerminalBoxType::mc_HeavyDotted3
							}
						;*/

						for
							(
								ETerminalBoxType BoxType = ETerminalBoxType::mc_NonBoxDrawing
								; BoxType <= ETerminalBoxType::mc_HeavyDotted3 && !bTerminate
								; BoxType = (ETerminalBoxType)((int)BoxType + 1)
							)
						//for (ETerminalBoxType BoxType : RandomDrawOrder)
						{
							Time = Stopwatch.f_GetTime();
							FrameTime = Time - LastTime;
							LastTime = Time;

							int32 ColumnStart = 0;

							CTerminalArea TextDestination{{x + ColumnStart + (int32(NMisc::fg_GetRandomUnsigned() % 4) - 2), (y + RowStart) % Height}, {11, 6}};

							CTextOptions RenderTextOptions;
							RenderTextOptions.m_Aligment = ETerminalTextAlignment::mc_VerticalCenter | ETerminalTextAlignment::mc_HorizontalCenter;
							RenderTextOptions.m_DestinationArea = TextDestination;

							Surface.f_Box
								(
									TextDestination
									,
									{
										.m_Type = Types[NMisc::fg_GetRandomUnsigned() % (sizeof(Types) / sizeof(Types[0]))]
										, .m_bMergeBoxes = true
										, .m_BoxCoverage
										= ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 0)
										| ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 1)
										| ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 2)
										| ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 3)
									}
								)
							;
							//Surface.f_Text("Testing", RenderTextOptions);

							{
								CTerminalArea TextDestination{{0, 0}, CTerminalSize{Width, Height}};

								CTextOptions RenderTextOptions;
								RenderTextOptions.m_DestinationArea = TextDestination;

								Surface.f_Text
									(
										"Frame {fe3,sj7} ms ({fe0,sj4} fps)    Draw {fe3,sj7} ms    Render {fe3,sj7} ms    Output {fe3,sj7} ms    FrameSize {ns }"_f
										<< (FrameTime * 1000.0)
										<< (fp64(1.0) / FrameTime)
										<< (LastDrawTime * 1000.0)
										<< (LastRenderTime * 1000.0)
										<< LastOutputTime ? (*LastOutputTime * 1000.0) : fp64::fs_QNan();
										<< LastFrameSize
										, RenderTextOptions
									)
								;

								MaxFrameTime = fg_Max(MaxFrameTime, FrameTime);
								auto DrawTimeEnd = Stopwatch.f_GetTime();
								LastDrawTime = DrawTimeEnd - Time;

								auto RenderedString = Surface.f_Render(ETerminalRenderFlag::mc_ResetAtEndOfLine);

								auto RenderTimeEnd = Stopwatch.f_GetTime();
								LastRenderTime = RenderTimeEnd - DrawTimeEnd;
								LastFrameSize = RenderedString.f_GetLen();
								MaxFrameSize = fg_Max(MaxFrameSize, RenderedString.f_GetLen());

								NCommandLine::fg_MalterlibConOut(RenderedString.f_GetStr(), RenderedString.f_GetLen());

								auto OutputEndTime = Stopwatch.f_GetTime();

								LastOutputTime = OutputEndTime - RenderTimeEnd;
								//NSys::fg_Thread_Sleep(1.0/120.0);
							}

							//RowStart += 5;
							RowStart -= 6 + (int32(NMisc::fg_GetRandomUnsigned() % 4) - 2);
							//+ (int32(NMisc::fg_GetRandomUnsigned() % 2) - 1);
						}
						// ┌─────────┐
						// │ Testing │
						// │ Testing │
						// │ Testing │
						// │ Testing ┌─────────┐
						// └─────────│ Testing │
						//           │ Testing │
						//           │ Testing │
						//           │ Testing │
						//           └─────────┘
						//y += NMisc::fg_GetRandomUnsigned() % 6;
						//x += 10 + (int32(NMisc::fg_GetRandomUnsigned() % 2) - 1);
						if (i == 0)
							y += 5;
						else
							y += 6;
						x -= 10;
					}
				}
#else
				//for (int32 y = Height / 10; (y < Height / 10 + 2) && !bTerminate; ++y)
				{
					// Overflow bit

					MaxFrameTime = 0;
					//while (!bTerminate)
					for (umint i = 0; i < 10000; ++i)
					{
						ETerminalBoxType Types[] =
							{
								ETerminalBoxType::mc_Light
								, ETerminalBoxType::mc_LightRounded
								, ETerminalBoxType::mc_Heavy
								, ETerminalBoxType::mc_Double
								, ETerminalBoxType::mc_LightDotted1
								, ETerminalBoxType::mc_LightDotted2
								, ETerminalBoxType::mc_LightDotted3
								, ETerminalBoxType::mc_LightDotted1Rounded
								, ETerminalBoxType::mc_LightDotted2Rounded
								, ETerminalBoxType::mc_LightDotted3Rounded
								, ETerminalBoxType::mc_HeavyDotted1
								, ETerminalBoxType::mc_HeavyDotted2
								, ETerminalBoxType::mc_HeavyDotted3
							}
						;

						Time = Stopwatch.f_GetTime();
						FrameTime = Time - LastTime;
						LastTime = Time;
						FrameTimes.f_Insert(FrameTime);
						FrameTimeQueue.push_back(FrameTime);
						if (FrameTimeQueue.size() > 10)
							FrameTimeQueue.pop_front();

						fp64 AverageFrameTime = 0.0;
						for (auto &FrameTime : FrameTimeQueue)
							AverageFrameTime += FrameTime;

						AverageFrameTime = AverageFrameTime / FrameTimeQueue.size();

						if (LastOutputTime)
						{
							OutputTimes.f_Insert(*LastOutputTime);
							OutputTimeQueue.push_back(*LastOutputTime);
						}

						if (OutputTimeQueue.size() > 10)
							OutputTimeQueue.pop_front();

						fp64 AverageOutputTime = 0.0;
						for (auto &OutputTime : OutputTimeQueue)
							AverageOutputTime += OutputTime;

						AverageOutputTime = AverageOutputTime / OutputTimeQueue.size();

						++nFrames;

						Surface.f_Fill
							(
								CTerminalArea{.m_UpperLeft = CTerminalPoint{.m_CoordX = 0, .m_CoordY = 0}, .m_Size = Size}
								, CTerminalSurfaceFillProperties{.m_Character = ' ', .m_Properties = {CTerminalColor::fs_FromRGBA(0, 0, 128), nullptr, nullptr}}
							)
						;

						for (umint i = 0; i < 100; ++i)
						{
							int32 BoxWidth = NMisc::fg_GetRandomUnsigned() % 200;
							int32 BoxHeight = NMisc::fg_GetRandomUnsigned() % 200;
							int32 BoxX = NMisc::fg_GetRandomUnsigned() % (Width - BoxWidth);
							int32 BoxY = NMisc::fg_GetRandomUnsigned() % (Height - BoxHeight);

							CTerminalArea TextDestination{BoxX, BoxY, {BoxWidth, BoxHeight}};

							CTextOptions RenderTextOptions;
							RenderTextOptions.m_Aligment = ETerminalTextAlignment::mc_VerticalCenter | ETerminalTextAlignment::mc_HorizontalCenter;
							RenderTextOptions.m_DestinationArea = TextDestination;

							Surface.f_Box
								(
									TextDestination
									,
									{
										.m_Type = Types[NMisc::fg_GetRandomUnsigned() % (sizeof(Types) / sizeof(Types[0]))]
										, .m_BoxCoverage
										= ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 0)
										| ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 1)
										| ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 2)
										| ETerminalBoxCoverage((NMisc::fg_GetRandomUnsigned() % 2) << 3)
										, .m_bMergeBoxes = true
									}
								)
							;
						}

						{
							CTerminalArea TextDestination{{0, 0}, CTerminalSize{Width, Height}};

							CTextOptions RenderTextOptions;
							RenderTextOptions.m_DestinationArea = TextDestination;

							Surface.f_Text
								(
									"Frame {fe3,sj7} ms ({fe0,sj6} fps)    Draw {fe3,sj7} ms    Render {fe3,sj7} ms    Output {fe3,sj7} ms    FrameSize {ns ,sj12}"_f
									<< (AverageFrameTime * 1000.0)
									<< (fp64(1.0) / AverageFrameTime)
									<< (LastDrawTime * 1000.0)
									<< (LastRenderTime * 1000.0)
									<< (AverageOutputTime * 1000.0)
									<< LastFrameSize
									, RenderTextOptions
								)
							;

							MaxFrameTime = fg_Max(MaxFrameTime, FrameTime);
							auto DrawTimeEnd = Stopwatch.f_GetTime();
							LastDrawTime = DrawTimeEnd - Time;

							auto fBlockFrame = [&]
								{
									if (LastRenderFuture.f_IsValid())
									{
										auto Stats = fg_Move(LastRenderFuture).f_CallSync();

										if (Stats.m_OutputTime)
											LastOutputTime = Stats.m_OutputTime;

										LastFrameSize = Stats.m_FrameSize;
										LastRenderTime = Stats.m_RenderTime;

										MaxFrameSize = fg_Max(MaxFrameSize, Stats.m_FrameSize);
									}
								}
							;

							auto NewRenderFuture = NConcurrency::fg_ConcurrentDispatch
								(
									[Stopwatch, Surface, &bFirst, &LastSurface, &LastOutputFuture]() mutable -> NConcurrency::TCFuture<CRenderStats>
									{
										(void)bFirst;
										(void)LastSurface;

										auto StartTime = Stopwatch.f_GetTime();
										CStr RenderedString;
										if constexpr (c_bDoDiffRender)
										{
											if (bFirst)
												RenderedString = Surface.f_Render(ETerminalRenderFlag::mc_None);
											else
												RenderedString = Surface.f_DiffRender(LastSurface, ETerminalRenderFlag::mc_None);

											bFirst = false;
											LastSurface = fg_Move(Surface);
										}
										else
											RenderedString = Surface.f_Render(ETerminalRenderFlag::mc_None);

										auto RenderTimeEnd = Stopwatch.f_GetTime();
										auto LastRenderTime = RenderTimeEnd - StartTime;
										auto LastFrameSize = RenderedString.f_GetLen();

										NStorage::TCOptional<fp64> OutputTime;

										auto NewOutputFuture = NConcurrency::fg_ConcurrentDispatch
											(
												[Stopwatch, RenderedString = fg_Move(RenderedString)]() mutable -> fp64
												{
													auto LastRenderStartTime = Stopwatch.f_GetTime();

													NCommandLine::fg_MalterlibConOut(RenderedString.f_GetStr(), RenderedString.f_GetLen());

													auto OutputEndTime = Stopwatch.f_GetTime();
													return OutputEndTime - LastRenderStartTime;
												}
											)
										;

										if (LastOutputFuture.f_IsValid())
											OutputTime = co_await fg_Move(LastOutputFuture);

										LastOutputFuture = fg_Move(NewOutputFuture);

										if constexpr (!c_bDoPipelining)
											OutputTime = co_await fg_Move(LastOutputFuture);

										co_return CRenderStats{.m_RenderTime = LastRenderTime, .m_FrameSize = LastFrameSize, .m_OutputTime = OutputTime};
									}
								)
							;

							fBlockFrame();

							LastRenderFuture = fg_Move(NewRenderFuture);

							if constexpr (!c_bDoPipelining)
								fBlockFrame();

/*							while (true)
							{
								auto Time = Stopwatch.f_GetTime();
								if (Time - LastRenderStartTime >= 1.0 / 121.0)
									break;

							}*/

						}
					}
				}
#endif
				if (LastRenderFuture.f_IsValid())
					fg_Move(LastRenderFuture).f_CallSync();
				if (LastOutputFuture.f_IsValid())
					fg_Move(LastOutputFuture).f_CallSync();

				MaxFrameTime = fg_Max(MaxFrameTime, FrameTime);
				LastDrawTime = Stopwatch.f_GetTime() - Time;

				auto RenderedString = Surface.f_Render(ETerminalRenderFlag::mc_None);
				LastRenderTime = Stopwatch.f_GetTime() - Time;
				MaxFrameSize = fg_Max(MaxFrameSize, RenderedString.f_GetLen());

				NCommandLine::fg_MalterlibConOut(RenderedString.f_GetStr(), RenderedString.f_GetLen());
 #endif

				//NMib::NSys::fg_Thread_Sleep(10.0);
				fp64 AverageFPS = (fp64(1.0) / (Stopwatch.f_GetTime() / nFrames));
				CStr Cleanup = "{}{}{}"_f << AnsiEncoding.f_ShowCursor(true) << AnsiEncoding.f_Default() << AnsiEncoding.f_EnableAlternativeScreenBuffer(false);
				DMibConOut("{}\n", Cleanup);
				DMibConOut("Average fps        : {fe1}\n{}", AverageFPS, Fpss);
				DMibConOut("Min fps            : {fe1}\n{}", fp64(1.0) / MaxFrameTime, Fpss);

				{
					fp64 Average = 0.0;
					for (auto FrameTime : FrameTimes)
						Average += FrameTime;

					Average /= nFrames;

					fp64 StdDev = 0.0;
					for (auto FrameTime : FrameTimes)
						StdDev += (FrameTime - Average) * (FrameTime - Average);

					StdDev /= nFrames;
					StdDev = StdDev.f_Sqrt();
					DMibConOut("Average frame time : {fe2} ms\n", Average * 1000.0);
					DMibConOut("StdDev frame time  : {fe2} ms\n", StdDev * 1000.0);
				}
				{
					fp64 Average = 0.0;
					for (auto FrameTime : OutputTimes)
						Average += FrameTime;

					Average /= nFrames;

					fp64 StdDev = 0.0;
					for (auto FrameTime : OutputTimes)
						StdDev += (FrameTime - Average) * (FrameTime - Average);

					StdDev /= nFrames;
					StdDev = StdDev.f_Sqrt();
					DMibConOut("Average output time: {fe2} ms\n", Average * 1000.0);
					DMibConOut("StdDev output time : {fe2} ms\n", StdDev * 1000.0);
				}

/*				for (auto FrameTime : OutputTimes)
					FrameTime *= 1000.0;
				DMibConOut("OutputTimes: {}\n", OutputTimes);*/
			};
		}
	};
	DMibTestRegister(CTerminal_Tests, Malterlib::CommandLine);
}
