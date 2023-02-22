# -*- coding: utf-8 -*-
"""
Copyright (C) 2020 Bosch Sensortec GmbH

SPDX-License-Identifier: BSD-3-Clause

Date : Aug 14, 2020

PC based tool for interacting with BLE Nordic UART service.
Works with BLE enabled COINES examples compiled for Application Board 3.0. 

Works with latest Bluetooth v4.0 USB dongles and recent notebook PCs with Bluetooth.
Tested with CSR8510 dongle in Windows 10 (Build 16299 and above) and Ubuntu 20.04 LTS
"""

import argparse
import asyncio
import aioconsole
import sys
import signal

from bleak import BleakClient
from bleak import discover

BLE_NUS_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
NUS_RX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
NUS_TX_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

BLE_GATT_WRITE_LEN = 20


# NUS data receive handler
def nus_data_rcv_handler(sender, data):
    sys.stdout.buffer.write(data)
    sys.stdout.flush()


def cleanup_handler(signum, frame):
    for task in asyncio.all_tasks():
        task.cancel()


async def run(address, loop, scan):
    if (scan == True):
        print("Scanning for devices...")
        devices = await discover()
        for dev in devices:
            print("%-20s \t %18s \t %+3d dB" % (dev.name, dev.address, dev.rssi))
        exit(0)

    async with BleakClient(address, loop=loop) as client:
        ble_nus_found = False
        connected = await client.is_connected()
        if connected is True:
            print("Connected to " + client.address)
            print("")

        for service in client.services:
            if service.uuid == BLE_NUS_UUID:
                ble_nus_found = True

        if ble_nus_found is True:
            await client.start_notify(NUS_TX_UUID, nus_data_rcv_handler)
        else:
            print("Nordic UART Service (NUS) not found ! Disconnecting ...")

        while True:
            try:
                line_input = await aioconsole.ainput()
                line_byte_arr = (line_input + '\n').encode()
                length = len(line_byte_arr)
                if length <= BLE_GATT_WRITE_LEN:
                    await client.write_gatt_char(NUS_RX_UUID, line_byte_arr)
                else:
                    for i in range(0, int(length / BLE_GATT_WRITE_LEN) + 1):
                        ble_gatt_byte_arr = line_byte_arr[i * BLE_GATT_WRITE_LEN:(i + 1) * BLE_GATT_WRITE_LEN]
                        await client.write_gatt_char(NUS_RX_UUID, ble_gatt_byte_arr)
                await asyncio.sleep(.1)
            except asyncio.CancelledError:
                await client.stop_notify(NUS_TX_UUID)
                break


if __name__ == "__main__":
    print("Application Board 3.0 BLE NUS Terminal")
    print("Bosch Sensortec GmbH (C) 2020")
    print("")
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument('-l', '--list', dest='scan', help='Scan for BLE devices', action='store_true')
    required_args = cmdline_parser.add_argument_group('required arguments')
    required_args.add_argument('-d', dest='device_mac_addr', help='Specify device MAC address')
    args = cmdline_parser.parse_args()
    devices = (args.device_mac_addr)
    scan = args.scan

    if args.scan is False and args.device_mac_addr is None:
        cmdline_parser.print_usage()
        exit(1)

    loop = asyncio.get_event_loop()
    signal.signal(signal.SIGINT, cleanup_handler)
    signal.signal(signal.SIGINT, cleanup_handler)
    loop.run_until_complete(run(devices, loop, scan))

