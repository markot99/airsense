# AirSense Alexa Skill

This repository contains the source code for the Alexa skill for querying the current air quality values.

## Setup

1. Log in [here](https://developer.amazon.com/de-DE/alexa/alexa-skills-kit) with your Amazon account

    ![](assets/login_page.png)

2. In the next step, click on Create skill.

    ![](assets/create_skill.png)

3. Give the skill a name and select your desired language.

    ![](assets/skill_name.png)

4. In the next step, select the custom model.

    ![](assets/custom_model.png)

5. And choose Python as the programming language

    ![](assets/programming_language.png)

6. Select the template from scratch.
    ![](assets/template.png)

7. Check everything again and click on create skill.
    ![](assets/review.png)

8. You should now be on the start page of your skill.
    ![](assets/homepage.png)

9. Paste the code of the [interactionModel.json](interactionModel.json) in the Intents Json Editor and click on build skill. Set your security token instead of "SET YOUR TOKEN HERE". You will receive your token from the backend when you log in.
    ![](assets/intents.png)

10. Paste the source code from the [lambda](lambda/) folder into the Code tab and click on Deploy.
    ![](assets/code.png)

11. Activate the skill for testing on the test page. And you're ready to go.
    ![](assets/test.png)