import scapy.all as scapy
import time
# Define the source and destination MAC addresses
src_mac = "98:43:FA:28:3C:14"
dst_mac = "00:12:2D:2D:12:00"

# Define the data payload
data_payload = input("Enter Payload to be sent")

# Create an Ethernet frame with the specified source and destination MAC addresses
eth_frame = scapy.Ether(src=src_mac, dst=dst_mac)

# Add the data payload to the frame
packet = eth_frame / data_payload
i =0

while(1):
    # Send the packet
    scapy.sendp(packet, iface="Ethernet 2")  # Replace with your network interface
    packet = eth_frame / (data_payload+str(i)+"\r\n")
    i=i+1
    time.sleep(2)