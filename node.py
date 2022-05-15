#!/usr/bin/env python3

import asyncio
import json
import websockets

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
    print('calculating frame', frame_index)
    await send_frame(bytes(str(frame_index), 'utf-8'))
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
