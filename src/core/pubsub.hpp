/** ========================================================================
 *
 * @file pubsub.hpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 * Idea took from:
 * https://gist.github.com/bosley/6e844b7d0f53e807fed0fd8c33828e80
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <iostream>
#include <string>
#include <mutex>
#include <queue>
#include <vector>

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
template <typename data_type> class event
{
private:
  data_type data;

public:
  event(data_type data)
  : data(data)
  {
  }

  data_type getData() const
  {
    return data;
  }
};

/* An interface for objects that want to 'be' subscribers */
template <typename data_type> class subscriber
{
protected:
  virtual void on_event(const yage::core::event<data_type> & event) = 0;

public:
  virtual void publish(const yage::core::event<data_type> & event)
  {
    on_event(event);
  }
};

template <typename data_type> class sync_subscriber : public subscriber<data_type>
{
private:
  std::queue<yage::core::event<data_type>> event_queue;
  std::mutex                               queue_mutex;

protected:
  virtual void on_event(const yage::core::event<data_type> & event) = 0;

  void process_events()
  {
    queue_mutex.lock();
    /* Get copy of all events in the queue */
    auto events = event_queue;
    /* Clear the queue */
    event_queue = std::queue<yage::core::event<data_type>>();
    queue_mutex.unlock();

    /* Process all events */
    while (!events.empty())
    {
      on_event(events.front());
      events.pop();
    }
  }

public:
  void publish(const yage::core::event<data_type> & event) override
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    event_queue.push(event);
  }
};

/* An object that published data to subscriber interfaces */
template <typename data_type> class publisher
{
private:
  /* We store pointers,
   * because we can't directly store a virtual object */
  std::vector<subscriber<data_type> *> subscribers;

public:
  void publish(const yage::core::event<data_type> & data)
  {
    /* Go through list of subscribers, and send them the data */
    for (auto & subscriber : subscribers)
    {
      subscriber->publish(data);
    }
  }

  /* Check how many subscribers the publisher has */
  size_t get_subscriber_count()
  {
    return subscribers.size();
  }

  /* We don't want to allow nullptrs, so we ensure the
   * existence of a subscriber by enforcing a reference. */
  void subscribe(subscriber<data_type> & subscriber)
  {
    log(LOG_DEBUG, "Subscribing %p to %p", &subscriber, this);

    /* Because we store pointers, and have a reference,
     * we need to make it a pointer so we add '&' */
    subscribers.push_back(&subscriber);
  }

  void unsubscribe(subscriber<data_type> & subscriber)
  {
    log(LOG_DEBUG, "Unsubscribing %p from %p", &subscriber, this);

    /* Remove the subscriber from the list */
    subscribers.erase(
          std::remove(subscribers.begin(), subscribers.end(), &subscriber),
          subscribers.end());
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
