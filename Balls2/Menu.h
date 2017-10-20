#pragma once
#include <functional>
#include <vector>
#include "UI.h"

namespace BallInput
{
class Input;
}

class Menu
{
public:
    void initialize(UI& ui, BallInput::Input& input) noexcept;

    void enable(bool);

    using Callback = std::function<void()>;

    void addItem(const std::string&, const Callback&);

	size_t size() const;

	static constexpr float itemHeight = 20.f;
private:
    UI* ui_{ nullptr };
	BallInput::Input* input_{ nullptr };

    bool enabled_{false};

    struct Item_
    {
        std::string text;
        Callback callback;
    };
    std::vector<Item_> items_;
    size_t activeElement_;

    std::vector<BallGraphics::Id> addedTextMeshes_;
	std::vector<BallGraphics::Id> addedTextMeshInstances_;
    std::vector<BallGraphics::Id> addedBacks_;
};

