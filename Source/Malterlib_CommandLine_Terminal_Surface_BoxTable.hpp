// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

namespace NMib::NCommandLine
{
	namespace
	{
		struct CTerminalBoxCharaters
		{
			ch32 m_TopLeft = 0;
			ch32 m_TopRight = 0;
			ch32 m_BottomLeft = 0;
			ch32 m_BottomRight = 0;
			ch32 m_HorizontalTop = 0;
			ch32 m_HorizontalBottom = 0;
			ch32 m_VerticalLeft = 0;
			ch32 m_VerticalRight = 0;
			ch32 m_HorizontalTopLeftOpen = 0;
			ch32 m_HorizontalTopRightOpen = 0;
			ch32 m_HorizontalBottomLeftOpen = 0;
			ch32 m_HorizontalBottomRightOpen = 0;
			ch32 m_VerticalTopLeftOpen = 0;
			ch32 m_VerticalTopRightOpen = 0;
			ch32 m_VerticalBottomLeftOpen = 0;
			ch32 m_VerticalBottomRightOpen = 0;
		};

		static constexpr CTerminalBoxCharaters gc_TerminalBoxCharacters[] =
			{
				{	// ETerminalBoxType::mc_NonBoxDrawing
					.m_TopLeft = str_utf32('-')			// ----------- ----------- ----------- ----------- |         |
					, .m_TopRight = str_utf32('-')		// | Testing | | Testing   | Testing |   Testing | | Testing |
					, .m_BottomLeft = str_utf32('-')	// ----------- ----------- |         | ----------- -----------
					, .m_BottomRight = str_utf32('-')
					, .m_HorizontalTop = str_utf32('-')
					, .m_HorizontalBottom = str_utf32('-')
					, .m_VerticalLeft = str_utf32('|')
					, .m_VerticalRight = str_utf32('|')
					, .m_HorizontalTopLeftOpen = str_utf32('-')
					, .m_HorizontalTopRightOpen = str_utf32('-')
					, .m_HorizontalBottomLeftOpen = str_utf32('-')
					, .m_HorizontalBottomRightOpen = str_utf32('-')
					, .m_VerticalTopLeftOpen = str_utf32('|')
					, .m_VerticalTopRightOpen = str_utf32('|')
					, .m_VerticalBottomLeftOpen = str_utf32('|')
					, .m_VerticalBottomRightOpen = str_utf32('|')
				}
				,
				{	// ETerminalBoxType::mc_NonBoxDrawingOuter
					.m_TopLeft = str_utf32('|')			// |¯¯¯¯¯¯¯¯¯| |¯¯¯¯¯¯¯¯¯¯ |¯¯¯¯¯¯¯¯¯| ¯¯¯¯¯¯¯¯¯¯| |         |
					, .m_TopRight = str_utf32('|')		// | Testing | | Testing   | Testing |   Testing | | Testing |
					, .m_BottomLeft = str_utf32('|')	// |_________| |__________ |         | __________| |_________|
					, .m_BottomRight = str_utf32('|')
					, .m_HorizontalTop = str_utf32('¯')
					, .m_HorizontalBottom = str_utf32('_')
					, .m_VerticalLeft = str_utf32('|')
					, .m_VerticalRight = str_utf32('|')
					, .m_HorizontalTopLeftOpen = str_utf32('¯')
					, .m_HorizontalTopRightOpen = str_utf32('¯')
					, .m_HorizontalBottomLeftOpen = str_utf32('_')
					, .m_HorizontalBottomRightOpen = str_utf32('_')
					, .m_VerticalTopLeftOpen = str_utf32('|')
					, .m_VerticalTopRightOpen = str_utf32('|')
					, .m_VerticalBottomLeftOpen = str_utf32('|')
					, .m_VerticalBottomRightOpen = str_utf32('|')
				}
				,
				{	// ETerminalBoxType::mc_NonBoxDrawingRounded
					.m_TopLeft = str_utf32('/')			// /¯¯¯¯¯¯¯¯¯\ /¯¯¯¯¯¯¯¯¯¯ /¯¯¯¯¯¯¯¯¯\ ¯¯¯¯¯¯¯¯¯¯\ |         |
					, .m_TopRight = str_utf32('\\')		// | Testing | | Testing   | Testing |   Testing | | Testing |
					, .m_BottomLeft = str_utf32('\\')	// \_________/ \__________ |         | __________/ \_________/
					, .m_BottomRight = str_utf32('/')
					, .m_HorizontalTop = str_utf32('¯')
					, .m_HorizontalBottom = str_utf32('_')
					, .m_VerticalLeft = str_utf32('|')
					, .m_VerticalRight = str_utf32('|')
					, .m_HorizontalTopLeftOpen = str_utf32('¯')
					, .m_HorizontalTopRightOpen = str_utf32('¯')
					, .m_HorizontalBottomLeftOpen = str_utf32('_')
					, .m_HorizontalBottomRightOpen = str_utf32('_')
					, .m_VerticalTopLeftOpen = str_utf32('|')
					, .m_VerticalTopRightOpen = str_utf32('|')
					, .m_VerticalBottomLeftOpen = str_utf32('|')
					, .m_VerticalBottomRightOpen = str_utf32('|')
				}
				,
				{	// ETerminalBoxType::mc_Light
					.m_TopLeft = str_utf32('┌')			// ┌─────────┐ ┌─────────╴ ┌─────────┐ ╶─────────┐ ╷         ╷
					, .m_TopRight = str_utf32('┐')		// │ Testing │ │ Testing   │ Testing │   Testing │ │ Testing │
					, .m_BottomLeft = str_utf32('└')	// └─────────┘ └─────────╴ ╵         ╵ ╶─────────┘ └─────────┘
					, .m_BottomRight = str_utf32('┘')
					, .m_HorizontalTop = str_utf32('─')
					, .m_HorizontalBottom = str_utf32('─')
					, .m_VerticalLeft = str_utf32('│')
					, .m_VerticalRight = str_utf32('│')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_Light)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_Light)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_Light)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_Light)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_Light)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_Light)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_Light)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_Light)
				}
				,
				{	// ETerminalBoxType::mc_LightRounded
					.m_TopLeft = str_utf32('╭')			// ╭─────────╮ ╭─────────╴ ╭─────────╮ ╶─────────╮ ╷         ╷
					, .m_TopRight = str_utf32('╮')		// │ Testing │ │ Testing   │ Testing │   Testing │ │ Testing │
					, .m_BottomLeft = str_utf32('╰')	// ╰─────────╯ ╰─────────╴ ╵         ╵ ╶─────────╯ ╰─────────╯
					, .m_BottomRight = str_utf32('╯')
					, .m_HorizontalTop = str_utf32('─')
					, .m_HorizontalBottom = str_utf32('─')
					, .m_VerticalLeft = str_utf32('│')
					, .m_VerticalRight = str_utf32('│')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightRounded)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightRounded)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightRounded)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightRounded)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightRounded)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightRounded)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightRounded)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightRounded)
				}
				,
				{	// ETerminalBoxType::mc_Heavy
					.m_TopLeft = str_utf32('┏')			// ┏━━━━━━━━━┓ ┏━━━━━━━━━╸ ┏━━━━━━━━━┓ ╺━━━━━━━━━┓ ╻         ╻
					, .m_TopRight = str_utf32('┓')		// ┃ Testing ┃ ┃ Testing   ┃ Testing ┃   Testing ┃ ┃ Testing ┃
					, .m_BottomLeft = str_utf32('┗')	// ┗━━━━━━━━━┛ ┗━━━━━━━━━╸ ╹         ╹ ╺━━━━━━━━━┛ ┗━━━━━━━━━┛
					, .m_BottomRight = str_utf32('┛')
					, .m_HorizontalTop = str_utf32('━')
					, .m_HorizontalBottom = str_utf32('━')
					, .m_VerticalLeft = str_utf32('┃')
					, .m_VerticalRight = str_utf32('┃')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_Heavy)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_Heavy)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_Heavy)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_Heavy)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_Heavy)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_Heavy)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_Heavy)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_Heavy)
				}
				,
				{	// ETerminalBoxType::mc_InnerHalf
					.m_TopLeft = str_utf32('▗')			// ▗▄▄▄▄▄▄▄▄▄▖ ▗▄▄▄▄▄▄▄▄▄▖ ▗▄▄▄▄▄▄▄▄▄▖ ▗▄▄▄▄▄▄▄▄▄▖ ▗         ▖
					, .m_TopRight = str_utf32('▖')		// ▐ Testing ▌ ▐ Testing   ▐ Testing ▌   Testing ▌ ▐ Testing ▌
					, .m_BottomLeft = str_utf32('▝')	// ▝▀▀▀▀▀▀▀▀▀▘ ▝▀▀▀▀▀▀▀▀▀▘ ▝         ▘ ▝▀▀▀▀▀▀▀▀▀▘ ▝▀▀▀▀▀▀▀▀▀▘
					, .m_BottomRight = str_utf32('▘')
					, .m_HorizontalTop = str_utf32('▄')
					, .m_HorizontalBottom = str_utf32('▀')
					, .m_VerticalLeft = str_utf32('▐')
					, .m_VerticalRight = str_utf32('▌')
					, .m_HorizontalTopLeftOpen = str_utf32('▗')
					, .m_HorizontalTopRightOpen = str_utf32('▖')
					, .m_HorizontalBottomLeftOpen = str_utf32('▝')
					, .m_HorizontalBottomRightOpen = str_utf32('▘')
					, .m_VerticalTopLeftOpen = str_utf32('▗')
					, .m_VerticalTopRightOpen = str_utf32('▖')
					, .m_VerticalBottomLeftOpen = str_utf32('▝')
					, .m_VerticalBottomRightOpen = str_utf32('▘')
				}
				,
				{	// ETerminalBoxType::mc_OuterHalf
					.m_TopLeft = str_utf32('▛')			// ▛▀▀▀▀▀▀▀▀▀▜ ▛▀▀▀▀▀▀▀▀▀▀ ▛▀▀▀▀▀▀▀▀▀▜ ▀▀▀▀▀▀▀▀▀▀▜ ▌         ▐
					, .m_TopRight = str_utf32('▜')		// ▌ Testing ▐ ▌ Testing   ▌ Testing ▐   Testing ▐ ▌ Testing ▐
					, .m_BottomLeft = str_utf32('▙')	// ▙▄▄▄▄▄▄▄▄▄▟ ▙▄▄▄▄▄▄▄▄▄▄ ▌         ▐ ▄▄▄▄▄▄▄▄▄▄▟ ▙▄▄▄▄▄▄▄▄▄▟
					, .m_BottomRight = str_utf32('▟')
					, .m_HorizontalTop = str_utf32('▀')
					, .m_HorizontalBottom = str_utf32('▄')
					, .m_VerticalLeft = str_utf32('▌')
					, .m_VerticalRight = str_utf32('▐')
					, .m_HorizontalTopLeftOpen = str_utf32('▀')
					, .m_HorizontalTopRightOpen = str_utf32('▀')
					, .m_HorizontalBottomLeftOpen = str_utf32('▄')
					, .m_HorizontalBottomRightOpen = str_utf32('▄')
					, .m_VerticalTopLeftOpen = str_utf32('▌')
					, .m_VerticalTopRightOpen = str_utf32('▐')
					, .m_VerticalBottomLeftOpen = str_utf32('▌')
					, .m_VerticalBottomRightOpen = str_utf32('▐')
				}
				,
				{	// ETerminalBoxType::mc_OuterHalfFullVertical
					.m_TopLeft = str_utf32('█')			// █▀▀▀▀▀▀▀▀▀█ █▀▀▀▀▀▀▀▀▀▀ █▀▀▀▀▀▀▀▀▀█ ▀▀▀▀▀▀▀▀▀▀█ █         █
					, .m_TopRight = str_utf32('█')		// █ Testing █ █ Testing   █ Testing █   Testing █ █ Testing █
					, .m_BottomLeft = str_utf32('█')	// █▄▄▄▄▄▄▄▄▄█ █▄▄▄▄▄▄▄▄▄▄ █         █ ▄▄▄▄▄▄▄▄▄▄█ █▄▄▄▄▄▄▄▄▄█
					, .m_BottomRight = str_utf32('█')
					, .m_HorizontalTop = str_utf32('▀')
					, .m_HorizontalBottom = str_utf32('▄')
					, .m_VerticalLeft = str_utf32('█')
					, .m_VerticalRight = str_utf32('█')
					, .m_HorizontalTopLeftOpen = str_utf32('▀')
					, .m_HorizontalTopRightOpen = str_utf32('▀')
					, .m_HorizontalBottomLeftOpen = str_utf32('▄')
					, .m_HorizontalBottomRightOpen = str_utf32('▄')
					, .m_VerticalTopLeftOpen = str_utf32('█')
					, .m_VerticalTopRightOpen = str_utf32('█')
					, .m_VerticalBottomLeftOpen = str_utf32('█')
					, .m_VerticalBottomRightOpen = str_utf32('█')
				}
				,
				{	// ETerminalBoxType::mc_FullHeavy
					.m_TopLeft = str_utf32('█')			// ███████████ ███████████ ███████████ ███████████ █         █
					, .m_TopRight = str_utf32('█')		// █ Testing █ █ Testing   █ Testing █   Testing █ █ Testing █
					, .m_BottomLeft = str_utf32('█')	// ███████████ ███████████ █         █ ███████████ ███████████
					, .m_BottomRight = str_utf32('█')
					, .m_HorizontalTop = str_utf32('█')
					, .m_HorizontalBottom = str_utf32('█')
					, .m_VerticalLeft = str_utf32('█')
					, .m_VerticalRight = str_utf32('█')
					, .m_HorizontalTopLeftOpen = str_utf32('█')
					, .m_HorizontalTopRightOpen = str_utf32('█')
					, .m_HorizontalBottomLeftOpen = str_utf32('█')
					, .m_HorizontalBottomRightOpen = str_utf32('█')
					, .m_VerticalTopLeftOpen = str_utf32('█')
					, .m_VerticalTopRightOpen = str_utf32('█')
					, .m_VerticalBottomLeftOpen = str_utf32('█')
					, .m_VerticalBottomRightOpen = str_utf32('█')
				}
				,
				{	// ETerminalBoxType::mc_Double
					.m_TopLeft = str_utf32('╔')			// ╔═════════╗ ╔═════════╸ ╔═════════╗ ╺═════════╗ ╻         ╻
					, .m_TopRight = str_utf32('╗')		// ║ Testing ║ ║ Testing   ║ Testing ║   Testing ║ ║ Testing ║
					, .m_BottomLeft = str_utf32('╚')	// ╚═════════╝ ╚═════════╸ ╹         ╹ ╺═════════╝ ╚═════════╝
					, .m_BottomRight = str_utf32('╝')
					, .m_HorizontalTop = str_utf32('═')
					, .m_HorizontalBottom = str_utf32('═')
					, .m_VerticalLeft = str_utf32('║')
					, .m_VerticalRight = str_utf32('║')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_Double)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_Double)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_Double)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_Double)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_Double)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_Double)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_Double)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_Double)
				}
				,
				{	// ETerminalBoxType::mc_LightDotted1
					.m_TopLeft = str_utf32('┌')			// ┌┈┈┈┈┈┈┈┈┈┐ ┌┈┈┈┈┈┈┈┈┈╴ ┌┈┈┈┈┈┈┈┈┈┐ ╶┈┈┈┈┈┈┈┈┈┐ ╷         ╷
					, .m_TopRight = str_utf32('┐')		// ┊ Testing ┊ ┊ Testing   ┊ Testing ┊   Testing ┊ ┊ Testing ┊
					, .m_BottomLeft = str_utf32('└')	// └┈┈┈┈┈┈┈┈┈┘ └┈┈┈┈┈┈┈┈┈╴ ╵         ╵ ╶┈┈┈┈┈┈┈┈┈┘ └┈┈┈┈┈┈┈┈┈┘
					, .m_BottomRight = str_utf32('┘')
					, .m_HorizontalTop = str_utf32('┈')
					, .m_HorizontalBottom = str_utf32('┈')
					, .m_VerticalLeft = str_utf32('┊')
					, .m_VerticalRight = str_utf32('┊')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted1)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted1)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted1)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted1)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted1)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted1)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted1)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted1)
				}
				,
				{	// ETerminalBoxType::mc_LightDotted2
					.m_TopLeft = str_utf32('┌')			// ┌┄┄┄┄┄┄┄┄┄┐ ┌┄┄┄┄┄┄┄┄┄╴ ┌┄┄┄┄┄┄┄┄┄┐ ╶┄┄┄┄┄┄┄┄┄┐ ╷         ╷
					, .m_TopRight = str_utf32('┐')		// ┆ Testing ┆ ┆ Testing   ┆ Testing ┆   Testing ┆ ┆ Testing ┆
					, .m_BottomLeft = str_utf32('└')	// └┄┄┄┄┄┄┄┄┄┘ └┄┄┄┄┄┄┄┄┄╴ ╵         ╵ ╶┄┄┄┄┄┄┄┄┄┘ └┄┄┄┄┄┄┄┄┄┘
					, .m_BottomRight = str_utf32('┘')
					, .m_HorizontalTop = str_utf32('┄')
					, .m_HorizontalBottom = str_utf32('┄')
					, .m_VerticalLeft = str_utf32('┆')
					, .m_VerticalRight = str_utf32('┆')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted2)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted2)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted2)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted2)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted2)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted2)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted2)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted2)
				}
				,
				{	// ETerminalBoxType::mc_LightDotted3
					.m_TopLeft = str_utf32('┌')			// ┌╌╌╌╌╌╌╌╌╌┐ ┌╌╌╌╌╌╌╌╌╌╴ ┌╌╌╌╌╌╌╌╌╌┐ ╶╌╌╌╌╌╌╌╌╌┐ ╷         ╷
					, .m_TopRight = str_utf32('┐')		// ╎ Testing ╎ ╎ Testing   ╎ Testing ╎   Testing ╎ ╎ Testing ╎
					, .m_BottomLeft = str_utf32('└')	// └╌╌╌╌╌╌╌╌╌┘ └╌╌╌╌╌╌╌╌╌╴ ╵         ╵ ╶╌╌╌╌╌╌╌╌╌┘ └╌╌╌╌╌╌╌╌╌┘
					, .m_BottomRight = str_utf32('┘')
					, .m_HorizontalTop = str_utf32('╌')
					, .m_HorizontalBottom = str_utf32('╌')
					, .m_VerticalLeft = str_utf32('╎')
					, .m_VerticalRight = str_utf32('╎')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted3)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted3)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted3)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted3)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted3)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted3)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted3)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted3)
				}
				,
				{	// ETerminalBoxType::mc_LightDotted1Rounded
					.m_TopLeft = str_utf32('╭')			// ╭┈┈┈┈┈┈┈┈┈╮ ╭┈┈┈┈┈┈┈┈┈╴ ╭┈┈┈┈┈┈┈┈┈╮ ╶┈┈┈┈┈┈┈┈┈╮ ╷         ╷
					, .m_TopRight = str_utf32('╮')		// ┊ Testing ┊ ┊ Testing   ┊ Testing ┊   Testing ┊ ┊ Testing ┊
					, .m_BottomLeft = str_utf32('╰')	// ╰┈┈┈┈┈┈┈┈┈╯ ╰┈┈┈┈┈┈┈┈┈╴ ╵         ╵ ╶┈┈┈┈┈┈┈┈┈╯ ╰┈┈┈┈┈┈┈┈┈╯
					, .m_BottomRight = str_utf32('╯')
					, .m_HorizontalTop = str_utf32('┈')
					, .m_HorizontalBottom = str_utf32('┈')
					, .m_VerticalLeft = str_utf32('┊')
					, .m_VerticalRight = str_utf32('┊')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted1Rounded)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted1Rounded)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted1Rounded)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted1Rounded)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted1Rounded)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted1Rounded)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted1Rounded)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted1Rounded)
				}
				,
				{	// ETerminalBoxType::mc_LightDotted2Rounded
					.m_TopLeft = str_utf32('╭')			// ╭┄┄┄┄┄┄┄┄┄╮ ╭┄┄┄┄┄┄┄┄┄╴ ╭┄┄┄┄┄┄┄┄┄╮ ╶┄┄┄┄┄┄┄┄┄╮ ╷         ╷
					, .m_TopRight = str_utf32('╮')		// ┆ Testing ┆ ┆ Testing   ┆ Testing ┆   Testing ┆ ┆ Testing ┆
					, .m_BottomLeft = str_utf32('╰')	// ╰┄┄┄┄┄┄┄┄┄╯ ╰┄┄┄┄┄┄┄┄┄╴ ╵         ╵ ╶┄┄┄┄┄┄┄┄┄╯ ╰┄┄┄┄┄┄┄┄┄╯
					, .m_BottomRight = str_utf32('╯')
					, .m_HorizontalTop = str_utf32('┄')
					, .m_HorizontalBottom = str_utf32('┄')
					, .m_VerticalLeft = str_utf32('┆')
					, .m_VerticalRight = str_utf32('┆')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted2Rounded)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted2Rounded)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted2Rounded)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted2Rounded)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted2Rounded)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted2Rounded)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted2Rounded)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted2Rounded)
				}
				,
				{	// ETerminalBoxType::mc_LightDotted3Rounded
					.m_TopLeft = str_utf32('╭')			// ╭╌╌╌╌╌╌╌╌╌╮ ╭╌╌╌╌╌╌╌╌╌╴ ╭╌╌╌╌╌╌╌╌╌╮ ╶╌╌╌╌╌╌╌╌╌╮ ╷         ╷
					, .m_TopRight = str_utf32('╮')		// ╎ Testing ╎ ╎ Testing   ╎ Testing ╎   Testing ╎ ╎ Testing ╎
					, .m_BottomLeft = str_utf32('╰')	// ╰╌╌╌╌╌╌╌╌╌╯ ╰╌╌╌╌╌╌╌╌╌╴ ╵         ╵ ╶╌╌╌╌╌╌╌╌╌╯ ╰╌╌╌╌╌╌╌╌╌╯
					, .m_BottomRight = str_utf32('╯')
					, .m_HorizontalTop = str_utf32('╌')
					, .m_HorizontalBottom = str_utf32('╌')
					, .m_VerticalLeft = str_utf32('╎')
					, .m_VerticalRight = str_utf32('╎')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted3Rounded)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted3Rounded)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted3Rounded)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╴'), ETerminalEndpointType::mc_LightDotted3Rounded)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted3Rounded)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╷'), ETerminalEndpointType::mc_LightDotted3Rounded)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted3Rounded)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╵'), ETerminalEndpointType::mc_LightDotted3Rounded)
				}
				,
				{	// ETerminalBoxType::mc_HeavyDotted1
					.m_TopLeft = str_utf32('┏')			// ┏┉┉┉┉┉┉┉┉┉┓ ┏┉┉┉┉┉┉┉┉┉╸ ┏┉┉┉┉┉┉┉┉┉┓ ╺┉┉┉┉┉┉┉┉┉┓ ╻         ╻
					, .m_TopRight = str_utf32('┓')		// ┋ Testing ┋ ┋ Testing   ┋ Testing ┋   Testing ┋ ┋ Testing ┋
					, .m_BottomLeft = str_utf32('┗')	// ┗┉┉┉┉┉┉┉┉┉┛ ┗┉┉┉┉┉┉┉┉┉╸ ╹         ╹ ╺┉┉┉┉┉┉┉┉┉┛ ┗┉┉┉┉┉┉┉┉┉┛
					, .m_BottomRight = str_utf32('┛')
					, .m_HorizontalTop = str_utf32('┉')
					, .m_HorizontalBottom = str_utf32('┉')
					, .m_VerticalLeft = str_utf32('┋')
					, .m_VerticalRight = str_utf32('┋')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted1)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_HeavyDotted1)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted1)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_HeavyDotted1)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_HeavyDotted1)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_HeavyDotted1)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_HeavyDotted1)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_HeavyDotted1)
				}
				,
				{	// ETerminalBoxType::mc_HeavyDotted2
					.m_TopLeft = str_utf32('┏')			// ┏┅┅┅┅┅┅┅┅┅┓ ┏┅┅┅┅┅┅┅┅┅╸ ┏┅┅┅┅┅┅┅┅┅┓ ╺┅┅┅┅┅┅┅┅┅┓ ╻         ╻
					, .m_TopRight = str_utf32('┓')		// ┇ Testing ┇ ┇ Testing   ┇ Testing ┇   Testing ┇ ┇ Testing ┇
					, .m_BottomLeft = str_utf32('┗')	// ┗┅┅┅┅┅┅┅┅┅┛ ┗┅┅┅┅┅┅┅┅┅╸ ╹         ╹ ╺┅┅┅┅┅┅┅┅┅┛ ┗┅┅┅┅┅┅┅┅┅┛
					, .m_BottomRight = str_utf32('┛')
					, .m_HorizontalTop = str_utf32('┅')
					, .m_HorizontalBottom = str_utf32('┅')
					, .m_VerticalLeft = str_utf32('┇')
					, .m_VerticalRight = str_utf32('┇')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted2)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_HeavyDotted2)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted2)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_HeavyDotted2)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_HeavyDotted2)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_HeavyDotted2)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_HeavyDotted2)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_HeavyDotted2)
				}
				,
				{	// ETerminalBoxType::mc_HeavyDotted3
					.m_TopLeft = str_utf32('┏')			// ┏╍╍╍╍╍╍╍╍╍┓ ┏╍╍╍╍╍╍╍╍╍╸ ┏╍╍╍╍╍╍╍╍╍┓ ╺╍╍╍╍╍╍╍╍╍┓ ╻         ╻
					, .m_TopRight = str_utf32('┓')		// ╏ Testing ╏ ╏ Testing   ╏ Testing ╏   Testing ╏ ╏ Testing ╏
					, .m_BottomLeft = str_utf32('┗')	// ┗╍╍╍╍╍╍╍╍╍┛ ┗╍╍╍╍╍╍╍╍╍╸ ╹         ╹ ╺╍╍╍╍╍╍╍╍╍┛ ┗╍╍╍╍╍╍╍╍╍┛
					, .m_BottomRight = str_utf32('┛')
					, .m_HorizontalTop = str_utf32('╍')
					, .m_HorizontalBottom = str_utf32('╍')
					, .m_VerticalLeft = str_utf32('╏')
					, .m_VerticalRight = str_utf32('╏')
					, .m_HorizontalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted3)
					, .m_HorizontalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_HeavyDotted3)
					, .m_HorizontalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted3)
					, .m_HorizontalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╸'), ETerminalEndpointType::mc_HeavyDotted3)
					, .m_VerticalTopLeftOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_HeavyDotted3)
					, .m_VerticalTopRightOpen = fg_CharacterAndEndpointType(str_utf32('╻'), ETerminalEndpointType::mc_HeavyDotted3)
					, .m_VerticalBottomLeftOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_HeavyDotted3)
					, .m_VerticalBottomRightOpen = fg_CharacterAndEndpointType(str_utf32('╹'), ETerminalEndpointType::mc_HeavyDotted3)
				}
			}
		;

	}
}
