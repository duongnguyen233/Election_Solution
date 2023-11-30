#include <iostream>
#include <fstream>
using namespace std;

// remove comment below to get execution trace output
// #define TRACEON

// ----------------------------------- structures and functions for events

enum eventType {
    customer_arrival = 0, service_completion
};

const char *eventTypeLabels[] = {"arrival", "service"};

struct event {
    eventType type;
    float time;
    float arrival;
    float serviceDuration;
    float enteredQueue;
    int priority;
    float queueDuration;
    int assignedServer;
};

event createEvent(float, float, int, eventType);

// ----------------------------------- class for customer arrival queue

/*
 * customer arrival queue implemented as a max heap
 * priority queue keyed on event.priority, event.time
 */
class ArrivalQueue {

    static const int QUEUE_MAX = 500; // max queue size
    event arrivals[QUEUE_MAX];

    int qLength; // number of customers in the queue
    int qMaxLength;  // maximum number customers ever in the queue

public:

    void init();

    void addQ(event);

    event removeQ();

    bool isQEmpty();

    int maxQueueLength();

    void showQueue();

private:

    void siftUp(int);

    void siftDown(int);
};

void ArrivalQueue::init() {

    qLength = 0;
    qMaxLength = 0;
}

void ArrivalQueue::addQ(event ev) {

   if(qLength >= QUEUE_MAX){
	exit(1);
}

    arrivals[qLength] = ev;
    ++qLength;

    siftUp(qLength - 1);

    qMaxLength = max(qMaxLength, qLength);
}

event ArrivalQueue::removeQ() {

    event ev;

  if(qLength <= 0) {
		exit(1);
	}

    ev = arrivals[0];

    arrivals[0] = arrivals[qLength - 1];
    --qLength;

    siftDown(0);

    return ev;
	
}

bool ArrivalQueue::isQEmpty() {

    return (qLength == 0);
}

int ArrivalQueue::maxQueueLength() {

    return qMaxLength;
}

void ArrivalQueue::siftUp(int current) {

    if (current == 0) {
        return;
    }

    int p = (current - 1) / 2;

    if (arrivals[p].priority > arrivals[current].priority) {

        return;

    } else if (arrivals[p].priority == arrivals[current].priority) {

        if (arrivals[p].arrival > arrivals[current].arrival) {

            event temp = arrivals[current];
            arrivals[current] = arrivals[p];
            arrivals[p] = temp;

            siftUp(p);

        } else {
            // do nothing
        }

    } else {  // arrivals[p].priority < arrivals[current].priority

        event temp = arrivals[current];
        arrivals[current] = arrivals[p];
        arrivals[p] = temp;

        siftUp(p);

    }
}

void ArrivalQueue::siftDown(int current) {

    int child = 2 * current + 1;

    if (child >= qLength)
        return;

    if ((child + 1) < qLength) { // find the smallest child for comparison with parent

        if (arrivals[child].priority < arrivals[child + 1].priority) {
            child++;
        } else if (arrivals[child].priority == arrivals[child + 1].priority) {

            if (arrivals[child].arrival > arrivals[child + 1].arrival) {
                child++;
            }
        }

    }

    if (arrivals[current].priority < arrivals[child].priority) {

        event temp = arrivals[current];
        arrivals[current] = arrivals[child];
        arrivals[child] = temp;

        siftDown(child);

    } else if (arrivals[current].priority == arrivals[child].priority) {

        if (arrivals[current].arrival > arrivals[child].arrival) {

            event temp = arrivals[current];
            arrivals[current] = arrivals[child];
            arrivals[child] = temp;

            siftDown(child);

        }
    }
}

/*
 * top to bottom list
 */
void ArrivalQueue::showQueue() {

    int i;

    if (!isQEmpty()) {
        cout << "Arrival Queue" << endl;
        for (i = 0; i < qLength; ++i) {
            cout << "i: " << i
                 << " priority: " << arrivals[i].priority
                 << " time: " << arrivals[i].time
                 << " arrival: " << arrivals[i].arrival
                 << " serviceDuration: " << arrivals[i].serviceDuration
                 << endl;
        }
    } else {
        cout << "Arrival Queue is Empty" << endl;
    }

    cout << endl;
}

// ----------------------------------- class for event queue

/*
 * event queue implemented as a min heap keyed on event.time
 */
class EventQueue {

    event *events;
    int actualE; // number of events in the queue
    int expectedE; // expected number of active events

public:

    void init(int);

    void addE(event);

    event removeE();

    eventType getNextEventType();

    bool isEmpty();

    void showEventQueue();

private:

    void siftDown(int);

    void siftUp(int);
};

void EventQueue::init(int expected) {

    expectedE = expected;
    events = new event[expectedE];
    actualE = 0;

}

void EventQueue::addE(event ev) {

	if(actualE >= expectedE){
  		exit(1);
	}


    events[actualE] = ev;
    ++actualE;

    siftUp(actualE - 1);
}


event EventQueue::removeE() {

    event ev;

    if(actualE <= 0){
	exit(1);
		}

    ev = events[0];

    events[0] = events[actualE - 1];
    --actualE;

    siftDown(0);

    return ev;
}

bool EventQueue::isEmpty() {

    return (actualE == 0);
}

eventType EventQueue::getNextEventType() {

    return events[0].type;
}

void EventQueue::siftUp(int current) {

    if (current == 0) {
        return;
    }

    int p = (current - 1) / 2;

    if (events[p].time < events[current].time) {
        return;
    } else {

        event temp = events[current];
        events[current] = events[p];
        events[p] = temp;

        siftUp(p);
    }
}

void EventQueue::siftDown(int current) {

    int child = 2 * current + 1;

    if (child >= actualE)
        return;

    if ((child + 1) < actualE) { // find the smallest child for comaprison with parent
        if (events[child].time > events[child + 1].time) {
            child++;
        }
    }

    if (events[current].time > events[child].time) {

        event temp = events[current];
        events[current] = events[child];
        events[child] = temp;

        siftDown(child);
    }
}

/*
 * expected size of the event queue is small
 * show...() is simple top to bottom list
 * reader needs to know heap property to interpret
 */
void EventQueue::showEventQueue() {

    int i;

    if (actualE > 0) {
        cout << "Event Queue" << endl;
        for (i = 0; i < actualE; ++i) {
            cout << "i: " << i
                 << " time: " << events[i].time
                 << " arrival: " << events[i].arrival
                 << " duration: " << events[i].serviceDuration
                 << " priority: " << events[i].priority
                 << " type: " << eventTypeLabels[events[i].type]
                 << endl;
        }
    } else {
        cout << "Event Queue Empty" << endl;
    }
    cout << endl;
}

// ----------------------------------- class for server pool
/*
 * servers assigned using a simple round robin policy
 */
class ServerPool {

    enum status {
        busy, idle
    };


    struct state {
        status current; // whether idle or busy
        int served; // number of customers served
        float idle; // time idle
        float lastFinish; // time busy
    };

    state *servers;
    int serverCount; // number of servers
    int serversBusy; // number of busy servers at given time
    int nextServer; // next server to be assigned a customer

public:

    void init(int);

    int assign(float, float);

    void release(int);

    bool isIdleServer();

    int numServed(int);

    float idleTime(int);

    void showServerPool();

};

void ServerPool::init(int serverC) {

    serverCount = serverC;
    servers = new state[serverCount];
    serversBusy = 0;
    nextServer = 0;

    for (int i = 0; i < serverCount; i++) {
        servers[i].current = idle;
        servers[i].served = 0;
        servers[i].idle = 0.0;
        servers[i].lastFinish = 0.0;
    }
}

int ServerPool::assign(float thisStart, float thisDuration) {

    int assignedServer;

    if(serversBusy >= serverCount){
		exit(1);
		}

    for (int i = 0; i < serverCount; ++i) { // query next idle server
        if (servers[nextServer].current == idle) {
            assignedServer = nextServer;
            break;
        } else {
            nextServer = (nextServer + 1) % serverCount;
        }
    }

    ++servers[nextServer].served;
    servers[nextServer].idle = servers[nextServer].idle + (thisStart - servers[nextServer].lastFinish);
    servers[nextServer].lastFinish = thisStart + thisDuration;
    servers[nextServer].current = busy;

    ++serversBusy;

    nextServer = (nextServer + 1) % serverCount;

    return assignedServer;
}

void ServerPool::release(int assignedServer) {

    if(serversBusy <= 0){
		exit(1);
		}

    servers[assignedServer].current = idle;

    --serversBusy;
}

bool ServerPool::isIdleServer() {

    return (serversBusy != serverCount);
}

int ServerPool::numServed(int serverId) {

    return servers[serverId].served;
}

float ServerPool::idleTime(int serverId) {

    return servers[serverId].idle;
}

void ServerPool::showServerPool() {

    int i;

    if (serversBusy > 0) {
        cout << "Server Pool" << endl;
        for (i = 0; i < serverCount; ++i) {
            cout << "i: " << i << " time: " << servers[i].current << endl;
        }
    } else {
        cout << "Server Pool Idle" << endl;
    }
    cout << endl;
}


// ----------------------------------- support functions


// ----------------------------------- functions to help with problem resolution

void logInfo(int, string);

void showEventShort(event);

// ----------------------------------- global data

int main() {

    // file details
    char filename[20];
    ifstream fin;

    int serverC; // number of servers for simulation

    // input event data
    float arrivalTime;
    float serviceTime;
    int priority;

    bool moreEvents;
    bool moreCustomers = true;

    // core data structures
    ArrivalQueue arrivalQ;
    EventQueue eventQ;
    ServerPool serverP;

    event ev;

    // statistics
    int customerCount = 0;
    float totalServiceTime = 0.0;
    float totalQueueingTime = 0.0; // total time waiting in queue

    float sim_time = 0.0;

    // get simulation inputs
    cerr << "Please enter the number of tellers: ";
    cin >> serverC;

    cerr << "Please enter the name of the input file: ";
    cin >> filename;

    fin.open(filename);
    if (!fin) {
        cerr << "Error opening file " << filename << ". Program exit." << endl;
        return 0;
    }

    // show simulation inputs
    cout << endl << "Simulation Inputs" << endl;
    cout << "Number of tellers: " << serverC << endl;
    cout << "Name of input file: " << filename << endl << endl;

    // create core data structures
    arrivalQ.init();
    eventQ.init(serverC + 1);
    serverP.init(serverC);

    // create initial customer arrival event and place in event queue
    // assume that input file is non-empty, that is, at least one arrival event present
    fin >> arrivalTime >> serviceTime >> priority;
    ++customerCount;

#ifdef TRACEON
    logInfo(__LINE__, "from input add " + to_string(arrivalTime) + " " + to_string(serviceTime) + " " + to_string(priority));
#endif

    eventQ.addE(createEvent(arrivalTime, serviceTime, priority, customer_arrival));

#ifdef TRACEON
    eventQ.showEventQueue();
#endif

    // do simulation while the following conditions hold
    moreEvents = moreCustomers or !eventQ.isEmpty();

    while (moreEvents) {

        if (eventQ.getNextEventType() == service_completion) {

#ifdef TRACEON
            logInfo(__LINE__, "service completion event");
#endif

            ev = eventQ.removeE();
            sim_time = ev.time;

#ifdef TRACEON
            logInfo(__LINE__, "sim time: " + to_string(sim_time));
#endif

            totalServiceTime = totalServiceTime + ev.serviceDuration;
            totalQueueingTime = totalQueueingTime + ev.queueDuration;

            serverP.release(ev.assignedServer);  // make server idle

            if (!arrivalQ.isQEmpty()) { // customer arrival priority queue has entry -> assign to server

                ev = arrivalQ.removeQ();
                ev.type = service_completion;
                ev.time = sim_time + ev.serviceDuration;
                ev.queueDuration = sim_time - ev.enteredQueue;
                ev.assignedServer = serverP.assign(sim_time, ev.serviceDuration);
                eventQ.addE(ev);

            } else {

                // adjust event queue
            }

        } else if (eventQ.getNextEventType() == customer_arrival) { // next event is customer_arrival

#ifdef TRACEON
            logInfo(__LINE__, "customer arrival event");
#endif

            ev = eventQ.removeE();
            sim_time = ev.time;

#ifdef TRACEON
            logInfo(__LINE__, "sim time: " + to_string(sim_time));
            showEventShort(ev);
#endif

            ev.enteredQueue = sim_time;
            arrivalQ.addQ(ev);

            if (serverP.isIdleServer()) { // there is an idle server; make it busy

                // remove next item from priority queue and assign to idle server
                ev = arrivalQ.removeQ();
                ev.type = service_completion;
                ev.time = sim_time + ev.serviceDuration;
                ev.queueDuration = sim_time - ev.enteredQueue;
                ev.assignedServer = serverP.assign(sim_time, ev.serviceDuration);
                eventQ.addE(ev);

            } else { // no idle server

                // nothing further to do
            }

            // get next arrival event from the input and add to the event queue
            fin >> arrivalTime >> serviceTime >> priority;

#ifdef TRACEON
            logInfo(__LINE__, "from input add " + to_string(arrivalTime) + " " + to_string(serviceTime) + " " + to_string(priority));
#endif

            if (moreCustomers) {
                if ((arrivalTime > 0.0) or (serviceTime > 0.0)) { // new customer in input file
                    ++customerCount;
                    eventQ.addE(createEvent(arrivalTime, serviceTime, priority, customer_arrival));
                }  else { // no more customers in input file
                    moreCustomers = false;
                }
            }

        }

#ifdef TRACEON
        logInfo(__LINE__, "End Simulation Step");
        eventQ.showEventQueue();
        arrivalQ.showQueue();
#endif

        moreEvents = moreCustomers or !eventQ.isEmpty();
    }

    // Output the required statistics
    cout << endl;
    cout << "Simulation Statistics" << endl;
    cout << "Customers Served by Each Teller" << endl;
    for (int i = 0; i < serverC; ++i) {
        cout << "Teller [" << i << "]: " << serverP.numServed(i) << endl;
    }
    cout << "Total Time of Simulation: " << sim_time << endl;
    cout << "Average Service Time per Customer: " << (totalServiceTime / customerCount) << endl;
    cout << "Average Waiting Time per Customer: " << (totalQueueingTime / customerCount) << endl;
    cout << "Maximum Length of the Queue: " << arrivalQ.maxQueueLength() << endl;
    cout << "Average Length of the Queue: " << (totalQueueingTime / sim_time) << endl;
    cout << "Average Idle Rate of Each Teller" << endl;
    for (int i = 0; i < serverC; ++i) {
        cout << "Teller [" << i << "]: " << (serverP.idleTime(i) / sim_time) << endl;
    }

    fin.close();

    return 0;
}

// ----------------------------------- support functions

// ----------------------------------- functions to help with problem resolution

/*
 * simple display function with line number and message
 * invoke with preprocessor macro __LINE__ eg logInfo(__LINE__, "message text");
 */
void logInfo(int lineNo, string message) {
    cout << "[" << lineNo << "]" << " " << message << endl;
}

// ----------------------------------- functions to help with events

event createEvent(float arrival, float duration, int priority, eventType type) {

    event ev;

    ev.time = arrival;

    ev.arrival = arrival;
    ev.serviceDuration = duration;
    ev.priority = priority;

    ev.queueDuration = 0.0;

    ev.type = type;

    return ev;
}

/*
 * show short form of event
 */
void showEventShort(event ev) {
    cout << "time: " << ev.time
         << " arrival: " << ev.arrival
         << " duration: " << ev.serviceDuration
         << " priority: " << ev.priority
         << " type: " << eventTypeLabels[ev.type]
         << endl;
}