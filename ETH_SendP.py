import scapy.all as scapy

# Define the source and destination MAC addresses
src_mac = "98:43:FA:28:3C:14"
dst_mac = "00:12:2D:2D:12:00"

# Define the data payload
data_payload = "Ahmed Azazy"

# Create an Ethernet frame with the specified source and destination MAC addresses
eth_frame = scapy.Ether(src=src_mac, dst=dst_mac)

# Add the data payload to the frame
packet = eth_frame / data_payload

# Send the packet
scapy.sendp(packet, iface="Ethernet 2")  # Replace with your network interface