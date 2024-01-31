# Virtual window demo

This is a demo application for the demonstration of smart triggers. It offers a virtual window that can be opened and closed via rest webhooks.

## Installation

Start the Docker containers with the following command:
```bash
docker compose up -d --build
```

## Usage

A websocket connection can be established via ws://localhost:8765 to obtain information about the opening status of the window.

You can open the window with POST http://localhost:5000/open and close the window again with POST localhost:5000/close.

The virtual window can be displayed at http://localhost:3000. The virtual window can be opened with a POST request to http://localhost:5000/open. It can be closed again with a POST request to http://localhost:5000/close.

## Configuration

The URLs of the API can be customised for deployment [here](frontend/src/environments/).