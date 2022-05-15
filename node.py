#!/usr/bin/env python3

import asyncio
import json
import websockets

import numpy as np

controller = None

async def send_ready():
    global controller
    await controller.send(json.dumps(
        { 'event' : 'ready' }
    ))

async def send_waiting():
    global controller
    await controller.send(json.dumps(
        { 'event' : 'waiting' }
    ))

async def send_frame(frame):
    global controller
    await controller.send(frame)

async def calculate(data):
    frame_index = data.get('frame_index')
    pixel_width = data.get('pixel_width')
    pixel_height = data.get('pixel_height')

    print('calculating frame', frame_index)

    array = np.empty([pixel_width, pixel_height], dtype = np.uint32)

    g = frame_index % 256

    f = np.vectorize(lambda x: np.frombuffer(bytes([g, g, g, 255]), dtype=np.uint32))

    frame = f(array).tobytes()

    await send_frame(frame)
    await send_waiting()

async def node():
    global controller

    async with websockets.connect('ws://localhost:8080/node') as websocket:

        print('controller connected')

        controller = websocket

        async for message in websocket:

            data = json.loads(message)

            if 'event' in data:

                event = data['event']

                if event == 'nudge':
                    await send_ready()

                elif event == 'calculate':
                    print('received calculate')
                    await calculate(data)

                elif event == 'transfer':
                    print('received transfer')
                    await send_ready()

asyncio.run(node())
