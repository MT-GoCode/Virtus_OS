# connect and disconnect is invoked in the bleak client context manager.

import asyncio
import struct
import time
from bleak import BleakScanner, BleakClient
from datetime import datetime

# UUIDs from your TimeService
SERVICE_UUID = "13371337-0000-4000-8000-133713371337"
CHAR_RX_UUID = "13371337-0001-4000-8000-133713371337"  # Write characteristic
CHAR_REQUEST_UUID = "13371337-0002-4000-8000-133713371337"  # Notify characteristic

async def notification_handler(sender, data):
    """Handle notifications from the request characteristic"""
    print(f"Received notification: {data.hex()}")
    
    # Get current time
    now = datetime.now()
    # Get timezone offset in seconds
    tz_offset = int(-time.timezone)  # Convert to positive offset from UTC
    
    # Pack time data into TimePayload format:
    # uint16_t year, int32_t month, day, hour, minute, second, tz_offset
    time_payload = struct.pack(
        '<Hiiiiii',  # Little-endian: H=uint16, i=int32
        now.year,
        now.month,
        now.day,
        now.hour,
        now.minute,
        now.second,
        tz_offset
    )
    
    # Connect and write the time payload
    async with BleakClient(sender.address) as client:
        await client.write_gatt_char(CHAR_RX_UUID, time_payload)
        print(f"Wrote time: {now.strftime('%Y-%m-%d %H:%M:%S')} with offset {tz_offset}s")

async def run():
    print("Scanning for TimeService...")
    
    while True:
        devices = await BleakScanner.discover(service_uuids=[SERVICE_UUID])
        
        for device in devices:
            if SERVICE_UUID.lower() in (s.lower() for s in device.metadata.get("uuids", [])):
                print(f"Found TimeService device: {device.address}")
                
                try:
                    async with BleakClient(device.address) as client:
                        print(f"Connected to {device.address}")
                        
                        # Test write immediately
                        now = datetime.now()
                        tz_offset = int(-time.timezone)
                        time_payload = struct.pack('<Hiiiiii', now.year, now.month, now.day, now.hour, now.minute, now.second, tz_offset)
                        try:
                            await client.write_gatt_char(CHAR_RX_UUID, time_payload)
                            print("Test write succeeded")
                        except Exception as e:
                            print(f"Test write failed: {e}")

                    
                        
                except Exception as e:
                    print(f"Connection error: {e}")
                
                await asyncio.sleep(5)
        
        await asyncio.sleep(5)

if __name__ == "__main__":
    asyncio.run(run())