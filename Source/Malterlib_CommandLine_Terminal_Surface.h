// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	enum class ETerminalTextRenderFlag : uint32
	{
		mc_None
		, mc_StripColor = DMibBit(0)
	};

	enum class ETerminalTextAlignment : uint32
	{
		mc_None = 0
		, mc_HorizontalCenter = DMibBit(0)
		, mc_VerticalCenter = DMibBit(1)
	};

	enum class ETerminalRenderFlag : uint32
	{
		mc_None
		, mc_ResetAtEndOfLine = DMibBit(0)
		, mc_NoSynchronizedUpdate = DMibBit(1) // Do not wrap the output in the synchronized update protocol
	};

	struct CTerminalSurfaceClearProperties
	{
		ch32 m_ClearCharacter = ' ';
		CSetChixelProperties m_ClearProperties = {};
	};

	struct CTerminalSurfaceFillProperties
	{
		NStorage::TCOptional<ch32> m_Character;
		CSetChixelProperties m_Properties = {};
	};

	enum class ETerminalBoxType : uint32
	{
		mc_NonBoxDrawing			// ----------- ----------- ----------- ----------- |         |
									// | Testing | | Testing   | Testing |   Testing | | Testing |
									// ----------- ----------- |         | ----------- -----------

		, mc_NonBoxDrawingOuter		// |¯¯¯¯¯¯¯¯¯| |¯¯¯¯¯¯¯¯¯¯ |¯¯¯¯¯¯¯¯¯| ¯¯¯¯¯¯¯¯¯¯| |         |
									// | Testing | | Testing   | Testing |   Testing | | Testing |
									// |_________| |__________ |         | __________| |_________|

		, mc_NonBoxDrawingRounded	// /¯¯¯¯¯¯¯¯¯\ /¯¯¯¯¯¯¯¯¯¯ /¯¯¯¯¯¯¯¯¯\ ¯¯¯¯¯¯¯¯¯¯\ |         |
									// | Testing | | Testing   | Testing |   Testing | | Testing |
									// \_________/ \__________ |         | __________/ \_________/

		, mc_Light 					// ┌─────────┐ ┌─────────╴ ┌─────────┐ ╶─────────┐ ╷         ╷
									// │ Testing │ │ Testing   │ Testing │   Testing │ │ Testing │
									// └─────────┘ └─────────╴ ╵         ╵ ╶─────────┘ └─────────┘

		, mc_LightRounded			// ╭─────────╮ ┌─────────╴ ┌─────────┐ ╶─────────┐ ╷         ╷
									// │ Testing │ │ Testing   │ Testing │   Testing │ │ Testing │
									// ╰─────────╯ └─────────╴ ╵         ╵ ╶─────────┘ └─────────┘

		, mc_Heavy 					// ┏━━━━━━━━━┓ ┏━━━━━━━━━╸ ┏━━━━━━━━━┓ ╺━━━━━━━━━┓ ╻         ╻
									// ┃ Testing ┃ ┃ Testing   ┃ Testing ┃   Testing ┃ ┃ Testing ┃
									// ┗━━━━━━━━━┛ ┗━━━━━━━━━╸ ╹         ╹ ╺━━━━━━━━━┛ ┗━━━━━━━━━┛

		, mc_InnerHalf				// ▗▄▄▄▄▄▄▄▄▄▖ ▗▄▄▄▄▄▄▄▄▄▖ ▗▄▄▄▄▄▄▄▄▄▖ ▗▄▄▄▄▄▄▄▄▄▖ ▗         ▖
									// ▐ Testing ▌ ▐ Testing   ▐ Testing ▌   Testing ▌ ▐ Testing ▌
									// ▝▀▀▀▀▀▀▀▀▀▘ ▝▀▀▀▀▀▀▀▀▀▘ ▝         ▘ ▝▀▀▀▀▀▀▀▀▀▘ ▝▀▀▀▀▀▀▀▀▀▘

		, mc_OuterHalf				// ▛▀▀▀▀▀▀▀▀▀▜ ▛▀▀▀▀▀▀▀▀▀▀ ▛▀▀▀▀▀▀▀▀▀▜ ▀▀▀▀▀▀▀▀▀▀▜ ▌         ▐
									// ▌ Testing ▐ ▌ Testing   ▌ Testing ▐   Testing ▐ ▌ Testing ▐
									// ▙▄▄▄▄▄▄▄▄▄▟ ▙▄▄▄▄▄▄▄▄▄▄ ▌         ▐ ▄▄▄▄▄▄▄▄▄▄▟ ▙▄▄▄▄▄▄▄▄▄▟

		, mc_OuterHalfFullVertical	// █▀▀▀▀▀▀▀▀▀█ █▀▀▀▀▀▀▀▀▀▀ █▀▀▀▀▀▀▀▀▀█ ▀▀▀▀▀▀▀▀▀▀█ █         █
									// █ Testing █ █ Testing   █ Testing █   Testing █ █ Testing █
									// █▄▄▄▄▄▄▄▄▄█ █▄▄▄▄▄▄▄▄▄▄ █         █ ▄▄▄▄▄▄▄▄▄▄█ █▄▄▄▄▄▄▄▄▄█

		, mc_FullHeavy				// ███████████ ███████████ ███████████ ███████████ █         █
									// █ Testing █ █ Testing   █ Testing █   Testing █ █ Testing █
									// ███████████ ███████████ █         █ ███████████ ███████████

		, mc_Double 				// ╔═════════╗ ╔═════════╸ ╔═════════╗ ╺═════════╗ ╻         ╻
									// ║ Testing ║ ║ Testing   ║ Testing ║   Testing ║ ║ Testing ║
									// ╚═════════╝ ╚═════════╸ ╹         ╹ ╺═════════╝ ╚═════════╝

		, mc_LightDotted1			// ┌┈┈┈┈┈┈┈┈┈┐ ┌┈┈┈┈┈┈┈┈┈╴ ┌┈┈┈┈┈┈┈┈┈┐ ╶┈┈┈┈┈┈┈┈┈┐ ╷         ╷
									// ┊ Testing ┊ ┊ Testing   ┊ Testing ┊   Testing ┊ ┊ Testing ┊
									// └┈┈┈┈┈┈┈┈┈┘ └┈┈┈┈┈┈┈┈┈╴ ╵         ╵ ╶┈┈┈┈┈┈┈┈┈┘ └┈┈┈┈┈┈┈┈┈┘

		, mc_LightDotted2			// ┌┄┄┄┄┄┄┄┄┄┐ ┌┄┄┄┄┄┄┄┄┄╴ ┌┄┄┄┄┄┄┄┄┄┐ ╶┄┄┄┄┄┄┄┄┄┐ ╷         ╷
									// ┆ Testing ┆ ┆ Testing   ┆ Testing ┆   Testing ┆ ┆ Testing ┆
									// └┄┄┄┄┄┄┄┄┄┘ └┄┄┄┄┄┄┄┄┄╴ ╵         ╵ ╶┄┄┄┄┄┄┄┄┄┘ └┄┄┄┄┄┄┄┄┄┘

		, mc_LightDotted3			// ┌╌╌╌╌╌╌╌╌╌┐ ┌╌╌╌╌╌╌╌╌╌╴ ┌╌╌╌╌╌╌╌╌╌┐ ╶╌╌╌╌╌╌╌╌╌┐ ╷         ╷
									// ╎ Testing ╎ ╎ Testing   ╎ Testing ╎   Testing ╎ ╎ Testing ╎
									// └╌╌╌╌╌╌╌╌╌┘ └╌╌╌╌╌╌╌╌╌╴ ╵         ╵ ╶╌╌╌╌╌╌╌╌╌┘ └╌╌╌╌╌╌╌╌╌┘

		, mc_LightDotted1Rounded	// ╭┈┈┈┈┈┈┈┈┈╮ ╭┈┈┈┈┈┈┈┈┈╴ ╭┈┈┈┈┈┈┈┈┈╮ ╶┈┈┈┈┈┈┈┈┈╮ ╷         ╷
									// ┊ Testing ┊ ┊ Testing   ┊ Testing ┊   Testing ┊ ┊ Testing ┊
									// ╰┈┈┈┈┈┈┈┈┈╯ ╰┈┈┈┈┈┈┈┈┈╴ ╵         ╵ ╶┈┈┈┈┈┈┈┈┈╯ ╰┈┈┈┈┈┈┈┈┈╯

		, mc_LightDotted2Rounded	// ╭┄┄┄┄┄┄┄┄┄╮ ╭┄┄┄┄┄┄┄┄┄╴ ╭┄┄┄┄┄┄┄┄┄╮ ╶┄┄┄┄┄┄┄┄┄╮ ╷         ╷
									// ┆ Testing ┆ ┆ Testing   ┆ Testing ┆   Testing ┆ ┆ Testing ┆
									// ╰┄┄┄┄┄┄┄┄┄╯ ╰┄┄┄┄┄┄┄┄┄╴ ╵         ╵ ╶┄┄┄┄┄┄┄┄┄╯ ╰┄┄┄┄┄┄┄┄┄╯

		, mc_LightDotted3Rounded	// ╭╌╌╌╌╌╌╌╌╌╮ ╭╌╌╌╌╌╌╌╌╌╴ ╭╌╌╌╌╌╌╌╌╌╮ ╶╌╌╌╌╌╌╌╌╌╮ ╷         ╷
									// ╎ Testing ╎ ╎ Testing   ╎ Testing ╎   Testing ╎ ╎ Testing ╎
									// ╰╌╌╌╌╌╌╌╌╌╯ ╰╌╌╌╌╌╌╌╌╌╴ ╵         ╵ ╶╌╌╌╌╌╌╌╌╌╯ ╰╌╌╌╌╌╌╌╌╌╯

		, mc_HeavyDotted1			// ┏┉┉┉┉┉┉┉┉┉┓ ┏┉┉┉┉┉┉┉┉┉╸ ┏┉┉┉┉┉┉┉┉┉┓ ╺┉┉┉┉┉┉┉┉┉┓ ╻         ╻
									// ┋ Testing ┋ ┋ Testing   ┋ Testing ┋   Testing ┋ ┋ Testing ┋
									// ┗┉┉┉┉┉┉┉┉┉┛ ┗┉┉┉┉┉┉┉┉┉╸ ╹         ╹ ╺┉┉┉┉┉┉┉┉┉┛ ┗┉┉┉┉┉┉┉┉┉┛

		, mc_HeavyDotted2			// ┏┅┅┅┅┅┅┅┅┅┓ ┏┅┅┅┅┅┅┅┅┅╸ ┏┅┅┅┅┅┅┅┅┅┓ ╺┅┅┅┅┅┅┅┅┅┓ ╻         ╻
									// ┇ Testing ┇ ┇ Testing   ┇ Testing ┇   Testing ┇ ┇ Testing ┇
									// ┗┅┅┅┅┅┅┅┅┅┛ ┗┅┅┅┅┅┅┅┅┅╸ ╹         ╹ ╺┅┅┅┅┅┅┅┅┅┛ ┗┅┅┅┅┅┅┅┅┅┛

		, mc_HeavyDotted3			// ┏╍╍╍╍╍╍╍╍╍┓ ┏╍╍╍╍╍╍╍╍╍╸ ┏╍╍╍╍╍╍╍╍╍┓ ╺╍╍╍╍╍╍╍╍╍┓ ╻         ╻
									// ╏ Testing ╏ ╏ Testing   ╏ Testing ╏   Testing ╏ ╏ Testing ╏
									// ┗╍╍╍╍╍╍╍╍╍┛ ┗╍╍╍╍╍╍╍╍╍╸ ╹         ╹ ╺╍╍╍╍╍╍╍╍╍┛ ┗╍╍╍╍╍╍╍╍╍┛
	};

	enum class ETerminalBoxCoverage
	{
		mc_None = 0

		, mc_Left = DMibBit(0)								// ╷
															// │ Testing
															// ╵

		, mc_Right = DMibBit(1)								//           ╷
															//   Testing │
															//           ╵

		, mc_Top = DMibBit(2)								// ╶─────────╴
															//   Testing
															//

		, mc_Bottom = DMibBit(3)							//
															//   Testing
															// ╶─────────╴

		, mc_Full = mc_Left | mc_Right | mc_Top | mc_Bottom	// ┌─────────┐
															// │ Testing │
															// └─────────┘

		, mc_OpenTop  = mc_Left | mc_Right  | mc_Bottom		// ╷         ╷
															// │ Testing │
															// └─────────┘

		, mc_OpenBottom = mc_Left | mc_Right | mc_Top		// ┌─────────┐
															// │ Testing │
															// ╵         ╵

		, mc_OpenLeft = mc_Right | mc_Top | mc_Bottom	 	// ╶─────────┐
															//   Testing │
															// ╶─────────┘

		, mc_OpenRight = mc_Left | mc_Top | mc_Bottom	 	// ┌─────────╴
															// │ Testing
															// └─────────╴

		, mc_LeftAndBottom = mc_Left | mc_Bottom			// ╷
															// │ Testing
															// └─────────╴

		, mc_RightAndBottom = mc_Right | mc_Bottom			//           ╷
															//   Testing │
															// ╶─────────┘

		, mc_LeftAndTop = mc_Left | mc_Top 					// ┌─────────╴
															// │ Testing
															// ╵

		, mc_RightAndTop = mc_Right | mc_Top				// ╶─────────┐
															//   Testing │
															//           ╵

		, mc_LeftAndRight = mc_Left | mc_Right 				// ╷         ╷
															// │ Testing │
															// ╵         ╵

		, mc_TopAndBottom = mc_Top | mc_Bottom				// ╶─────────╴
															//   Testing
															// ╶─────────╴
	};

	struct CBlendOptions
	{
		uint8 m_ConstantOpacity = 255;
	};

	struct CTextOptions
	{
		CTerminalArea m_DestinationArea;
		ETerminalTextRenderFlag m_Flags = ETerminalTextRenderFlag::mc_None;
		ETerminalTextAlignment m_Aligment = ETerminalTextAlignment::mc_None;
		CAnsiEncoding::EWordWrap m_WordWrap = CAnsiEncoding::EWordWrap_None;
		CSetChixelProperties m_Properties;
	};

	struct CRenderBoxOptions
	{
		ETerminalBoxType m_Type = ETerminalBoxType::mc_Light;
		ETerminalBoxCoverage m_BoxCoverage = ETerminalBoxCoverage::mc_Full;
		bool m_bMergeBoxes = false;
	};

	struct CTerminalSurface
	{
		CTerminalSurface(CAnsiEncoding const &_AnsiEncoding, CTerminalSize const &_Size, CTerminalSurfaceClearProperties const &_DefaultProperties = {});
		CTerminalSurface(CTerminalSurface const &) = default;
		CTerminalSurface(CTerminalSurface &&) = default;
		CTerminalSurface &operator = (CTerminalSurface const &) = default;
		CTerminalSurface &operator = (CTerminalSurface &&) = default;

		//void f_Blend(CTerminalSurface const &_Source, CTerminalArea const &_SourceArea = {}, CTerminalPoint const &_Destination = {}, CBlendOptions);
		void f_Clear(CTerminalSurfaceClearProperties const &_ClearProperties = {});
		void f_Blit(CTerminalSurface const &_Source, CTerminalArea const &_SourceArea = {}, CTerminalPoint const &_Destination = {});

		// Blit that keeps destination cells where the source is transparent: cells without glyph and
		// with fully transparent colors are skipped, and transparent color channels of copied cells
		// keep the destination value
		void f_BlitMerge(CTerminalSurface const &_Source, CTerminalArea const &_SourceArea = {}, CTerminalPoint const &_Destination = {});
		void f_Box(CTerminalArea const &_Area, CRenderBoxOptions const &_Options = {});
		CTerminalArea f_Text(NStr::CStr const &_String, CTextOptions const &_Options);
		void f_Fill(CTerminalArea const &_Area, CTerminalSurfaceFillProperties const &_Properties);
		void f_GradientHorizontal(CTerminalArea const &_Area, CTerminalSurfaceFillProperties const &_PropertiesStart, CTerminalSurfaceFillProperties const &_PropertiesEnd);
		void f_GradientVertical(CTerminalArea const &_Area, CTerminalSurfaceFillProperties const &_PropertiesStart, CTerminalSurfaceFillProperties const &_PropertiesEnd);
		void f_Gradient
			(
				CTerminalArea const &_Area
				, CTerminalSurfaceFillProperties const &_UpperLeft
				, CTerminalSurfaceFillProperties const &_UpperRight
				, CTerminalSurfaceFillProperties const &_LowerLeft
				, CTerminalSurfaceFillProperties const &_LowerRight
			)
		;

		// Writes one pre-broken run of plain text (no escape sequences, no line breaking) starting
		// at _Position, clipped to _ClipArea (the whole surface when invalid); combining characters
		// land in the overflow of the preceding cell. Returns the column after the last cell.
		// _Text is a borrowed view that is only read during the call.
		int32 f_TextRun
			(
				CTerminalPoint _Position
				, NStr::CStrPtr const &_Text
				, CSetChixelProperties const &_Properties
				, CTerminalArea const &_ClipArea = {}
			)
		;

		NStr::CStr f_Render(ETerminalRenderFlag _Flags);
		NStr::CStr f_DiffRender(CTerminalSurface const &_Previous, ETerminalRenderFlag _Flags);

		CTerminalSize const &f_Size() const;
		CTerminalArea f_Area() const;
		CChixel const &f_GetChixel(CTerminalPoint const &_Point) const;

#if DCommandLineSupportOverflow
		// The combining-character overflow of the cell, or nullptr when the cell has none
		NStr::CStr const *f_GetOverflow(CTerminalPoint const &_Point) const;
#endif

	private:
#if DCommandLineSupportOverflow
		// Only valid for cells whose chixel has the overflow flag set; entries for overwritten
		// cells linger until the next f_Clear but are unreachable through the flag
		NStr::CStr const *fp_FindOverflow(umint _iCell) const;
#endif

		NContainer::TCVector<CChixel> mp_Buffer;
		CTerminalSize mp_Size;
		CAnsiEncoding mp_AnsiEncoding;
		CChixelProperties mp_DefaultProperties;
		NContainer::TCVector<CAnsiEncoding::CLine> mp_TextLines; // Line break scratch reused by f_Text
#if DCommandLineSupportOverflow
		NContainer::TCMap<umint, NStr::CStr> mp_OverflowStrings; // Overflow codepoints keyed by cell index
#endif
	};
}

#include "Malterlib_CommandLine_Terminal_Surface.hpp"
