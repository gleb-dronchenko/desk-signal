#pragma once

#include "shared_reading.hpp"

// the network task starts the HTTP server and listens for requests.
void StartNetwork(SharedReading *shared);
