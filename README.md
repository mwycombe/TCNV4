# TCNV4 Overview
## July 2025, clone of TCNV3
### Purpose
This is yet another rewrite but using the new features in C++17 that supports shared global constants using "inline constexpr" which allows them to be included multiple times in multiple T(ranslation) U(nits) but have the linker reduce them all to a singular definition.

The next innovation is to use vectos for storing everything as vectors allocated the storage for their elements on the hep and TCN has some huge vectors - tens of millions of elements.

The new testing section is to support trying out verioius approaches to problems to find the optimum implentation. For this version of TCN, as before, element index number is preferred over pointers as X64 pointers are now 8 bytes long, whereas integers are limited to 4 bytes. With tens of millions of elements the savings are relevant and wherever possible performance will be preserved by using refs which are not stored as pointers.

### Basic Testing
The new directory is used of testing out various consructs and alos building a series of basic test cases to make sure the framework operates as expected.
As much as possible the tests are linked with the TCNConstants definitons so they represent how things will be in the real world. As needed overrides are provided to limit the size of the tests.

All testing the involves connections will have an impulse injection system that creates a connection event. The connection event will then provision a signal with the approprites parameters, log the signal, and, where appropriate, informa the target neuron of the incoming signal. The target neuron is responsible to just enqueue the signal. It will be processed later - maybe at the next clock tick.

1. **SRB testing**\
Basic test to make sure a small SRB acts as a ring buffer when receiving more requests than the size of the SRB.
A small SRB pool is constructed. A list of Signal allocations is presented with the Reserved field set to an increasing short int. 
The results show the SRB index number provisioned, and the incremental number in Reserved.

2. **Connection testing**\
A connection is created and connected to a target neuron. This test is for on connection and one target neuron.
A cascade impulse is presented to the connection which should provision a signal and deliver it with the correct time clock and amplitude to the target neuron.

3. **Multiple connection testing**\
Multiple connections are created and all connected to the same target neuron.
Each connection recieves a cascade impuse which is delivered to the same target neuron. The test is ensure that the signals are faithfully enqueued by the neuron.
Each connection is given a different temporal distance from the target neuron so we can identify each one.

4. **Setting neuron refractory**\
Set up a neuron with multiple incoming connections. Have connections create enough signals with enough amplituded to cause the neuron to cascade.
The refractory end time should be the youngext connection time clock plus the refractory period. This should be stored in the neuron.
Signals that continue to arrive during the refractory period should be ignored.

5. **Cascade causes neuron to signal connections**\
Repeat #4 but this time have connections assigned to the neuron. Ensure that when the neuron cascades it generates signals to its downstream connections.

6. **Build and test mini connection network**\
Create a cluster of neurons with a small netowrk of connections. Repeat the above test to make sure that injected impulses create the expected resultant outputs.
