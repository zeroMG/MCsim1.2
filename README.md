# MCsim
A Simulation Framework for DRAM Controllers

## Simulator Architecture
1. Simulation Engine
2. Requestor (Ex: Dumpy Memory Trace Reader)
3. AddressTranslator: map the logical address into physical location (rank, bankgroup, bank, row, column)
4. SystemQueues[Request]: buffers requests based on queue structure
5. RequestScheduler: selects schedulable requests from the request queues
6. CommmandGenerator: converts the scheduled requests into corresponding DRAM commands (bank row status)
7. SystemQueues[Commands]: buffers generated commands into queue structure
8. CommandScheduler: selects a single command from command queues which will be sent to DRAM device
9. MemoryDevice (Ex: Ramulator): the memory device that performs DRAM timing checking
10.Global tables: MemoryLevelTable + MemoryTimingTable + RequestorPriorityTable + BankOpenRowTable

The tables are accessable across all channels and controller components
RequestorPriorityTable keeps priority for individual requestor across different channels
MemoryLevelTable keeps memory hierarchy on individual channel
MemoryTimingTable keeps timing constraints checking on individual channel
BankOpenRowTable keeps checking the current and pending row that in or will be fetched to the bank row buffer

## How to Run MCsim
1. make
2. ex. ./MCsim -n 2 -c 1 -m Configuration.ini -t 1 -r MemTraces/a2time.trc


# Available Controllers
