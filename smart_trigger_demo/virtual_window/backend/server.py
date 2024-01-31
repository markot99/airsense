import asyncio
import threading

import waitress
import websockets
from flask import Flask

app = Flask(__name__)

# set of connected clients
clients = set()

# global variables
# main_event_loop is used to schedule tasks from other threads
main_event_loop = None

# window_open is used to keep track of the window state
window_open = False


@app.route("/open", methods=["POST"])
def handle_open():
    """Handle open window request"""
    print("Received open")
    global window_open, main_event_loop
    if not window_open:
        window_open = True
        main_event_loop.call_soon_threadsafe(asyncio.create_task, broadcast_messages())
    return ("", 204)


@app.route("/close", methods=["POST"])
def handle_close():
    """Handle close window request"""
    global window_open, main_event_loop
    if window_open:
        window_open = False
        main_event_loop.call_soon_threadsafe(asyncio.create_task, broadcast_messages())

    return ("", 204)


async def broadcast_messages():
    """Broadcast the window state to all connected clients"""
    global window_open
    message = '{"windowOpened": true}' if window_open else '{"windowOpened": false}'
    websockets.broadcast(clients, message)


async def handler(websocket):
    """Handle new websocket connection, add it to the set of connected clients and send initial message"""
    clients.add(websocket)
    global window_open
    message = '{"windowOpened": true}' if window_open else '{"windowOpened": false}'
    await websocket.send(message)
    await websocket.wait_closed()
    clients.remove(websocket)


async def main():
    """Main function, start the websocket server and the flask server"""
    global main_event_loop
    main_event_loop = asyncio.get_event_loop()
    async with websockets.serve(handler, "", 8765):
        threading.Thread(
            target=waitress.serve, args=(app,), kwargs={"host": "0.0.0.0", "port": 5000}
        ).start()
        await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())
