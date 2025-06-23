#pragma once

#include <functional>
#include <unordered_map>
#include <mutex>

namespace lbnl
{

    template<typename Key, typename Value>
    class LazyEvaluator
    {
    public:
        using Generator = std::function<Value(const Key &)>;

        explicit LazyEvaluator(Generator generator) : m_Generator(std::move(generator))
        {}

        Value operator()(const Key & key)
        {
            return get(key);
        }

        const Value & get(const Key & key)
        {
            std::lock_guard lock(m_Mutex);
            auto [it, inserted] = m_Cache.try_emplace(key);
            if(inserted)
            {
                it->second = m_Generator(key);
            }
            return it->second;
        }

    private:
        Generator m_Generator;
        std::unordered_map<Key, Value> m_Cache;
        std::mutex m_Mutex;
    };

}   // namespace lbnl
