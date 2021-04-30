#include "KbiWindow.h"
#include "maximilian.h"
#include "Player.h"
#include <iostream>
#include <portaudio.h>
#include <gtkmm/application.h>

int routing(const void* inputBuffer,
        void* outputBuffer,
        unsigned long nBufferFrames,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags status,
        void* myPlayer);

//This is main()
int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("walrus.kbi");
    PaError err;
    err = Pa_Initialize();
    reportIfPaFail(err);

    double data[maxiSettings::channels];
    PaStream* stream;
    Player player(&stream, data);
    err = Pa_OpenDefaultStream(&stream,
            0,          /* no input channels */
            maxiSettings::channels,          /* stereo output */
            paFloat32,  /* 64 bit floating point output */
            maxiSettings::sampleRate,
            maxiSettings::bufferSize,        /* frames per buffer, i.e. the number
												   of sample frames that PortAudio will
												   request from the callback. Many apps
												   may want to use
												   paFramesPerBufferUnspecified, which
												   tells PortAudio to pick the best,
												   possibly changing, buffer size.*/
            &routing, /* this is your callback function */
            &player); /*This is a pointer that will be passed to
										 your callback*/

    //PaAlsa_EnableRealtimeScheduling(stream,true);
    reportIfPaFail(err);
    return app->make_window_and_run<KbiWindow, Player*>(argc, argv, &player);
}

void reportIfPaFail(PaError err)
{
    if (err != paNoError)
        cout << "PortAudio error: " << Pa_GetErrorText(err) << endl;
}

int routing(const void* inputBuffer,
        void* outputBuffer,
        unsigned long nBufferFrames,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags status,
        void* myPlayer)
{

    unsigned int i, j;

    auto* buffer = (float*) outputBuffer;
    auto player = (Player*) myPlayer;
    double* lastValues = player->output;
//	double currentTime = (double) streamTime; Might come in handy for control
    if (status)
        cout << "Stream underflow detected!" << endl;
    for (i = 0; i < nBufferFrames; i++) {
    }
// Write interleaved audio data.
    for (i = 0; i < nBufferFrames; i++) {
        player->play();
        for (j = 0; j < maxiSettings::channels; j++) {
            *buffer++ = lastValues[j];
        }
    }
    return 0;
}