#pragma once
#include <tuple>

template<typename P, typename... Ts>
class TreeNode
{
public:
    constexpr TreeNode(P& parent, Ts... nodes) noexcept;
protected:
    P& parent_;
    std::tuple<Ts...> nodes_;
};

template<typename P, typename ...Ts>
constexpr inline TreeNode<P, Ts...>::TreeNode(P & parent, Ts ...nodes) noexcept:
    parent_{parent}, nodes_{nodes...}
{
}