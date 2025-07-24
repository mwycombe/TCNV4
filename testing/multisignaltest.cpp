#include <iostream>
#include <vector>
#include <climits>
#include <cstdint>
#include "Connections.h"
#include "Connection.h"
#include "Signal.h"
#include "SignalRingBuffer.h"
#include "Neurons.h"
#include "Neuron.h"

// *** temporary master clock for testing
extern int32_t masterClock;
extern int32_t globalNextEvent;
inline int32_t oldestEvent {0};

extern std::vector<connection::Connection> m_connPool;
extern int32_t currentConnectionSlot;
extern int32_t connectionPoolCapacity;

/* other externs used by connection*/
extern std::vector<signal::Signal> m_srb;
extern int32_t currentSignalSlot;
extern int32_t signalBufferCapacity;

extern std::vector<neuron::Neuron> m_neuronPool;
extern int32_t currentNeuronSlot;
extern int32_t neuronPoolCapacity;

namespace tconst = tcnconstants;

// Access objects for printing routings
conns::Connections    cObj = conns::Connections();
neurons::Neurons      nObj = neurons::Neurons();
srb::SignalRingBuffer sObj = srb::SignalRingBuffer();

int main  ()
{
  void prtN(int32_t);   // shorthand for print neuron by index.
  void outDetails(int32_t); // shorthand for print outgoing details.
  void inDetails(int32_t);  // shorthand for print incoming details
  int32_t nonRef(int32_t);  // look for non refractory neurons
  int32_t nonRNeurons{0};   // non refractory neurons count

  int32_t allocateASignalSlot(int32_t);  // ok to use during testing
  /**
   * @brief Test out with six originating connections and one
   * target connection
   * 
   * @param Create an eighth connection that is linked to all
   * six originating connections and delivers a signal to each
   * so they in turn create their signals.
   * 
   * @return Nothing
   * 
   * @throws Nothing
   * 
   * POP  Five of the connections will deliver signals of magnitude
   * 6000 which is the number needed in the aggregation window to
   * just not quite exceed cascade threshold. Thes connections must
   * deliver signals that are 1 clock tick apart.
   * Sixth connection will then deliver various amplitude signals
   * to the other five neurons until they reach the cascade threshold
   * in the aggregation window.
   * As each new signal is presented to see if aggregation works the
   * signals received by target neuron are monitored.
   * 
   * Success:
   * When last signal amplitude plus aggregation window contributions
   * exceeds cascade threshold the neuron connection emits a signal
   * that is enqued on the target neuron. Prior to that no signals
   * are enqued on the target neuron
   */

   // Amounts are designed to create enough heap allocation that
   // it shows in taskmanager but it won't show up until vectors
   // are populated.

   srb::SignalRingBuffer srb = srb::SignalRingBuffer(500000);
   conns::Connections connections = conns::Connections(10000000);
   neurons::Neurons neurons = neurons::Neurons(150000);





    // Indexes into pools
    int32_t cIdx;
    int32_t nIdx;
    int32_t sIdx;

    // Trackers for ranges
    int32_t connSlot{};
    int32_t neuronSlot{};
    int32_t signalSlot{};

    int32_t nextSignalSlot; // used for signal allocation


    std::cout << "Connections pool:= " << std::to_string(connectionPoolCapacity) << '\n';
    std::cout << "Neuron pool     := " << std::to_string(neuronPoolCapacity) << '\n';
    std::cout << "SRB pool        := " << std::to_string(signalBufferCapacity) << "\n\n";

    prtN(0);

    // create the various connections used
    // have them all point at the same target 
    // they all arrive one after the other inside the aggregation window

    connection::Connection conn0{ 9,0,1000,6000,0};
    connection::Connection conn1{ 9,0,999, 6000,0};
    connection::Connection conn2{ 9,0,998, 6000,0};
    connection::Connection conn3{ 9,0,997, 6000,0};
    connection::Connection conn4{ 9,0,996, 6000,0};

    // now add these to the conn pool
    // leave conn[0] as empty conn
    m_connPool[1] = conn0;
    m_connPool[2] = conn1;
    m_connPool[3] = conn2;
    m_connPool[4] = conn3;
    m_connPool[5] = conn4;


    // connection to inject a cascade signal into the source connection 
    // all are aimed at the same target
    connection::Connection conn10{ 9,0,10,6000,0};
    connection::Connection conn11{ 9,0,11,6000,0};
    connection::Connection conn12{ 9,0,12,6000,0};
    connection::Connection conn13{ 9,0,13,6000,0};
    connection::Connection conn14{ 9,0,14,6000,0};

    connection::Connection conn6{ 11,0,995, 188,0};   // this is the signal to cause cascade in target

    connection::Connection conn9{ 12,0,1200, 888,0};   // next signal emitted for immediate cascade

    m_connPool[6]   = conn6;  
    m_connPool[9]   = conn9;  
    m_connPool[10]  = conn10;
    m_connPool[11]  = conn11;
    m_connPool[12]  = conn12;
    m_connPool[13]  = conn13;
    m_connPool[14]  = conn14;

   // scan neurons lookin for non-refractory
   std::cout << "\nScan looking for non-refractory Neurons\n";
  // nonRNeurons =  nonRef(0);


  std::cout << "\nFound:= " << std::to_string(nonRef(0)) << " non-refractory neurons\n";

    // virgin neuron[0]
    std::cout << "\nEmpty neuron 0";
    prtN(0);

    std::cout << "\nZero outgoing slot in n[0]\n";
    cObj.printConnectionFromIndex(m_neuronPool[0].outgoingSignals[0]);

    std::cout << "\nNeuron outgoing before any pushes\n";
    for (int32_t cIdx : m_neuronPool[0].outgoingSignals)
    {
      cObj.printConnectionFromIndex(cIdx);
    }
    // originating connections to first node
    // all of these neurons have the same signal pointed at the same target [9]
    // n[0] when it cascades will produce a train of 5 signals all aimed at n[9]
    // n[9] should see 5 incoming signals with 1 clock tick of each other

    // Leave outsig[0] pointing to conn[0]
 
    m_neuronPool[0].outgoingSignals.push_back(1);
    m_neuronPool[0].outgoingSignals.push_back(2);
    m_neuronPool[0].outgoingSignals.push_back(3);
    m_neuronPool[0].outgoingSignals.push_back(4);
    m_neuronPool[0].outgoingSignals.push_back(5);

    m_neuronPool[9].outgoingSignals.push_back(9);
    m_neuronPool[10].outgoingSignals.push_back(10);
    m_neuronPool[11].outgoingSignals.push_back(11);

  // let's see the neuron structure.
    prtN(0); 
    outDetails(0);

    std::cout << "\nn[9] should be the target to cascade \nand create a cascade aimed at n[12]\n";

  // Make n[0] cascade and generate incoming on n[9]
  prtN(9);
  outDetails(9);

  // have to set this signal into srb as neuron incoming uses index into srb to retrieve signals
    nextSignalSlot = allocateASignalSlot(0);
    m_srb[nextSignalSlot].actionTime = masterClock;
    m_srb[nextSignalSlot].amplitude = tconst::cascadeThreshold + 1;
    m_srb[nextSignalSlot].owner = 0;  // this is where we are going to push this cascading signal
    m_srb[nextSignalSlot].testId = 0;

    m_neuronPool[0].incomingSignals.push_back(nextSignalSlot);
    // having set a signal my hand, we should update the nextEvent time for this neuron
    // vector.back() returns a reference to the last slot in the vector
    m_neuronPool[0].nextEvent = m_srb[ m_neuronPool[0].incomingSignals.back() ].actionTime;

    std::cout << "\nOpening MasterClock:= " << std::to_string(masterClock);
    globalNextEvent = INT32_MAX;  // Any signal will set this to a lower clock value

    
    std::cout << "\n\nMASTERCLOCK & GLOBALNEXTEVENT before we scan neurons:= " << std::to_string(masterClock) <<
              " : " << std::to_string(globalNextEvent);


    // ensure n[0] and n[9] are not marked refractory as this would prevent processing
    // If masterClock is at 0 this sets them to -1 which is before current master clock so they are non-refractory
    m_neuronPool[0].refractoryEnd = masterClock - 1;
    m_neuronPool[9].refractoryEnd = masterClock - 1;

    // all the connections in [0] point at n[9]

    std::cout << "\n\nJust before we call for the scan...";
    std::cout << "\nFound:= " << std::to_string(nonRef(0)) << " non-refractory neurons\n";

    std::cout << "\nneuron scan globalNextEvent:= " << std::to_string(globalNextEvent);

    nObj.scanNeuronsForSignals(); // this should scan the neurons and create incoming signals for n[9]

    inDetails(9);

  std::cout << "\nMASTERCLOCK & GLOBALNEXTEVENT after we scan neurons:= " << std::to_string(masterClock) <<
            " :" << std::to_string(globalNextEvent);


  // once we see the above we can than decide what clock time to generate the signal sent to n[9] at 1001                  

  // now examine the other neurons
    prtN(9);
    outDetails(9);
    prtN(10);
    outDetails(10);
    prtN(11);
    outDetails(11);

    std::cout << "\nmasterClock and globalNextEvent\n" << std::to_string(masterClock) <<
                  " : " << globalNextEvent;

  // std::cout << "\nOutgoing details for n[0]\n";

  // outDetails(0);  // outgoingSignal details for n[0]

  // cause 10 to cascade and send a signal to all connections on n[0]
  
  // target neuron
  //  char input;

  //  std::cout << "\nPress any key...";
  //  std::cin >> input;


} //end of main
void prtN(int32_t nIdx)
{
  //print this neuron by index
  nObj.printNeuronFromIndex(nIdx);
}
void outDetails(int32_t nIdx)
{
  for (int32_t cIdx : m_neuronPool[nIdx].outgoingSignals)
  {
    cObj.printConnectionFromIndex(cIdx);
  }
}
void inDetails(int32_t nIdx)
{
  for (int32_t sIdx : m_neuronPool[nIdx].incomingSignals)
  {
    sObj.printSignalFromIndex(sIdx);
  }
}
int32_t allocateASignalSlot(int32_t i)
{
  // fake unused input param
  // ok to use a function during testing
  // Relys on currentSignalSlot and signalBufferCapacity being global
  // SRB is different as it can wrap  
  // Initial value should be INT32_MAX to cause immediate wrap

  if (currentSignalSlot >= signalBufferCapacity) {
      currentSignalSlot = 0;
      return 0;
  }
  else { 
      return ++currentSignalSlot; 
  }  

}
int32_t nonRef(int32_t i)
{
  int32_t nonR{0};
  int32_t neuronIdx{-1};
  for (neuron::Neuron nref : m_neuronPool)
  { 
    ++neuronIdx;
    if (nref.refractoryEnd < 10000)
    {
      ++nonR;
      std::cout << "\nNeuronIdx:= " << std::to_string(neuronIdx);
      nObj.printNeuronFromRef(nref);
    }

  }
  return nonR;
}

