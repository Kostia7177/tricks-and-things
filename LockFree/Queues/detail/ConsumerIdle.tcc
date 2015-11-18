
namespace TricksAndThings { namespace detail
{

template<class F>
void ConsumerIdle::until(F f)
{
    std::unique_lock<std::mutex> locker(lock);
    awaiting = true;
    check.wait(locker, f);
    awaiting = false;
}

} }
