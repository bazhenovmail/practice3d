#include <windows.h>
#include <assert.h>
#include <string>

#include <World.h>
#include <Graphics.h>
#include <Input.h>
#include "UI.h"
#include <Timer.h>
#include <FpsMeter.h>

#include <EffectColor.h>
#include <EffectText.h>
#include <EffectTexture.h>

#include "IApplication.h"

HINSTANCE g_hinstance;
HWND g_hwnd;
const std::wstring g_application_name = L"Engine";

namespace
{
BallPhysics::World world;
BallGraphics::Graphics graphics;
BallInput::Input input;
BallGraphics::EffectColor effectColor;
BallGraphics::EffectText effectText;
BallGraphics::EffectTexture effectTexture;
BallGraphics::Font font;
BallGraphics::Texture fontTexture;
UI ui;
Timer timer;
FpsMeter fpsMeter;
}

namespace Balls
{

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch(umessage)
    {
    case WM_INPUT:
    {
        input.processInput(lparam);
        //break;
    }

    // Any other messages send to the default message handler as our application won't make use of them.
    default:
    {
        return DefWindowProc(hwnd, umessage, wparam, lparam);
    }
    }
}

void initialize_windows(const Balls::Params& params, int& screen_width, int& screen_height)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;

    // Get the instance of this application.
    g_hinstance = GetModuleHandle(nullptr);
    assert(g_hinstance != NULL);

    // Setup the windows class with default settings.
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = g_hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = g_application_name.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    ATOM atom = RegisterClassEx(&wc);
    assert(atom != NULL);

    // Determine the resolution of the clients desktop screen.
    screen_width = GetSystemMetrics(SM_CXSCREEN);
    assert(screen_width != 0);
    screen_height = GetSystemMetrics(SM_CYSCREEN);
    assert(screen_height != 0);

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
    if(params.g_full_screen)
    {
        // If full screen set the screen to maximum size of the users desktop and 32bit.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screen_width;
        dmScreenSettings.dmPelsHeight = (unsigned long)screen_height;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        LONG ch_result = ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        assert(ch_result == DISP_CHANGE_SUCCESSFUL);

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    }
    else
    {
        // Place the window in the middle of the screen.
        posX = (screen_width - params.g_windows_screen_width) / 2;
        posY = (screen_height - params.g_windows_screen_height) / 2;
        screen_width = params.g_windows_screen_width;
        screen_height = params.g_windows_screen_height;
    }

    // Create the window with the screen settings and get the handle to it.
    g_hwnd = CreateWindowEx(WS_EX_APPWINDOW, g_application_name.c_str(), g_application_name.c_str(),
                            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                            posX, posY, screen_width, screen_height, nullptr, nullptr, g_hinstance, nullptr);
    assert(g_hwnd != NULL);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(g_hwnd, SW_SHOW);
    SetForegroundWindow(g_hwnd);
    HWND old_focus_window = SetFocus(g_hwnd);
    assert(old_focus_window != NULL);

    // Hide the mouse cursor.
    ShowCursor(false);
}

void shutdown_windows()
{
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if(Balls::getApplication().getParams().g_full_screen)
    {
        LONG ch_result = ChangeDisplaySettings(nullptr, 0);
        assert(ch_result == DISP_CHANGE_SUCCESSFUL);
    }

    // Remove the window.
    BOOL destroy_result = DestroyWindow(g_hwnd);
    assert(destroy_result != FALSE);

    g_hwnd = nullptr;

    // Remove the application instance.
    destroy_result = UnregisterClass(g_application_name.c_str(), g_hinstance);
    assert(destroy_result != FALSE);

    g_hinstance = nullptr;
}

}//namespace Balls

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    Balls::IApplication& application = Balls::getApplication();
    auto params = application.getParams();

    int screen_width, screen_height;
    initialize_windows(params, screen_width, screen_height);

    graphics.initialize(screen_width, screen_height, params.g_vsync_enabled, g_hwnd, params.g_full_screen);
    world.initialize(screen_width, screen_height);
    input.initialize(g_hwnd);

	graphics.addEffect(&effectColor);
	graphics.addEffect(&effectText);
	graphics.addEffect(&effectTexture);

	fontTexture = graphics.createTexture("../../BallGraphics/Resources/font.tga");
	font.load("../../BallGraphics/Resources/font.txt");

	ui.initialize(graphics, effectText, effectColor, effectTexture, font, fontTexture);

    application.initialize(world, ui, input);

	auto textMesh = ui.addUITextMesh("fps: ", { 0.1f, 0.1f, 0.f, 1. });
	auto textInst = ui.addUIMeshInst(textMesh, 2, [screen_width, screen_height]() {return Vector{ -screen_width / 2.f + 8, -screen_height / 2.f + 8 }; });

    timer.reset(Timer::Clock::duration(Timer::Clock::rep(Timer::Clock::period::den / 60.f)));
    timer.start();

    MSG msg;
    bool done{false};
    Balls::FrameResult result{false, 0.f};

    ZeroMemory(&msg, sizeof(MSG));
    while(!done)
    {
        BOOL is_msg{FALSE};
        do
        {
            is_msg = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
            if(is_msg)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if(msg.message == WM_QUIT)
                {
                    done = true;
                }
            }
        }
        while(is_msg);

        std::chrono::milliseconds physDiff;

        if(!done)
        {
            timer.reset(Timer::Clock::duration());
			input.frame();
            result = application.frame();
            if(!result.running)
                done = true;
            else
            {
                fpsMeter.frame();
                BallGraphics::Id textId;
                bool addedFps = fpsMeter.canGet();
                
                if(addedFps)
                {
                    auto fps = fpsMeter.getFps();

					const auto& tMesh = ui.getGraphics().adjustMesh(textMesh);
					BallGraphics::TextMesh* meshP = dynamic_cast<BallGraphics::TextMesh*>(tMesh.get());
					assert(meshP);
					meshP->updateText("fps: " + std::to_string(fps));
                }

                graphics.frame();

                //if(result.second)
                //{
                    timer.update();

                    auto physStart = Timer::Clock::now();
                    world.frame(result.dt);//timer.getInSeconds());
                    auto physEnd = Timer::Clock::now();
                    physDiff = std::chrono::duration_cast<std::chrono::milliseconds> (physEnd - physStart);
                //}
            }
        }
    }

    graphics.shutdown();

    Balls::shutdown_windows();

    return 0;
}