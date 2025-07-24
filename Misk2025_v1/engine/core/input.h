#pragma once
#include <glm/glm.hpp>


#include "key_code.h"


namespace input {

	void init();
	void update();
	bool key_pressed(unsigned int keycode);
	bool key_down(unsigned int keycode);
	float get_mouse_offset_x();
	float get_mouse_offset_y();
	bool mouse_button_pressed(int button);
	bool mouse_button_down(int button);
	bool mouse_button_released(int button);
	//bool MouseWheelUp();
	//bool MouseWheelDown();
	//int GetMouseWheelValue();
	double get_mouse_x();
	double get_mouse_y();
	glm::vec2 get_mouse_pos();
	//void PreventRightMouseHold();
	//int GetScrollWheelYOffset();
	//void ResetScrollWheelYOffset();
	////int GetCursorX();
	////int GetCursorY();
	//void DisableCursor();
	//void HideCursor();
	//void ShowCursor();
	//int GetCursorScreenX();
	//int GetCursorScreenY();
	//int GetViewportMappedMouseX(int viewportWidth);
	//int GetViewportMappedMouseY(int viewportHeight);


	//window
	void handle_window();
}
