<div align="center"><h1>D++ top.gg</h1></div>

<div align="center"><img src="https://raw.githubusercontent.com/Henonicks/Guiding-Light-code/refs/heads/master/http_server/resources/dpptgg/DPPTGG_md.png" alt="DPPTGG logo" width=50% height=50%></div>

DPPTGG (D++ top.gg) is a C++-17+ library that wraps around [D++](https://github.com/brainboxdotcc/DPP)'s `http_server` class, helping its user listen to webhook requests from top.gg.

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

## Running the listener

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

Of course, running the listener separately from a cluster is possible. In fact, you don't even need any Discord functionality at all! This is perfect for those who need to listen for votes alongside a D++ bot as this library doesn't require any additional dependencies. Speaking of...

### Dependencies

* [D++](https://github.com/brainboxdotcc/DPP) 10.1.5+ (Not released as of me writing this but the current `dev` branch will suffice)
