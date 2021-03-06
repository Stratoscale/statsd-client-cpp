
#ifndef STATSD_CLIENT_H
#define STATSD_CLIENT_H

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string>
#include <thread>
#include <condition_variable>
#include <deque>
#include <iostream>

namespace statsd {

#define DEFAULT_QUEUE_WAIT_MS 1000

struct _StatsdClientData;

class StatsdClient {
public:
    StatsdClient(const std::string& host="127.0.0.1", int port=8125, const std::string& ns = "", const bool batching = false, bool debug_=true);
    ~StatsdClient();

public:
    // you can config at anytime; client will use new address (useful for Singleton)
    void config(const std::string& host, int port, const std::string& ns = "");
    const char* errmsg();
    int send_to_daemon(const std::string & message);

public:
    int inc(const std::string& key, float sample_rate = 1.0);
    int dec(const std::string& key, float sample_rate = 1.0);
    int count(const std::string& key, double value, float sample_rate = 1.0);
    int gauge(const std::string& key, double value, float sample_rate = 1.0);
    int timing(const std::string& key, double ms, float sample_rate = 1.0);

public:
    /**
     * (Low Level Api) manually send a message
     * which might be composed of several lines.
     */
    int send(const std::string& message);

    /* (Low Level Api) manually send a message
     * type = "c", "g" or "ms"
     */
    int send(std::string key, double value,
             const std::string& type, float sample_rate);

protected:
    int init();
    void cleanup(std::string& key);
    void debug(const std::string & msg);

protected:
    struct _StatsdClientData* d;

    bool batching_;
    bool exit_;
    bool debug_;
    pthread_spinlock_t batching_spin_lock_;
    std::thread batching_thread_;
    std::deque<std::string> batching_message_queue_;
    std::condition_variable interrupting_cv;
    const uint64_t max_batching_size = 32768;
};

}; // end namespace

#endif
