#pragma once
#include "TreeNode.h"

template<typename P, typename ...Ts>
class MenuNode: public TreeNode<P, Ts...>
{
public:
    constexpr MenuNode(P& parent, Ts... nodes) noexcept;
private:
};

template<typename P, typename ...Ts>
constexpr inline MenuNode<P, Ts...>::MenuNode(P & parent, Ts ...nodes) noexcept:
    TreeNode(parent, nodes...)
{
}