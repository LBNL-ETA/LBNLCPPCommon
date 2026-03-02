#pragma once

#include <functional>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <future>

namespace lbnl
{

    template<typename Key, typename Value>
    class LazyEvaluator
    {
    public:
        using Generator = std::function<Value(const Key &)>;

        explicit LazyEvaluator(Generator generator) : m_Generator(std::move(generator))
        {}

        const Value & operator()(const Key & key)
        {
            return get(key);
        }

        const Value & get(const Key & key)
        {
            // Fast path: check if already computed (shared/read lock)
            {
                std::shared_lock readLock(m_Mutex);
                auto iter = m_Cache.find(key);
                if(iter != m_Cache.end())
                {
                    return iter->second.get();
                }
            }

            // Slow path: need to compute (exclusive/write lock)
            std::unique_lock writeLock(m_Mutex);

            // Double-check after acquiring write lock
            auto iter = m_Cache.find(key);
            if(iter != m_Cache.end())
            {
                return iter->second.get();
            }

            // Create promise/future, insert future into cache
            std::promise<Value> prom;
            auto fut = prom.get_future().share();
            auto [inserted, _] = m_Cache.emplace(key, fut);

            // Release lock before computing
            writeLock.unlock();

            // Compute and fulfill the promise
            prom.set_value(m_Generator(key));

            return inserted->second.get();
        }

    private:
        Generator m_Generator;
        std::unordered_map<Key, std::shared_future<Value>> m_Cache;
        std::shared_mutex m_Mutex;
    };

}   // namespace lbnl
