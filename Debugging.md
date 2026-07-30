## June 29: Buzzing noise and no motor movement (Resolved July 29)

Ran into an issue where there was an audible buzzing, and no motor movement when expected with input. After some tinkering, I discovered that the PWM (pulse-width modulation) set at 90 was too low to overcome static friction, and the default frequency of 1kHz was causing the motor windings to vibrate and a frequency audible to the human ear. I adjusted the PWM frequency to 140 and this solved the issue.

I now have a bug where one motor functions perfectly as expected, but the other is not responding to any inputs. I have two dual motor driver boards, so i tested between the two. I got slightly different results, but neither perfectly drove both motors. I still have to go through some more debugging, but at this point I am suspicious of the motor driver board.

*This has since been resolved on July 29th*

## July 7: Only one motor receiving power

Debugging extended video: [DriveWire V1 Debugging - Motor Power Issues](https://youtu.be/DzpZYJx54m0?si=vVjL2RTWoly1Th7c)

During previous tests, I confirmed that both DC motors function correctly, and that the 22 AWG automotive wire and soldered motor terminal connections are not the source of the issue.

The suspected failure point was the dual motor driver board. To isolate the issue, I tested whether the fault followed the ESP32 control inputs or stayed with the motor driver output channel.

Before debugging, only the left motor was receiving power. However, its polarity was reversed relative to the firmware command.

First, I swapped the motor driver input signals. After this change, the left motor still ran, but its polarity changed. The right motor remained off. This suggested that the ESP32 control signals were not the main issue, since changing the input mapping affected the working motor but did not restore the inactive motor.

Next, I swapped the motor outputs. After swapping the outputs, the left motor no longer worked, while the right motor began working with reversed polarity. This confirmed that the fault stayed with one output channel of the motor driver board, rather than following the motor, wiring, or ESP32 control signals.

I inspected the driver board visually, but did not find any obvious solder bridges or damaged connections. The issue may be internal to the H-bridge or motor driver IC.

As a temporary workaround, I added the backup dual motor driver board. This second board also appears to have only one fully functional output channel, so each board is now driving one motor using its working channel. Both boards are powered from the same motor supply rail, which was approximately 6.24 V during testing, and both boards share a common ground with the ESP32.

With this setup, both wheels now spin correctly, and the forward, reverse, and pulse commands function as intended.

### New bug discovered: 
The turning commands are currently producing incorrect behavior.

For the right turn command, R, the left wheel spins backward, which suggests its polarity or logic is inverted. However, the right wheel also spins backward.

For the left turn command, L, the right wheel still spins backward, while the left wheel spins forward. The left wheel behavior appears to be an inverted polarity issue, but the right wheel not changing direction between L and R suggests that there may also be a firmware mapping or command logic issue.

The next debugging step is to verify the motor direction mapping in firmware and create a simple truth table for each command, showing the expected and actual direction of each wheel.

| Command | Expected Left | Expected Right | Actual Left | Actual Right |
| ------- | ------------- | -------------- | ----------- | ------------ |
| F       | forward       | forward        | forward     | forward      |
| B       | backward      | backward       | backward    | backward     |
| L       | backward      | forward        | forward     | backward     |
| R       | forward       | backward       | backward    | backward     |



# July 29th: PWM Frequency Testing and Audible Motor Noise Fix
I investigated the audible beeping noise produced by both motors during low duty PWM operation. The initial hypothesis was that the default PWM carrier frequency was within the audible range, causing periodic current and torque ripple to mechanically excite the motor windings, housing, or gearbox.

The existing motor-control implementation used analogWrite(), which implicitly configured the ESP32-S3 LEDC peripheral at approximately 1 kHz with 8-bit duty resolution. I replaced this with explicit LEDC configuration using ledcSetup(), ledcAttachPin(), and ledcWrite().

During implementation, I determined that the project uses Arduino-ESP32 core 2.0.17. An earlier attempt had followed the newer Arduino-ESP32 3.x API, which uses functions such as ledcAttach(). That API was incompatible with the installed 2.x framework and caused the previous implementation to fail. Updating the code to use the correct version-specific LEDC functions resolved the configuration issue.

The 8-bit resolution was preserved so that existing motor commands remained on the same 0–255 duty scale.

To test the hypothesis, I held the motor duty command constant and increased the PWM frequency while listening for changes in the motor tone.

| PWM frequency | Observed result                                          |
| ------------: | -------------------------------------------------------- |
|         1 kHz | Clearly audible baseline tone                            |
|         2 kHz | Tone increased to approximately twice the original pitch |
|         4 kHz | Higher-pitched audible tone                              |
|         8 kHz | Audible, but significantly higher pitched                |
|        12 kHz | Faintly audible                                          |
|        17 kHz | Barely audible                                           |
|      17.5 kHz | Effectively unnoticeable during testing                  |
|        18 kHz | Effectively inaudible                                    |

The observed tone consistently increased with PWM frequency, strongly supporting the hypothesis that the noise was caused by PWM-driven electromechanical vibration.

A PWM frequency of **17.5 kHz** was selected as the current operating point because it was effectively inaudible during testing while remaining slightly below 18 kHz to avoid introducing unnecessary switching losses in the motor driver. Both motors were also tested at the maximum 8-bit duty command of `255`, and both operated correctly at the new frequency.

This change resolved the audible motor noise issue and replaced the implicit Arduino PWM configuration with an explicit, configurable motor-control implementation.

### Follow-up testing

Further testing will compare the minimum reliable motor-starting duty at 1 kHz and 17.5 kHz. This will help determine whether the higher switching frequency affects low-speed startup behaviour and whether a minimum-duty mapping or short startup-boost strategy is required.
