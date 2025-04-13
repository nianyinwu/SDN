# Simulation of Switch and Controller in Software-Defined Networking (SDN)
 
This project simulates the interaction between switches and a centralized controller in a Software-Defined Networking (SDN) environment. 
The primary focus is on designing a Network Update Algorithm that dynamically computes and updates the routing paths for each switch in response to topology changes, both before and after they occur.

<!-- Input Parameters: 
- Number of Switches (Nodes): Total number of network switches.
- Number of Destinations: Distinct destination endpoints.
- Number of Links: Total links between nodes (edges in the graph).
- Initialization Time: Time required for initial topology setup and path calculation.
- Update Time: Time when the controller re-calculates and distributes new routing tables.
- Simulation Time: Total duration of the network simulation.
- Destination ID (DstID): ID of the destination node.
- Network Topology: A weighted directed/undirected graph representing links between nodes with associated costs (e.g., bandwidth, latency).
- Data Transmission Start Time: Time at which data packets begin to be transmitted.
- Source and Destination Pairs (SrcID, DstID): Each packet has a source and destination node.

Output (per event/log entry):
- Time: The timestamp when the event occurs.
- Receiver ID: The ID of the node receiving the packet.
- Packet ID: Unique identifier for the packet.
- Source ID: The originating node of the packet.
- Destination ID: The intended destination node.
- Previous Node ID: The node that forwarded the packet.
- Next Node ID: The next hop as determined by the controller's routing update.
- Packet Type: Indicates whether the packet is a data packet, control packet, or update-related. -->

