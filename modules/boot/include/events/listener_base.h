#ifndef TPGZ_BOOT_LISTENERS_BASE_H
#define TPGZ_BOOT_LISTENERS_BASE_H

#include <rels/include/cxx.h>
#include <rels/include/defines.h>
#include <cstddef>
#include <boot/include/utils/containers/deque.h>

namespace events {
/**
 * @brief Listener class, made to produce listeners which can dispatch events to registered
 * functions.
 *
 * @tparam Callback The signature of the callback functions to register.
 */
template <typename Callback>
class ListenerBase {
public:
    ListenerBase() {}
    virtual ~ListenerBase() {}

    /**
     * @brief Registeres a listener to run at the listened event.
     *
     * @param listener Function to register.
     */
    void addListener(Callback* listener) { callbacks.push_back(listener); }

    /**
     * @brief Unregisters a function.
     *
     * @param listener Function to unregister
     * @return true The function was previously registered and has been removed.
     * @return false The function was not previously registered.
     */
    bool removeListener(Callback* listener) {
        for (typename tpgz::containers::deque<Callback*>::iterator it = callbacks.begin(); it != callbacks.end(); ++it) {
            if (*it == listener) {
                callbacks.erase(it);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Calls all the registered functions.
     */
    void dispatchAll() {
        for (typename tpgz::containers::deque<Callback*>::iterator it = callbacks.begin(); it != callbacks.end(); ++it) {
            (*it)();
        }
    }

    size_t getCount() const { return callbacks.size(); }

private:
    tpgz::containers::deque<Callback*> callbacks;
};

}  // namespace events

#endif