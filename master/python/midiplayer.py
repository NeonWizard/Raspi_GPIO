import sys
import mido
mido.set_backend("mido.backends.pygame")

import serialcomm
import playertools

def handleRaw(msg, sd): # sd = song data
	if isinstance(msg, mido.MetaMessage) or not hasattr(msg, "note"): # Filter out meta messages
		return

	# Get the channel it's on and convert to floppy track
	if msg.channel not in sd["TRACK_KEY"]: return
	#track = sd["TRACK_KEY"].index(msg.channel)
	tracks = [i for i, x in enumerate(sd["TRACK_KEY"]) if x == msg.channel]

	# Determine if it's a NOTE_ON or NOTE_OFF
	state = msg.type=="note_on" and msg.velocity > 0

	# Convert the message to a floppy note then transpose
	note = msg.note-46
	note += sd["TRANSPOSE"] # Transposition

	# Capping on either side of the range
	if note > 31:
		note = 20+((note-32)%12)
	if note <= 0:
		# Fix this later
		print "Jaspar"
		note = abs(note)%12

	# If it isn't playable by the florchestra
	#if note <= 0 or note > 31: return
	if note <= 0: return
	if note > 31:
		print note
		return

	for track in tracks:
		if state:
			serialcomm.sendNote(note, track)
		else:
			serialcomm.sendNote(0, track)

if len(sys.argv) != 2:
	print "Invalid argument count!"
	sys.exit()

print "Getting song info..."
songdata = playertools.readSongData("../songs/"+sys.argv[1])
print "Reading MIDI file into memory..."
songfile = mido.MidiFile("../songs/"+songdata["MIDI_NAME"]+".mid")

print "Ready to begin."

for message in songfile.play():
	handleRaw(message, songdata)

