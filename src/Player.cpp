//
// Created by mike on 4/10/21.
//
#include <maxiSynths.h>
#include "maximilian.h"
#include "Player.h"

Player::Player(void** stream, double* output)
        :stream(stream), output(output) { }

Player::~Player()
{
    Pa_CloseStream(*stream);
}

// Return whether is playing.
bool Player::toggle()
{
    if (Pa_IsStreamActive(*stream)) {
        stop();
        return false;
    }
    else {
        start();
        return true;
    }
}

void Player::start() const { reportIfPaFail(Pa_StartStream(*stream)); }

void Player::stop()
{
    is_at_the_beginning = false;
    is_playing_a_note = false;
    note_count = 0;
    reportIfPaFail(Pa_StopStream(*stream));
}

maxiOsc myCounter, mySwitchableOsc;//
int CurrentCount;//
double myOscOutput, myCurrentVolume;//
maxiEnv myEnvelope;

void setup()
{//some inits

    //Timing is in ms

    myEnvelope.setAttack(5);
    myEnvelope.setDecay(50);  // Needs to be at least 1
    myEnvelope.setSustain(100);
    myEnvelope.setRelease(1000);

}

void Player::play(double* output)
{

    //notice that we feed in a value of 1. to create an envelope shape we can apply later.
    myCurrentVolume = myEnvelope.adsr(1., myEnvelope.trigger);

    CurrentCount = myCounter.phasor(1, 1, 9);//phasor can take three arguments; frequency, start value and end value.

    // You'll notice that these 'if' statements don't require curly braces "{}".
    // This is because there is only one outcome if the statement is true.
    if (is_playing_a_note) {
        if (is_at_the_beginning) {
            myEnvelope.trigger = 1; // Trigger the envelop
            cout << "Envelope Triggered!" << endl;
            is_at_the_beginning = false;
        }
    }
    else myEnvelope.trigger = 0;//release the envelope to make it fade out only if it's been triggered

    myOscOutput = mySwitchableOsc.sinewave(frequency);//one osc object can produce whichever waveform you want.


    output[0] = myOscOutput*myCurrentVolume;//left speaker
    output[1] = output[0];

}

void Player::noteOn(Note note)
{
    if (note!=prev_note) {
        cout << (char) note.letter << note.number << endl;
        ++note_count;
        cout << "count added: " << note_count << endl;
        prev_note = note;
        int scale[7] = {0, 2, 3, 5, 7, 8, 10};
        int relative_to_A_4 = scale[note.letter - 'A'] + 12*(note.number - 4);
        cout << relative_to_A_4;
        frequency = 440.0*pow(2.0, (relative_to_A_4/12.0));
        cout << " frequency " << frequency << endl;
        is_playing_a_note = true;
        is_at_the_beginning = true;
    }
}

void Player::noteOff()
{
    --note_count;
    cout << "count subtracted: " << note_count << endl;
    if (note_count <= 0) {
        note_count = 0;
        prev_note = {-1};
        is_playing_a_note = false;
    }
    is_at_the_beginning = false;
}
