#pragma once
#include <unordered_map>
#include <assert.h>

template<typename Key, typename... CallbackArgs>
class CallbackProcessor
{
public:
	using Callback = std::function<bool(CallbackArgs...)>;
    void add(Key key, Callback callback) noexcept;
    void clear(Key key) noexcept;
    void process(Key key, CallbackArgs...) noexcept;
private:
    using Container = std::unordered_multimap<Key, Callback>;
    Container callbacks_;
    Container callbacksTemp_;
	std::vector<Key> toClear_;
    Container* addHere_{&callbacks_};
};

template<typename Key, typename... CallbackArgs>
inline void CallbackProcessor<Key, CallbackArgs...>::add(Key key, Callback callback) noexcept
{
    addHere_->emplace(key, std::move(callback));
}

template<typename Key, typename... CallbackArgs>
inline void CallbackProcessor<Key, CallbackArgs...>::clear(Key key) noexcept
{
	if (addHere_ == &callbacks_)
	{
		auto numErased = callbacks_.erase(key);
		assert(numErased > 0);
	}
	else
	{
		assert(callbacks_.find(key) != callbacks_.end());
		toClear_.push_back(key);		
	}    
}

template<typename Key, typename... CallbackArgs>
inline void CallbackProcessor<Key, CallbackArgs...>::process(Key key, CallbackArgs... args) noexcept
{
    addHere_ = &callbacksTemp_;
    auto equalRange = callbacks_.equal_range(key);
    for(auto cbkIt = equalRange.first; cbkIt != equalRange.second;)
    {
        assert(cbkIt->second);
        if(cbkIt->second(args...))
        {
            cbkIt = callbacks_.erase(cbkIt);
        }
        else
        {
            cbkIt++;
        }
    }
	//callbacks_.merge(callbacksTemp_);
	for (const auto& key : toClear_)
	{
		auto numErased = callbacks_.erase(key);
		assert(numErased > 0);
	}
	toClear_.clear();
	for (auto& it : callbacksTemp_)
	{
		callbacks_.emplace(std::move(it));
	}
	callbacksTemp_.clear();
    addHere_ = &callbacks_;
}
