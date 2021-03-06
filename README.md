MUDI2000

College group project completed in spring 2017 as part of Microprocessor Systems
The name comes from how moody the controller was during a phase of development - we were getting random inputs due to contact bounce on certain multiplexer inputs

Brief overview:
MIDI Controller with 30 programmable buttons - used to play whatever kind of music/sounds/general racket you can imagine. Arduino polls buttons and sends pre-determined MIDI messages to computer. Computer then plays desired sounds. Huge amount of flexibility, software on computer side can be used to play whatever sounds are desired, through any number of instruments and effects.

More detailed overview:
MUDI 2000 is an Arduino UNO based MIDI controller, similar in function and appearance to a Novation Launchpad, only on a smaller scale and sadly without the light show. 
The controller connects to a computer for power and to send MIDI messages - on its own it has no way of producing sound. It sends the computer MIDI note on and note off messages, with information on the pitch of the note (high or low), the volume and the channel. The pitch, volume and channel serve to identify the note. If no note off message follows a note on message the computer software will sustain the note. Channels are used to allow easy separation of instruments on the computer e.g. having a column of buttons for percussion, another for bass guitar etc.
MIDI messages consist of 3 bytes of information. A status byte containing an event identification (note on, note off etc.) and the note channel (0.15) is first, followed by two data bytes; the pitch of the note (0-127) and the velocity (volume) of the note (0-127).
The physical case was 3D printed in our Maker's Space. Simple push-to-make buttons were used as inputs, with bottle caps glued on to provide a larger contact area and make using the controller easier. 30 buttons were multiplexed through four 8-bit multiplexers (74LS151), with 47K Ohm pull-down resistors on all multiplexer inputs.
Even though we were sending MIDI messages, we wanted to use a standard USB cable from the Arduino to the computer, rather than wiring a separate MIDI female port to the Arduino and using a MIDI to USB cable, as well as the USB cable to power the Arduino. As such, we used a program called Hairless MIDI serial bridge to map our USB port to a virtual MIDI port. Hairless MIDI allowed effective debugging as it displayed the serial data the Arduino was outputting through the USB.
We used Reaper as our Digital Audio Workstation to receive input from this virtual MIDI port and to actually play sounds and do something with the input from the controller. Different instruments can be used for different channels such as guitar, drums - if you can name it you can play it.
We also mapped the lower 5 buttons of the controller to control functions within Reaper (begin looping play/pause etc.). These are all on their own channel to avoid interfering with any actual note playing.


