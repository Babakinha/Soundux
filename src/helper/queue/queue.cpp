#include "queue.hpp"
#include "fancy.hpp"

namespace Soundux::Objects
{
    void Queue::handle()
    {
        std::unique_lock lock(queueMutex);
        while (!stop)
        {
            cv.wait(lock, [&]() { return !queue.empty() || stop; });
            while (!queue.empty())
            {
                auto front = std::move(*queue.begin());

                lock.unlock();
                front.second();
                lock.lock();

                queue.erase(front.first);
            }
        }
    }

    void Queue::push_unique(std::uint64_t id, std::function<void()> function)
    {
        {
            if (queue.find(id) != queue.end()){
                return;
            }
        }

        std::unique_lock lock(queueMutex);
        queue.emplace(id, std::move(function));
        lock.unlock();

        cv.notify_one();
    }

    void Queue::push(std::function<void()> function)
    {   
        function = function; // wow genius
        Fancy::fancy.logTime().warning() << "Queue::push Temporarily disabled" << std::endl;
    }

    Queue::Queue()
    {
        handler = std::thread([this] { handle(); });
    }
    Queue::~Queue()
    {
        stop = true;
        cv.notify_all();
        handler.join();
    }
} // namespace Soundux::Objects