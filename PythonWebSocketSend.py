import asyncio
import websockets
import json

# Fonction asynchrone pour gérer la connexion WebSocket
async def async_dmx_send(IP, channel, value):
    url = f"ws://{IP}/ws"
    async with websockets.connect(url) as ws:
        message = json.dumps({"channel": channel, "value": value})
        await ws.send(message)

def dmxSend(IP, channel, value):
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    try:
        loop.run_until_complete(async_dmx_send(IP, channel, value))
    finally:
        loop.close()

IPS = "192.168.1.36"

channel = 3

value = 150

dmxSend(IPS, channel, value)