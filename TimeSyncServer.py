# connect and disconnect is invoked in the bleak client context manager.

import asyncio
import struct
import time
from bleak import BleakScanner, BleakClient
from datetime import datetime, timezone

# UUIDs from your TimeService
SERVICE_UUID = "13371337-0000-4000-8000-133713371337"
CHAR_RX_UUID = "13371337-0001-4000-8000-133713371337"  # Write characteristic

async def run():
    print("Starting TimeService client...")
    
    while True:
        # Initial scan to find the device
        print("Scanning for TimeService...")
        devices = await BleakScanner.discover(timeout=5.0, service_uuids=[SERVICE_UUID])  # 5s timeout
        
        device_to_connect = None
        for device in devices:
            if SERVICE_UUID.lower() in (s.lower() for s in device.metadata.get("uuids", [])):
                device_to_connect = device
                print(f"Found TimeService device: {device.address}")
                break
        
        if device_to_connect:
            try:
                async with BleakClient(device_to_connect.address) as client:
                    print(f"Connected to {device_to_connect.address}")
                    
                    # Test write immediately
                    now = datetime.now(timezone.utc)
                    # Get timezone offset in seconds
                    tz_offset = int(datetime.now().astimezone().utcoffset().total_seconds())
                    time_payload = struct.pack('<Hiiiiii', now.year, now.month, now.day, now.hour, now.minute, now.second, tz_offset)
                    try:
                        await client.write_gatt_char(CHAR_RX_UUID, time_payload)
                        print("Test write succeeded")
                    except Exception as e:
                        print(f"Test write failed: {e}")
                    
                    # Stay connected until disconnection occurs naturally
                    while client.is_connected:
                        await asyncio.sleep(1)  # Poll connection state every second
                    
                    print(f"Disconnected from {device_to_connect.address}")
            
            except Exception as e:
                print(f"Connection error: {e}")
        
        else:
            print("No TimeService device found.")
        
        # Wait 10s before next scan if not connected
        await asyncio.sleep(10)

if __name__ == "__main__":
    asyncio.run(run())