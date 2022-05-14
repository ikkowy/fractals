#!/usr/bin/env python3

import asyncio
import websockets

async def node():
    async with websockets.connect("ws://localhost:8080/node") as websocket:
        async for message in websocket:
            print(message)

asyncio.run(node())
