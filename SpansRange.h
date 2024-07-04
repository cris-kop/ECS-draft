#include <span>
#include <tuple>

template<class T, class... Ts>
struct SpansRange 
{
    struct SpanItor
    {
        SpanItor& operator++()
        {
            ++index_;
            return *this;
        }
    
        SpanItor operator++(int) 
        {
            auto prev = *this;
            ++index_;
            return prev;
        }

        std::tuple<T&, Ts&...> operator*() 
        {
            return
            {
                std::get<std::span<T>>(range_->spans)[index_],
                std::get<std::span<Ts>>(range_->spans)[index_]...
            };
        }

        bool operator==(SpanItor const& other) const 
        {
            return other.index_ == index_ && other.range_ == range_;
        }

        bool operator!=(SpanItor const& other) const 
        {
            return !(other == *this);
        }

    private:
        friend struct SpansRange;

        SpanItor(SpansRange* range, size_t index) : range_(range), index_(index) {}

        SpansRange*     range_;
        size_t          index_;
    };

    //explicit SpansRange(std::span<T> t, std::span<Ts>... ts) : spans(t, ts...), size_(t.size()) {
    SpansRange(std::span<T> t, std::span<Ts>... ts) : spans(t, ts...), size_(t.size()) 
    { 
    }

    SpanItor begin() 
    {
        return SpanItor(this, 0);
    }

    SpanItor end() 
    {
        return SpanItor(this, size_);
    }

private:
    size_t size_{};
    std::tuple<std::span<T>, std::span<Ts>...> spans;
};