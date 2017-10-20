#pragma once

#include "windows.h"
#include <utility>
#include <functional>
#include <unordered_map>
#include <array>
#include "CallbackProcessor.h"

namespace BallInput
{

class Input
{
public:
	//TODO: query all controls state during initialization
	enum class MouseButtonFlag
	{
		Left = 0x1,
		Right = 0x2,
		Middle = 0x4,
		Button4 = 0x8,
		Button5 = 0x10
	};

    bool initialize(HWND hwnd);

	void frame() noexcept;

    using SimpleCallback = std::function<bool()>; //return whether it should be deleted after call
    using MouseMoveCallback = std::function<void(LONG x, LONG y)>;

    void setOnKeyDown(unsigned key, SimpleCallback) noexcept;
    void setOnMouseMove(MouseMoveCallback) noexcept;
	void setOnMouseDown(MouseButtonFlag, SimpleCallback) noexcept;
	void setOnMouseUp(MouseButtonFlag, SimpleCallback) noexcept;

	void clearOnKeyDown(unsigned key) noexcept;
	void clearOnMouseDown(MouseButtonFlag key) noexcept;
	void clearOnMouseUp(MouseButtonFlag key) noexcept;

    void processInput(LPARAM lparam) noexcept;

private:
    struct KeyStatus_
    {
        bool down{false};
    };

    std::array<KeyStatus_, 0x100> keys_;
	int mouseButtons_;
	std::pair<LONG, LONG> mouseMovement_{ 0, 0 };

	MouseMoveCallback mouseMoveCallback_;
	CallbackProcessor<unsigned> downCallbacks_;
	CallbackProcessor<MouseButtonFlag> mouseDownCallbacks_;
	CallbackProcessor<MouseButtonFlag> mouseUpCallbacks_;

	void processMouse_(const RAWMOUSE& data) noexcept;
};

}//BallInput