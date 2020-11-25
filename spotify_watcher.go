package deej

import (
	"fmt"

	"github.com/godbus/dbus/v5"
	"go.uber.org/zap"
)

// SongMetaData store the information about the currently playing song
type SongMetaData struct {
	artists []string
	album   string
	title   string
}

// SpotifyWatcher provides infomation about the song currently being played
type SpotifyWatcher struct {
	conn         *dbus.Conn
	playerObject dbus.BusObject // dbus object
	logger       *zap.SugaredLogger
	currentSong  SongMetaData
}

// NewSpotifyWatcher creates and return a MediaWatcher struct
func NewSpotifyWatcher(logger *zap.SugaredLogger) (*SpotifyWatcher, error) {

	mw := &SpotifyWatcher{}

	mw.logger = logger

	// Connect to Dbus
	conn, err := dbus.SessionBus()
	if err != nil {
		return mw, err
	}
	mw.conn = conn

	// Get the propper Dbus Object to work on
	obj := conn.Object("org.mpris.MediaPlayer2.spotify", "/org/mpris/MediaPlayer2")
	mw.playerObject = obj

	return mw, nil
}

// GetSongMetadata returns a populated songMetaData struct
func (m *SpotifyWatcher) getSongMetadata() SongMetaData {
	data, err := m.playerObject.GetProperty("org.mpris.MediaPlayer2.Player.Metadata")

	if err != nil {
		fmt.Print("Error getting dbus object property")
	}

	dataMap := data.Value().(map[string]dbus.Variant)
	// Populate a SongMetaData struct with the data recieived
	// That looks ugly but i don't know what i'l doing, so yeah.
	songData := SongMetaData{
		album:   dataMap["xesam:album"].Value().(string),
		artists: dataMap["xesam:artist"].Value().([]string),
		title:   dataMap["xesam:title"].Value().(string),
	}

	return songData
}

// WatchMediaChange returns the metadata of the currently playing song when the
// there is a change in its properties
func (m *SpotifyWatcher) WatchMediaChange(ch chan SongMetaData) error {

	// Attach to dbus signal
	if err := m.conn.AddMatchSignal(
		dbus.WithMatchSender("org.mpris.MediaPlayer2.spotify"),
		dbus.WithMatchInterface("org.freedesktop.DBus.Properties"),
	); err != nil {
		fmt.Print(err)
		return err
	}

	c := make(chan *dbus.Signal, 10)
	m.conn.Signal(c)
	// Wait for a signal
	for {
		// On signal received, get new metadata and send it through channel
		<-c
		song := m.getSongMetadata()
		// Avoids msg spam from dbus
		if !(song.title == m.currentSong.title && song.artists[0] == m.currentSong.artists[0]) {
			m.currentSong = song
			ch <- m.currentSong
		}
	}
}
