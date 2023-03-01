# CS598Project1

For implementing the MWMR shared registers protocol, we use C++ and GPRC for communication between the clients and the replicas.

## Basic requirement for system
1. Provide a simple key-value store service to clients
   - store is a collection of registers
   - each register is identified by a key
   - both keys and valiues are simple strings
   - keys are 24 bytes and values are 10 bytes
2. Provide APIs
   - set(string key, string value)
   - get(string key) **NOTE** the replicas will return both value and timestamps
3. The communication layer is responsible for
   - send messages to multile replicas (five) in parallel from a client (asynchronous network)
   - block client operation until it receives a majority of responses from the replicas
   - Be able to support multiple concurrent writers and readers
   - Need to prevent data races (mutex lock based on key)
   - Client issues a request and wait until its completion before proceeding to the next
4. For the failures
   - NOT consider replica recovery and durability
   - NOT consider duplicate messages or reordering of the messages in the same stream (By default GPRC use TCP)

## Measurement and Evaluation
