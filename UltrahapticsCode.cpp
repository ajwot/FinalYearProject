// This example uses the Amplitude Modulation Emitter and a Leap Motion Controller to
// project a basic point onto a moving palm

#include <chrono>
#include <thread>

#include <Leap.h>
#include <UltrahapticsAmplitudeModulation.hpp>

#include <stdio.h> 
// To use time library of C 
#include <time.h> 

int main(int argc, char *argv[])
{
   
    // Create an emitter and a leap controller.
    Ultrahaptics::AmplitudeModulation::Emitter emitter;
    Leap::Controller leap_controller;
    // Load the appropriate Leap Motion alignment matrix for this kit
    Ultrahaptics::Alignment alignment = emitter.getDeviceInfo().getDefaultAlignment();

    // Set frequency to 200 Hertz and maximum intensity
    float frequency = 300.0f * Ultrahaptics::Units::hertz;
    float intensity = 1.0f;
    
    int i = 0;
    int pitch = 0;
    bool C;
    
    for (;;)
    {
        //Hand Tracking Code - the code below is from an example file in the Ultrahaptics SDK, Amplitude Modulation Handtracking
        // Get all the hand positions from the leap and position a focal point on each.
        Leap::Frame frame = leap_controller.frame();
        Leap::HandList hands = frame.hands();
        // Get the first visible hand.
        Leap::Hand hand = hands[0];

        // Translate the hand position from leap objects to Ultrahaptics objects.
        Leap::Vector leap_palm_position = hand.palmPosition();
        Leap::Vector leap_palm_normal = hand.palmNormal();
        Leap::Vector leap_palm_direction = hand.direction();

        // Convert to Ultrahaptics vectors, normal is negated as leap normal points down.
        Ultrahaptics::Vector3 uh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);
        Ultrahaptics::Vector3 uh_palm_normal(-leap_palm_normal.x, -leap_palm_normal.y, -leap_palm_normal.z);
        Ultrahaptics::Vector3 uh_palm_direction(leap_palm_direction.x, leap_palm_direction.y, leap_palm_direction.z);

        // Convert to device space from leap space.
        Ultrahaptics::Vector3 device_palm_position = alignment.fromTrackingPositionToDevicePosition(uh_palm_position);
        Ultrahaptics::Vector3 device_palm_normal = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_normal).normalize();
        Ultrahaptics::Vector3 device_palm_direction = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_direction).normalize();

        // These can then be converted to be a unit axis on the palm of the hand.
        Ultrahaptics::Vector3 device_palm_z = device_palm_normal;                             // Unit Z direction.
        Ultrahaptics::Vector3 device_palm_y = device_palm_direction;                          // Unit Y direction.
        Ultrahaptics::Vector3 device_palm_x = device_palm_y.cross(device_palm_z).normalize(); // Unit X direction.

        // Use these to create a point at 2cm x 2cm from the centre of the palm
        Ultrahaptics::Vector3 position = device_palm_position + (2 * Ultrahaptics::Units::cm * device_palm_x) + (2 * Ultrahaptics::Units::cm * device_palm_y);

        // Create the control point and emit.
        Ultrahaptics::ControlPoint point(position, intensity, frequency);
        //conditional works out where the hand lies and causes the emitter to emit a certain way
        //At these points, the C notes should lie, when the hand is at these positions C becomes true
        if ((leap_palm_position.y > 39 && leap_palm_position.y < 44) || (leap_palm_position.y > 89 && leap_palm_position.y < 93) || (leap_palm_position.y > 139 && leap_palm_position.y < 143) || (leap_palm_position.y > 191 && leap_palm_position.y < 195) || (leap_palm_position.y > 242 && leap_palm_position.y < 246) || (leap_palm_position.y > 293 && leap_palm_position.y < 297))
        {
            C = true;
        }
        else
        {
            C = false;
        }
       //If the hand is within an octave space then the pitch variable will have a certain value
        if (leap_palm_position.y > 39 && leap_palm_position.y < 91)
        {
            pitch = 100;
        }
       
        if (leap_palm_position.y > 91 && leap_palm_position.y < 143)
        {
            pitch = 75;
        }
        if (leap_palm_position.y > 143 && leap_palm_position.y < 194)
        {
            pitch = 50;
        }
        if (leap_palm_position.y > 194 && leap_palm_position.y < 246)
        {
            pitch = 25;
        }
        if (leap_palm_position.y > 246)
        {
            pitch = 5;
        }
        
        // the pitch variable determines how long the pulses last, a low pitch value causes the pulse frequency to be high
        // The device emits until i becomes greater than the pitch value
        if (!hands.isEmpty() && i < pitch)
        {
            
             // if the hand is not on a C note i will increase
             emitter.update(point);
             if (C == false)
             {
                 i++;
             }


        }
        else
        {
            // We can't see a hand, don't emit anything.
            //the device has stopped emitting, i is increased
            emitter.stop();
            if (i >= pitch)
            {
                i++;
            }
            //when i becomes greater than the pitch + 1.5*pitch then i will be reset, causing the device to emit again
            if (i >= (pitch+(pitch/1.5)))
            {
                i = 0;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
       
    }

    return 0;
}
