//
// Created by mike on 4/10/21.
//
#include <maxiSynths.h>
#include "maximilian.h"
#include "Player.h"

class voice {
private:
    maxiOsc osc;
    maxiEnv env;
    double freq;
    Player::Note current_note;

public:
    voice();
    double output();

    void on(const Player::Note& note);
    void off();
};

double voice::output()
{
    return env.adsr(osc.sinewave(freq), env.trigger);
}

voice::voice()
{
    env.setAttack(5);
    env.setDecay(50);  // Needs to be at least 1
    env.setSustain(100);
    env.setRelease(1000);
}

void voice::on(const Player::Note& note)
{
    env.trigger = 1;
    freq = Player::noteToFrequency(note);
}

void voice::off()
{
    env.trigger = 0;
}

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
    is_playing_a_note = false;
    note_count = 0;
    reportIfPaFail(Pa_StopStream(*stream));
}

maxiOsc mySwitchableOsc;//
double myOscOutput, myCurrentVolume;//
maxiEnv myEnvelope;
voice myVoice;

void setup()
{//some inits

    //Timing is in ms

    myEnvelope.setAttack(500);
    myEnvelope.setDecay(50);  // Needs to be at least 1
    myEnvelope.setSustain(100);
    myEnvelope.setRelease(1000);
}

void Player::play(double* output)
{

    /*
    //notice that we feed in a value of 1. to create an envelope shape we can apply later.
    myCurrentVolume = myEnvelope.adsr(1., myEnvelope.trigger);

    // You'll notice that these 'if' statements don't require curly braces "{}".
    // This is because there is only one outcome if the statement is true.
    if (is_playing_a_note) {
            myEnvelope.trigger = 1; // Trigger the envelop
            cout << "Envelope Triggered!" << endl;
    }
    else myEnvelope.trigger = 0;//release the envelope to make it fade out only if it's been triggered

    myOscOutput = mySwitchableOsc.sinewave(frequency);//one osc object can produce whichever waveform you want.

     */
    output[0] = myVoice.output();//left speaker
    output[1] = output[0];

}

double Player::noteToFrequency(const Note& note)
{
    int scale[7] = {0, 2, 3, 5, 7, 8, 10};
    int relative_to_A_4 = scale[note.letter - 'A'] + 12*(note.number - 4);
    return 440.0*pow(2.0, (relative_to_A_4/12.0));
}

void Player::note_on(const Note& note)
{
    myVoice.on(note);
    /*
    if (note!=prev_note) {
        cout << (char) note.letter << note.number << endl;
        ++note_count;
        cout << "count added: " << note_count << endl;
        prev_note = note;
        frequency = noteToFrequency(note);
        cout << " frequency " << frequency << endl;
        is_playing_a_note = true;
    }
     */
}

void Player::note_off(const Note& note)
{
    myVoice.off();
    /*
    --note_count;
    cout << "count subtracted: " << note_count << endl;
    if (note_count<=0) {
        note_count = 0;
        prev_note = {-1};
        is_playing_a_note = false;
    }
     */
}
