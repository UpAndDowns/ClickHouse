#pragma once

#include "ConnectionParameters.h"

#include <Client/Connection.h>
#include <IO/ConnectionTimeouts.h>
#include <common/LineReader.h>


namespace DB
{

namespace ErrorCodes
{
    extern const int UNKNOWN_PACKET_FROM_SERVER;
}

class Suggest : public LineReader::Suggest, boost::noncopyable
{
public:
    static Suggest & instance()
    {
        static Suggest instance;
        return instance;
    }

    void load(const ConnectionParameters & connection_parameters, size_t suggestion_limit);

    /// Older server versions cannot execute the query above.
    static constexpr int MIN_SERVER_REVISION = 54406;

private:
    Suggest();
    ~Suggest()
    {
        if (loading_thread.joinable())
            loading_thread.join();
    }

    void loadImpl(Connection & connection, const ConnectionTimeouts & timeouts, size_t suggestion_limit);
    void fetch(Connection & connection, const ConnectionTimeouts & timeouts, const std::string & query);
    void fillWordsFromBlock(const Block & block);

    /// Words are fetched asynchronously.
    std::thread loading_thread;
};

}
