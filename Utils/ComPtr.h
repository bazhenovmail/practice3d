#pragma once

namespace BallUtils
{

template<class T>
class ComPtr
{
public:
    ComPtr(T * const arg) noexcept:
		ptr_{arg}
	{
    }

    ComPtr() noexcept :
        ptr_{nullptr}
	{
    }

    ~ComPtr()
	{
        release();
    }

    T* operator = (T* arg) noexcept
	{
        if(ptr_ != arg)
		{
            release();
            ptr_ = arg;
        }
        return arg;
    }

    bool operator == (const T* arg) const noexcept
	{
        return ptr_ == arg;
    }

    bool operator != (const T* arg) const noexcept
	{
        return ptr_ != arg;
    }

    operator T* () const noexcept
	{
        return ptr_;
    }

    T * operator->() noexcept
	{
        return ptr_;
    }

	const T * operator->() const noexcept
	{
		return ptr_;
	}

    bool release() noexcept
	{
        if(ptr_)
		{
            ptr_->Release();
            ptr_ = nullptr;
            return true;
        }
        return false;
    }

    ComPtr(ComPtr&& arg) noexcept
	{
        ptr_ = arg.ptr_;
        arg.ptr_ = nullptr;
    }

    ComPtr& operator = (ComPtr&& arg) noexcept
	{
        std::swap(arg.ptr_, ptr_);
        return *this;
    }

private:
    T* ptr_;

private:
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator = (const ComPtr&) = delete;
};

}