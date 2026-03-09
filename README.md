<div align="center"><h1>D++ top.gg</h1></div>

<div align="center"><img src="https://raw.githubusercontent.com/Henonicks/Guiding-Light-code/refs/heads/master/http_server/resources/dpptgg/DPPTGG_md.png" alt="DPPTGG logo" width=50% height=50%></div>

DPPTGG (D++ top.gg) is a C++-17+ library that wraps around [D++](https://github.com/brainboxdotcc/DPP)'s `http_server` class, helping its user listen to webhook requests from top.gg as well as make requests to its API, both the v0 and v1 endpoints.

## Setting up

This library uses CMake 3.15. In your CMakeLists.txt, add this subdirectory:
```cmake
add_subdirectory(dpptgg)
```
Then you need to link the library:
```cmake
target_link_libraries(<project name> dpptgg)
```

Now you can include the library in your code!

## Running

You can run both a listener and a poker, a class that pokes top.gg's API, at the same time, attached to the same cluster.

### Listener

First, include it:

```cpp
#include "dpptgg/topgg_listener.hpp"
```

Everything is located within the `dpptgg` namespace, including the listener. You can tie it to your `dpp::cluster` if you wish, otherwise the listener will heap-allocate one itself.

If you attach the listener to a cluster while the second is running, it will instantly start listening. Otherwise, you need to start the responsible cluster with `dpptgg::listener::start()`.

Here's an example utilising every feature on the surface of a listener that supports both bot and server votes:

```cpp
#include "dpptgg/topgg_listener.hpp"

int main() {
	dpp::cluster bot("Token gone, no trace, nowhere to be found. Maybe that's because there wasn't any to begin with.");

	bot.on_log(dpp::utility::cout_logger()); // Cluster-specific logs are handled with this

	dpptgg::listener listener("0.0.0.0", 6553,
        {"endpoint" : {"wsh_", "wsh_"}, "endpoint" : {"wsh_"}}, // Add as many secrets as you have. Multiple can be handled on the same endpoint.
            [](dpptgg::topgg_request const& request) {
                std::cout << "A " << (request.project_type == dpptgg::pt_bot ? "bot" : "server") << " vote has arrived!\n";
                std::cout << "The user's ID on Discord is: " << request.user_platform_id << std::endl;
                // By default, the response status will be 204 (No Content) here. You are free to change it to whatever.
            },
            [](dpptgg::non_topgg_request const& request) {
                std::cout << "Verification failed! Error code: " << request.status << std::endl;
                // Refer to the dpptgg::sender_identification_statuses enum. The error has also been logged.
            },
        &bot
	);

	listener.on_log([](dpp::log_t const& log) { // If this is set, non-cluster-specific logs are handled with this, otherwise bot.on_log is used.
		std::cout << log.message << ' ' << log.severity << std::endl;
	}); // dpp::utiltity::cout_logger() fits here as well.

	listener.start();
	// Starts the cluster and therefore itself.
	
	return 0;
}
```

We can also send requests instead of receiving them. The act of doing so is referred to as "poking" and is done via a poker. It works similarly to a listener, in that you can provide a cluster of your own or have the library allocate one for you. Yes, the poker also needs to be started with `dpptgg::poker::start()`, although if the underlying cluster is already running you can skip this step.

The poker's constructor accepts a token string and a `dpp::cluster*`. top.gg has endpoints for either both bots and servers or bots only to use. Use the correct functions with the correct tokens. The library assumes you know what you're doing, at least in that regard. Here's an example that gets votes for your project and, assuming it's a bot, updates the amount of servers it's in:

```cpp
#include "dpptgg/topgg_poker.hpp"

dpp::snowflake constexpr BOT_ID = 0; // your bot's user ID.

int main() {
    dpptgg::poker poker("top.gg token");
    
    poker.get_cluster()->on_log(dpp::utility::cout_logger());

    poker.post_server_count([](dpptgg::v0::request_completion_t const& callback) {
        std::cout << callback.request.status << std::endl;
    }, BOT_ID, 1);

    // Get the current date (time defaults to 00:00:00.0):
    auto const now = std::chrono::system_clock::now();
    auto const time_t = std::chrono::system_clock::to_time_t(now);    
    dpptgg::datetime start_date = {
        .year = static_cast <uint16_t>(1900 + std::localtime(&time_t)->tm_year),
        .month = static_cast <uint8_t>(std::localtime(&time_t)->tm_mon + 1),
        .day = static_cast <uint8_t>(std::localtime(&time_t)->tm_mday),
    };
    poker.get_votes(start_date, [&poker](dpptgg::v1::request_completion_t const& callback) {
        // When a response arrives, assuming it's not erroneuous, request another batch of votes using the pagination cursor we obtained from the API.
        poker.get_votes(callback.get <dpptgg::v1::requested_votes_t>().cursor, [](dpptgg::v1::request_completion_t const& inner_callback) {
            std::cout << inner_callback.get <dpptgg::v1::requested_votes_t>().data.size() << std::endl;
        });
    });

    poker.start();
    // The program will keep running until instructed otherwise.

    return 0;
}
```

Of course, running a client separately from a cluster is possible. The library will use a cluster but if you don't have a cluster of your own, one will be provided for you. In fact, you don't even need any Discord-specific functionality at all! This is perfect for those who need to listen for votes and/or update information about their project(s) alongside a D++ bot as this library doesn't require any additional dependencies. Speaking of...

### Dependencies/prerequisites

* [D++](https://github.com/brainboxdotcc/DPP) 10.1.5+ (Not released as of me writing this but the current `dev` branch will suffice) and its prerequisites
