# Canyon - Convolution Reverb with JUCE

### Description

Canyon is a convolution reverb VST plugin made with JUCE. This emerges from a computer engineering school project in which we wanted to develop an app that combined Acoustics and Computer Science.

![canyon-screenshot](https://github.com/cl3m3nt0n1/Canyon---Convolution-Reverb/assets/118217064/40c6e6cb-99c4-4103-8e4e-14964e610ae3)


During this project we recorded Impulse Responses of some of Le Mans' famous locations such as : Cathédrale St-Julien, Abbaye de l'Epau and others (mostly on campus). We share every one of them inside the IR.zip file and release them under the creative commons license.

### Features

**To use the plugin, you must first put the IR.zip extracted folder inside the common documents directory (e.g. on Windows : Users/Public/Public Documents/IR Files and on OSX : Users/Shared/IR Files).**

##### Import your own IR

Just drag&drop a .wav file or click the "Open File..." button and see its waveform.

##### A | B mode

You can define one IR for A and another one for B. Each one has its own set of parameters (filters and IR) and you can blend between A and B using the blend slider.

##### Filters

Canyon includes three different filters : Hi-pass, Low-pass and Bandpeak filter.
Hi-pass and Low-pass : pole, frequency.
Bandpeak : Q-Factor, Frequency, Gain.

### Notes

Since we developed the whole software on Windows we did not had the chance to test other OSes. As so, if you detect any issue please let us know.

If you are running Windows 64 bits you can just download the VST3 file.
Otherwise you must compile it yourself using the .projucer.

Start/Stop buttons are useless.
