# D++ top.gg listener

This is a C++ program that uses the D++ library to listen to requests, handling those that come from top.gg. Whenever top.gg tells it that someone has voted for my D++ bot, [Guiding Light](https://github.com/Henonicks/Guiding-Light-code), a message is sent via a Discord webhook so the bot can pick it up and reward its users.

## Running the server

Create a `config.hfg` in the root directory of the project and put this in:

```henifig
/TOPGG_WEBHOOK_LISTEN_IP\ | "(string) The IP to attach the top.gg server to. 0.0.0.0 will usually work and that's what I use personally."
/TOPGG_WEBHOOK_LISTEN_PORT\ | "(integer) The port to attach the top.gg server to."
/TOPGG_WEBHOOK_SECRET\ | "(string) Your top.gg webhook secret. It is displayed when you create a webhook or reset its secret on the dashboard. Starts with whs"
/TOPGG_WEBHOOK_LINK\ | "(string) Your Discord webhook link that will be used to send votes. Always needed if top.gg is enabled."
```

## Compilation

### Dependencies

This is designed to run alongside [Guiding Light](https://github.com/Henonicks/Guiding-Light-code), which is why the dependencies may seem overkill for the task. You don't need anything that the bot doesn't require, in fact, you don't need as much if this listener is all that you want to run.

* [D++](https://github.com/brainboxdotcc/DPP) 10.1.5+ (Not released as of me writing this but the current `dev` branch will suffice)

### Building

The program assumes it's placed inside a directory below the project's root directory. So make one:

    mkdir build
    cd build

Then generate a Makefile with cmake:

    cmake ..

You need to run the command above every time a change is made to `CMakeLists.txt`.

Now you're ready to compile:

    make -j

If DPP is installed in a different location you can specify the root directory to look in while running cmake

    cmake .. -DCMAKE_PREFIX_PATH=<your/path>

If you wish to use a non-default compiler, you can specify it while running cmake

    cmake .. -DCMAKE_CXX_COMPILER=<your_compiler>

## Running the server

That's it! You can simply do `./topgg_listener` and that will launch the listener.
