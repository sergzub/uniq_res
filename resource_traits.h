#pragma once


template <typename R, R IV, auto CF>
struct ResourceTraits
{
    typedef R Resource;

    static constexpr Resource InvalidValue = IV;

    struct Cleaner
    {
        void operator()(Resource r) const noexcept
        {
            CF(r);
        }
    };
};
