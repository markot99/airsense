from dataclasses import dataclass

import requests


class RequestFailedError(Exception):
    """Raised when the request to the AirSense API fails."""


class DeviceNotFoundError(Exception):
    """Raised when the device name is not found in the response."""


@dataclass
class AirQualityData:
    """The air quality data of a device."""
    temperature: str
    humidity: str
    pressure: str
    gas_resistance: str


class AirSenseRequestHandler:
    """Handles requests to the AirSense API."""

    def __init__(self, base_url, token):
        """
        Initialize the handler.

        Parameters:
            base_url : str
                The base url of the AirSense API.
            token : str
                The token to authenticate with the AirSense API.
        """
        self.base_url = base_url
        self.token = token

    def get_temperature(self, device_name: str) -> str:
        """
        Get the temperature of the device with the given name.

        Parameters:
            device_name : str
                The name of the device to get the temperature of.

        Returns:
            str
                The temperature of the device.
        """
        return self.get_air_quality_data(device_name).temperature

    def get_humidity(self, device_name: str) -> str:
        """
        Get the humidity of the device with the given name.

        Parameters:
            device_name : str
                The name of the device to get the humidity of.

        Returns:
            str
                The humidity of the device.
        """
        return self.get_air_quality_data(device_name).humidity

    def get_pressure(self, device_name: str) -> str:
        """
        Get the pressure of the device with the given name.

        Parameters:
            device_name : str
                The name of the device to get the pressure of.

        Returns:
            str
                The pressure of the device.
        """
        return self.get_air_quality_data(device_name).pressure

    def get_gas_resistance(self, device_name: str) -> str:
        """
        Get the gas resistance of the device with the given name.

        Parameters:
            device_name : str
                The name of the device to get the gas resistance of.

        Returns:
            str
                The gas resistance of the device.
        """
        return self.get_air_quality_data(device_name).gas_resistance

    def float_to_string(self, value: float) -> str:
        """
        Convert a float to a string that can be spoken by Alexa.

        Parameters:
            value : float
                The value to convert to a string.

        Returns:
            str
                The string representation of the value.
        """
        # convert the float to a string with 2 decimal places
        value_str = "{:.2f}".format(value)
        # remove trailing zeros (and the decimal point if there are no decimal places)
        return value_str.rstrip("0").rstrip(".")

    def int_to_string(self, value: int) -> str:
        """
        Convert an int to a string that can be spoken by Alexa.

        Parameters:
            value : int
                The value to convert to a string.

        Returns:
            str
                The string representation of the value.
        """
        # convert the int to a string
        # add tags to prevent Alexa from spelling out the number wrong
        return '<say-as interpret-as="cardinal">' + str(value) + "</say-as>"

    def get_air_quality_data(self, device_name: str) -> AirQualityData:
        """
        Get the air quality data of the device with the given name.

        Parameters:
            device_name : str
                The name of the device to get the air quality data of.

        Returns:
            AirQualityData
                The air quality data of the device.
        """
        response = requests.get(
            self.base_url + "/sensors/latest",
            headers={"Authorization": "Bearer " + self.token},
            timeout=5,
        )

        if response.status_code != 200:
            raise RequestFailedError

        for data in response.json():
            if data["device"].casefold() == device_name.casefold():
                print(data)
                return AirQualityData(
                    self.float_to_string(data["temperature"]),
                    self.float_to_string(data["humidity"]),
                    self.int_to_string(data["pressure"]),
                    self.int_to_string(data["gasResistance"]),
                )

        # if the device name is not found in the response, raise an error
        raise DeviceNotFoundError
