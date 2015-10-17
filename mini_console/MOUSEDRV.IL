:-	i_begin(mouse_driver),
	i_export([
		reset_mouse_driver,
		enable_cursor_display,
		disable_cursor_display,
		read_cursor_button_state,
		set_cursor_location
	]).

reset_mouse_driver(MouseState, NumButtons) :-
	os_interrupt(51, [0], [MouseState, NumButtons | _]).
/*
gives the current status of the mouse hardware plus the current
status of the mouse software.  The calling program is able to determine the
presence of a mouse driver and/or a serial port.

This function resets the mouse driver to the following default status as
indicated:

Variable                                Value
------------------------------------------------------------------------------
internal cursor flag                    -1 (cursor concealed)
graphics  cursor shape                  horizontal oval
text cursor                             reverse video
user-defined call mask                  all zeroes
light pen emulation mode                enabled
vertical mouse motion/pixel ratio       16 to 8
horizontal mouse motion/pixel ratio     8 to 8
vertical min/max cursor coordinates     0/current display mode y values minus 1
horizontal min/max cursor coordinates   0/current display mode x values minus 1

8086 Register
Input:  AX = 0
Return: AX = mouse state (-1: installed, 0: not installed)
        BX = number of buttons (2 button mode, 3 button mode)
*/

enable_cursor_display :-
	os_interrupt(51, [1], _).
/*
increments the internal cursor flag counter.  If the counter is zero,
the cursor is enabled and appears on-screen.

The default value is -1 which indicates a concealed cursor.  Function 1 must be
called to display the cursor.  In case the internal cursor flag is already zero,
a call to this function produces no effect.

8086 Register
Input:  AX = 1
Return: none
*/

disable_cursor_display :-
	os_interrupt(51, [2], _).
/*
disables the cursor by removing it from the screen and decrementing
the internal cursor flag.  Even though the cursor cannot be seen, it still
tracks any motion made with the mouse.

You should use this function before changing any portion of the screen
containing the cursor.  You will avoid the problem of the cursor affecting
screen data.

Keep in mind that whenever your program calls function 2, it must later call
function 1 to return the internal cursor flag to its default value.  In
addition, if your program changes the screen mode, function 2 is called
automatically. Therefore, the cursor's movement is enabled the next time it is
displayed.

Call function 2 at the end of a program in order to conceal the cursor.  This
ensures that nothing remains on-screen.

8086 Register
Input:  AX = 2
Return: none
*/

read_cursor_button_state(ButtonStatus, HorCoord, VerCoord) :-
	os_interrupt(51, [3], [_, ButtonStatus, HorCoord, VerCoord | _]).
/*
gives the status of mouse buttons, plus cursor location.

Button status consists of a single integer value:
  Bit 0 = left button (2 button mode, 3 button mode)
  Bit 1 = right button (2 button mode, 3 button mode)
  Bit 2 = middle button (3 button mode)

The bit is 1 when the button is pressed.  The bit is 0 when the button is
released.

8086 Register
Input:  AX = 3
Return: BX = button status
        CX = horizontal cursor coordinate
        DX = vertical cursor coordinate
*/

set_cursor_location(HorCoord, VerCoord) :-
	os_interrupt(51, [4, _, HorCoord, VerCoord], _).
/*
sets the current cursor location.  Values must be within the
coordinate ranges for the virtual screen and, if necessary, are rounded to the
nearest values allowed for the current screen mode.

  Screen     Display        Virtual         Cell        Bits/Pixel
   Mode      Adapter       Screen (XxY)     Size      Graphics Mode
---------  ------------  ---------------  --------   ----------------
    0      C, E, 3270     640 x 200        16 x 8          -
    1      C, E, 3270     640 x 200        16 x 8          -
    2      C, E, 3270     640 x 200         8 x 8          -
    3      C, E, 3270     640 x 200         8 x 8          -
    4      C, E, 3270     640 x 200         2 x 1          2
    5      C, E, 3270     640 x 200         2 x 1          2
    6      C, E, 3270     640 x 200         1 x 1          1
    7      M, E, 3270     640 x 200         8 x 8          -
    D      E              640 x 200        16 x 8          2
    E      E              640 x 200         1 x 1          1
    F      E              640 x 350         1 x 1          1
    10     E              640 x 350         1 x 1          1
    30     3270           720 x 350         1 x 1          1
           H              720 x 348         1 x 1          1

Display Adapter:
  M = IBM Monochrome Display/Printer Adapter
  C = IBM Color/Graphics Adapter
  E = IBM Enhanced Graphics Adapter
3270 = IBM All Points Addressable Graphics Adapter (3270 PC)
  H = Hercules Monochrome Graphics Card

8086 Register
Input:  AX = 4
        CX = new horizontal cursor coordinate
        DX = new vertical cursor coordinate
Return: none
*/

:-	i_end(mouse_driver).
