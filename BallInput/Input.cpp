#include "Input.h"
#include <string>
#include <assert.h>

namespace BallInput
{

bool Input::initialize(HWND hwnd)
{
    RAWINPUTDEVICE Rid[2];
    Rid[0].usUsagePage = 0x01;// this is the usagepage for desktop devices
    Rid[0].usUsage = 0x02;// this is the usage code for mouse devices
    Rid[0].dwFlags = RIDEV_DEVNOTIFY;
    Rid[0].hwndTarget = hwnd;

    // this enabled keyboard detection
    Rid[1].usUsagePage = 0x01;// this is the usagepage for desktop devices
    Rid[1].usUsage = 0x06; // this is the usage code for keyboard devices
    Rid[1].dwFlags = RIDEV_DEVNOTIFY;
    Rid[1].hwndTarget = hwnd;

    // register input
    if(!RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE))){
        //appGetLastErrorMsg();
        return false;
    }

    return true;
}

void Input::frame() noexcept
{
	if (mouseMoveCallback_)
	{
		mouseMoveCallback_(mouseMovement_.first, mouseMovement_.second);
	}
	mouseMovement_ = { 0, 0 };
}

void Input::setOnKeyDown(unsigned key, SimpleCallback callback) noexcept
{
	downCallbacks_.add(key, std::move(callback));
}

void Input::setOnMouseMove(MouseMoveCallback callback) noexcept
{
	mouseMoveCallback_ = std::move(callback);
}

void Input::setOnMouseDown(MouseButtonFlag key, SimpleCallback callback) noexcept
{
	mouseDownCallbacks_.add(key, std::move(callback));
}

void Input::setOnMouseUp(MouseButtonFlag key, SimpleCallback callback) noexcept
{
	mouseUpCallbacks_.add(key, std::move(callback));
}

void Input::clearOnKeyDown(unsigned key) noexcept
{
	downCallbacks_.clear(key);
}

void Input::clearOnMouseDown(MouseButtonFlag key) noexcept
{
	mouseDownCallbacks_.clear(key);
}

void Input::clearOnMouseUp(MouseButtonFlag key) noexcept
{
	mouseUpCallbacks_.clear(key);
}

void Input::processInput(LPARAM lparam) noexcept
{
    UINT dwSize;
    //HRESULT hResult{ 0 };

    GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize,
                    sizeof(RAWINPUTHEADER));
    std::vector<BYTE> lpb(dwSize);

    if(GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb.data(), &dwSize,
                       sizeof(RAWINPUTHEADER)) != dwSize)
        OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

    RAWINPUT* raw = (RAWINPUT*)lpb.data();

    if(raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        if(raw->data.keyboard.Message == WM_KEYDOWN)
        {
            if(!keys_[raw->data.keyboard.VKey].down)
            {
                keys_[raw->data.keyboard.VKey].down = true;

				downCallbacks_.process(raw->data.keyboard.VKey);
            }
        }
        else if(raw->data.keyboard.Message == WM_KEYUP)
        {
            keys_[raw->data.keyboard.VKey].down = false;
        }
    }
    else if(raw->header.dwType == RIM_TYPEMOUSE)
    {
		processMouse_(raw->data.mouse);
    }
}

void Input::processMouse_(const RAWMOUSE & data) noexcept
{
	//assert(data.usFlags & MOUSE_MOVE_ABSOLUTE == 0);
	mouseMovement_.first += data.lLastX;
	mouseMovement_.second += data.lLastY;

	if (data.usButtonFlags)
	{
		int flags{ 0 };
		int flagsMask{ 0 };

		flagsMask |= (int)MouseButtonFlag::Left * (data.usButtonFlags & (RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_LEFT_BUTTON_UP) != 0);
		flagsMask |= (int)MouseButtonFlag::Right * (data.usButtonFlags & (RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_UP) != 0);
		flagsMask |= (int)MouseButtonFlag::Middle * (data.usButtonFlags & (RI_MOUSE_MIDDLE_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_UP) != 0);
		flagsMask |= (int)MouseButtonFlag::Button4 * (data.usButtonFlags & (RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_4_UP) != 0);
		flagsMask |= (int)MouseButtonFlag::Button5 * (data.usButtonFlags & (RI_MOUSE_BUTTON_5_DOWN | RI_MOUSE_BUTTON_5_UP) != 0);
		flags |= (int)MouseButtonFlag::Left * (RI_MOUSE_LEFT_BUTTON_DOWN != 0);
		flags |= (int)MouseButtonFlag::Right * (RI_MOUSE_RIGHT_BUTTON_DOWN != 0);
		flags |= (int)MouseButtonFlag::Middle * (RI_MOUSE_MIDDLE_BUTTON_DOWN != 0);
		flags |= (int)MouseButtonFlag::Button4 * (RI_MOUSE_BUTTON_4_DOWN != 0);
		flags |= (int)MouseButtonFlag::Button5 * (RI_MOUSE_BUTTON_5_DOWN != 0);

		assert((mouseButtons_ & flagsMask) ^ (flags & flagsMask) == flagsMask);
		mouseButtons_ |= flags & flagsMask;
		mouseButtons_ &= ~(flags ^ flagsMask);
		
		if (data.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
			mouseDownCallbacks_.process(MouseButtonFlag::Left);
		if (data.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
			mouseDownCallbacks_.process(MouseButtonFlag::Right);
		if (data.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			mouseDownCallbacks_.process(MouseButtonFlag::Middle);
		if (data.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
			mouseDownCallbacks_.process(MouseButtonFlag::Button4);
		if (data.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
			mouseDownCallbacks_.process(MouseButtonFlag::Button5);
		if (data.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
			mouseUpCallbacks_.process(MouseButtonFlag::Left);
		if (data.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
			mouseUpCallbacks_.process(MouseButtonFlag::Right);
		if (data.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
			mouseUpCallbacks_.process(MouseButtonFlag::Middle);
		if (data.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
			mouseUpCallbacks_.process(MouseButtonFlag::Button4);
		if (data.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
			mouseUpCallbacks_.process(MouseButtonFlag::Button5);
	}
}

}//BallInput