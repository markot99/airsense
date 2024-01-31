import logging

import ask_sdk_core.utils as ask_utils
from airsense_request_handler import (
    AirSenseRequestHandler,
    DeviceNotFoundError,
    RequestFailedError,
)
from ask_sdk_core.dispatch_components import (
    AbstractExceptionHandler,
    AbstractRequestHandler,
)
from ask_sdk_core.handler_input import HandlerInput
from ask_sdk_core.skill_builder import SkillBuilder
from ask_sdk_model import Response

logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

request_handler = AirSenseRequestHandler(
    "https://<URL>/api/v1",
    "<SET YOUR TOKEN HERE>",
)

AIRSENSE_PRONOUNCIATION = "<lang xml:lang='en-GB'>AirSense</lang>"


class NoDeviceNameError(Exception):
    """Raised when the device name is not found in the alexa intent."""


def get_device_name(handler_input):
    """
    Get the device name from the alexa intent.

    Parameters:
        handler_input : ask_sdk_core.handler_input.HandlerInput
            The input from the alexa request.

    Returns:
        str
            The name of the device.
    """
    device_name = handler_input.request_envelope.request.intent.slots[
        "devicename"
    ].value

    if (device_name is None) or (device_name == ""):
        raise NoDeviceNameError

    return device_name


class LaunchRequestHandler(AbstractRequestHandler):
    """Handler for Skill Launch."""

    def can_handle(self, handler_input):
        return ask_utils.is_request_type("LaunchRequest")(handler_input)

    def handle(self, handler_input):
        speak_output = (
            "Willkommen bei "
            + AIRSENSE_PRONOUNCIATION
            + ", deiner smarten Wetterstation. Wie kann ich dir weiterhelfen?"
        )
        return (
            handler_input.response_builder.speak(speak_output)
            .ask(speak_output)
            .response
        )


class TemperatureIntentHandler(AbstractRequestHandler):
    """Handler for the temperature intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("TemperatureIntent")(handler_input)

    def handle(self, handler_input):
        devicename = get_device_name(handler_input)
        speak_output = (
            "Die aktuelle Temperatur beträgt "
            + request_handler.get_temperature(devicename)
            + " Grad Celsius."
        )

        return handler_input.response_builder.speak(speak_output).response


class HumidityIntentHandler(AbstractRequestHandler):
    """Handler for the humidity intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("HumidityIntent")(handler_input)

    def handle(self, handler_input):
        devicename = get_device_name(handler_input)
        speak_output = (
            "Die aktuelle Luftfeuchtigkeit beträgt "
            + request_handler.get_humidity(devicename)
            + " Prozent."
        )

        return handler_input.response_builder.speak(speak_output).response


class PressureIntentHandler(AbstractRequestHandler):
    """Handler for the pressure intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("PressureIntent")(handler_input)

    def handle(self, handler_input):
        devicename = get_device_name(handler_input)
        speak_output = (
            "Der aktuelle Luftdruck beträgt "
            + request_handler.get_pressure(devicename)
            + " Pascal."
        )

        return handler_input.response_builder.speak(speak_output).response


class GasResistanceIntentHandler(AbstractRequestHandler):
    """Handler for the gas resistance intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("GasResistanceIntent")(handler_input)

    def handle(self, handler_input):
        devicename = get_device_name(handler_input)
        speak_output = (
            "Der aktuelle Gaswiderstand beträgt "
            + request_handler.get_gas_resistance(devicename)
            + " Ohmen."
        )

        return handler_input.response_builder.speak(speak_output).response


class AirQualityIntentHandler(AbstractRequestHandler):
    """Handler for the air quality intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("AirQualityIntent")(handler_input)

    def handle(self, handler_input):
        devicename = get_device_name(handler_input)
        data = request_handler.get_air_quality_data(devicename)
        speak_output = (
            "Aktuell sind es "
            + data.temperature
            + " Grad Celsius bei einer Luftfeuchtigkeit von "
            + data.humidity
            + " Prozent und einem Luftdruck von "
            + data.pressure
            + " Pascal bei einem Gaswiderstand von "
            + data.gas_resistance
            + " Ohmen."
        )

        return handler_input.response_builder.speak(speak_output).response


class HelpIntentHandler(AbstractRequestHandler):
    """Handler for Help Intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("AMAZON.HelpIntent")(handler_input)

    def handle(self, handler_input):
        speak_output = "Frage mich einfach nach der aktuellen Temperatur, Luftfeuchtigkeit, Luftdruck, Gaswiderstand oder für alle Informationen nach der Luftqualität eines gewünschten Sensors."

        return (
            handler_input.response_builder.speak(speak_output)
            .ask(speak_output)
            .response
        )


class CancelOrStopIntentHandler(AbstractRequestHandler):
    """Single handler for Cancel and Stop Intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("AMAZON.CancelIntent")(
            handler_input
        ) or ask_utils.is_intent_name("AMAZON.StopIntent")(handler_input)

    def handle(self, handler_input):
        speak_output = "Bis bald!"

        return handler_input.response_builder.speak(speak_output).response


class FallbackIntentHandler(AbstractRequestHandler):
    """Single handler for Fallback Intent."""

    def can_handle(self, handler_input):
        return ask_utils.is_intent_name("AMAZON.FallbackIntent")(handler_input)

    def handle(self, handler_input):
        logger.info("In FallbackIntentHandler")
        speak_output = "Hmm, das weiß ich leider nicht. Frage mich einfach nach der aktuellen Temperatur, Luftfeuchtigkeit, Luftdruck, Gaswiderstand oder für alle Informationen nach der Luftqualität eines gewünschten Sensors."

        return (
            handler_input.response_builder.speak(speak_output)
            .ask(speak_output)
            .response
        )


class SessionEndedRequestHandler(AbstractRequestHandler):
    """Handler for Session End."""

    def can_handle(self, handler_input):
        return ask_utils.is_request_type("SessionEndedRequest")(handler_input)

    def handle(self, handler_input):
        return handler_input.response_builder.response


class IntentReflectorHandler(AbstractRequestHandler):
    """The intent reflector is used for interaction model testing and debugging.
    It will simply repeat the intent the user said. You can create custom handlers
    for your intents by defining them above, then also adding them to the request
    handler chain below.
    """

    def can_handle(self, handler_input):
        return ask_utils.is_request_type("IntentRequest")(handler_input)

    def handle(self, handler_input):
        intent_name = ask_utils.get_intent_name(handler_input)
        speak_output = "Du hast gerade den Intent " + intent_name + " aufgerufen."

        return handler_input.response_builder.speak(speak_output).response


class CatchAllExceptionHandler(AbstractExceptionHandler):
    """Generic error handling to capture any syntax or routing errors. If you receive an error
    stating the request handler chain is not found, you have not implemented a handler for
    the intent being invoked or included it in the skill builder below.
    """

    def can_handle(self, handler_input, exception):
        return True

    def handle(self, handler_input, exception):
        logger.error(exception, exc_info=True)

        speak_output = "Leider ist ein unerwarteter Fehler aufgetreten. Bitte versuche es später erneut."

        if isinstance(exception, DeviceNotFoundError):
            speak_output = "Leider kenne ich kein Gerät mit diesem Namen."
        elif isinstance(exception, RequestFailedError):
            speak_output = (
                "Leider konnte ich keine Verbindung mit der "
                + AIRSENSE_PRONOUNCIATION
                + " API herstellen. Bitte versuche es später erneut."
            )
        elif isinstance(exception, NoDeviceNameError):
            speak_output = "Bitte nenne den Namen des Sensors bei deiner Anfrage um Daten abzufragen."

        return (
            handler_input.response_builder.speak(speak_output)
            .ask(speak_output)
            .response
        )


sb = SkillBuilder()

# launch handler
sb.add_request_handler(LaunchRequestHandler())

# custom handlers
sb.add_request_handler(TemperatureIntentHandler())
sb.add_request_handler(HumidityIntentHandler())
sb.add_request_handler(PressureIntentHandler())
sb.add_request_handler(AirQualityIntentHandler())
sb.add_request_handler(GasResistanceIntentHandler())

# default handlers
sb.add_request_handler(HelpIntentHandler())
sb.add_request_handler(CancelOrStopIntentHandler())
sb.add_request_handler(FallbackIntentHandler())
sb.add_request_handler(SessionEndedRequestHandler())
sb.add_request_handler(IntentReflectorHandler())

# exception handlers
sb.add_exception_handler(CatchAllExceptionHandler())

lambda_handler = sb.lambda_handler()
