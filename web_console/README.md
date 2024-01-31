# AirSense Web Console

Welcome to the source code of the *AirSense* web console!

## Overview

*AirSense* is a robust and scalable solution designed to track and manage temperature, pressure, and humidity data from smart home devices. This backend system provides a centralized platform for collecting, storing, and analyzing environmental metrics, offering real-time insights into the conditions within smart homes.

## Local Deployment

To deploy the application locally, including all current changes, use the provided `docker-compose.yml` file. Make sure to rebuild the *AirSense* container to include the latest changes:

```sh
docker compose up -d --build
```

To shutdown the local deployment use:

```sh
docker compose down
```
