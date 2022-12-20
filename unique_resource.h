#pragma once

#include <type_traits>
#include <memory>
#include <utility>

template <typename RT>
class UniqueResource
{
public:
    typedef RT                    ResourceTraits;
    typedef typename RT::Resource Resource;
    typedef typename RT::Cleaner  Cleaner;

    static constexpr Resource InvalidValue = ResourceTraits::InvalidValue;

    // Now we assume the most strong requirements for 'Resource' type
    // They might be reduced in future if needed
    static_assert(std::is_nothrow_constructible<Resource>() &&
                  std::is_nothrow_copy_constructible<Resource>() &&
                  std::is_nothrow_copy_assignable<Resource>(),
                  "such resource type is not supported now");
    static_assert(std::is_nothrow_move_constructible<Resource>() &&
                  std::is_nothrow_move_assignable<Resource>(),
                  "such resource type is not supported now");

    static_assert(std::is_nothrow_default_constructible<Cleaner>(),
                  "such resource cleaner type is not supported now");

    static_assert(noexcept(Cleaner().operator()(InvalidValue)),
                  "cleanup operator() must be 'noexcept'");

    UniqueResource() noexcept = default;

    UniqueResource(UniqueResource const&) = delete;
    void operator=(UniqueResource const&) const = delete;

    explicit UniqueResource(Resource r) noexcept
        : m_r(r)
    {
    }

    UniqueResource(UniqueResource&& other) noexcept
        : m_r(other.Release())
    {
    }

    ~UniqueResource() noexcept
    {
        this->Reset();
    }

    UniqueResource& operator=(UniqueResource&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            UniqueResource(std::move(other)).Swap(*this);
        }

        return *this;
    }

    bool IsValid() const noexcept
    {
        return m_r != InvalidValue;
    }

    bool IsInvalid() const noexcept
    {
        return !this->IsValid();
    }

    explicit operator bool() const noexcept
    {
        return this->IsValid();
    }

    const Resource Release() noexcept
    {
        const Resource r(m_r);
        m_r = InvalidValue;
        return r;
    }

    void Reset() noexcept
    {
        if (m_r != InvalidValue)
        {
            Cleaner()(m_r);
            m_r = InvalidValue;
        }
    }

    void Reset(Resource r) noexcept
    {
        if (m_r != r)
        {
            UniqueResource(r).Swap(*this);
        }
    }

    void Swap(UniqueResource& other) noexcept
    {
        using std::swap;
        swap(m_r, other.m_r);
    }

    Resource Get() const noexcept
    {
        return m_r;
    }

    operator Resource() const noexcept
    {
        return m_r;
    }

    Resource operator->() const noexcept
    {
        return m_r;
    }

    typename std::add_lvalue_reference<typename std::add_const<
        typename std::remove_pointer<Resource>::type>::type>::type
    operator*() const noexcept(noexcept(*UniqueResource::self_))
    {
        return *m_r;
    }

private:
    static Resource& self_; // ODR is not used
                            // Just for applying 'noexcept()' expression in 'operator*()' above

    Resource m_r = InvalidValue;

    // assistant
    class Assistant
    {
        friend class UniqueResource;

        UniqueResource* m_ur;
        Resource m_rAcceptor;

        explicit Assistant(UniqueResource& ur) noexcept
             : m_ur(&ur)
             , m_rAcceptor(ur.m_r)
        {
        }

        Assistant(const Assistant&) = delete;
        void operator=(const Assistant&) const = delete;
        void operator=(const Assistant&&) const = delete;

        // 'move ctor' is needed syntactically only
        // if not uncomment it's code and one line in the dtor
        Assistant(Assistant&& other) noexcept;
        //     : m_ur(other.m_ur)
        //     , m_rAcceptor(other.m_rAcceptor)
        // {
        //     other.m_ur = nullptr;
        // }

    public:
        ~Assistant() noexcept
        {
            m_ur->Reset(m_rAcceptor);
        }

        operator Resource*() noexcept
        {
            return &m_rAcceptor;
        }
    };

public:
    Assistant Assist() noexcept
    {
        return Assistant(*this);
    }
};
