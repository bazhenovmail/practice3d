#include "Menu.h"
#include <Input.h>
#include <VectorToMatrix.h>

void Menu::initialize(UI & ui, BallInput::Input& input) noexcept
{
	ui_ = &ui;
	input_ = &input;
}

void Menu::enable(bool value)
{
    assert(value != enabled_);
    assert(items_.size() > 0);

    if(!enabled_)
    {
        float totalH = itemHeight * items_.size();
        float startH = totalH * 0.5f;
        float h = startH;

        for(const auto& item : items_)
        {
			auto textMesh = ui_->addUITextMesh(item.text, { 0., 1., 0., 1. });
			addedTextMeshes_.push_back(textMesh);
			addedTextMeshInstances_.push_back(ui_->addUIMeshInst(textMesh, 2, [h]() {return Vector{ 0., h }; }));
            h -= itemHeight;
        }
        activeElement_ = 0;
		addedBacks_.push_back(ui_->addMenuRectInst([this, startH]()
		{
			return Vector{ 48.8f, startH + itemHeight * (items_.size() - 1 - 0.5f - activeElement_) };
		}));
        input_->setOnKeyDown(VK_DOWN, [this]()
        {
            if(activeElement_ + 1 < items_.size())
            {
                activeElement_++;
            }
            else
            {
                activeElement_ = 0;
            }
            return false;
        });
        input_->setOnKeyDown(VK_UP, [this]()
        {
            if(activeElement_ > 0)
            {
                activeElement_--;
            }
            else
            {
                activeElement_ = items_.size() - 1;
            }
            return false;
        });
        input_->setOnKeyDown(VK_RETURN, [this]()
        {
            assert(activeElement_ < items_.size());
            if(items_[activeElement_].callback)
            {
                items_[activeElement_].callback();
            }
            return false;
        });
    }
    else
    {
		for (auto id : addedTextMeshInstances_)
		{
			ui_->getGraphics().removeMeshInstance(id);
		}
		addedTextMeshInstances_.clear();
        for(auto id : addedTextMeshes_)
        {
            ui_->getGraphics().removeMesh(id);
        }
        addedTextMeshes_.clear();
        for(auto id : addedBacks_)
        {
			ui_->getGraphics().removeMeshInstance(id);
        }
		addedBacks_.clear();
        input_->clearOnKeyDown(VK_DOWN);
        input_->clearOnKeyDown(VK_UP);
        input_->clearOnKeyDown(VK_RETURN);
    }

    enabled_ = value;
}

void Menu::addItem(const std::string& str, const Callback& cbk)
{
    assert(!enabled_);
    items_.emplace_back(Item_{str, cbk});
}

size_t Menu::size() const
{
	return items_.size();
}
