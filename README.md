# Sonos

Quickly and easily control Sonos speakers from your Particle device. This library is a wrapper around the Sonos HTTP API. It allows you to control Sonos speakers on your network. You can play, pause, skip, and change the volume of your speakers. You can also get information about the currently playing track. 

## Usage

Connect your Particle hardware, add the Sonos library to your project and follow this simple example:

```
#include "Sonos.h"
Sonos sonos = Sonos();

void setup() {
  sonos.setIp("192.168.7.10");
}

void loop() {
  sonos.startPlayback();
  delay(1000);               
  
  sonos.pausePlayback();
  delay(1000);               
}
```

See the [examples](examples) folder for a more detailed example.

## Documentation

### Constructors

#### `Sonos()`
Instantiates a Sonos object.

#### `Sonos(String ipString)`
Instantiates a Sonos object with a server address. Same as calling [`setIp(String ipString)`](#setipstring-ipstring) on an instantiated Sonos object.

### Methods

#### `startPlayback()`
Start playback on connected device. Returns `void`.

#### `pausePlayback()`
Pause playback on connected device. Returns `void`.

#### `nextTrack()`
Skip ahead to the next track on connected device. Returns `void`. 

#### `previousTrack()`
Skip back to the previous track on connected device. Returns `void`. 

#### `setVolume(int volume)`
Set the volume of the connected device. Returns `void`.

#### `getVolume()`
Request the volume of the connected device. Successful request fires [`onVolume`](#onvolumecallbackinttype-callback) event. Returns `void`.

#### `getSongInfo()`
Request the currently playing song info of the connected device. Successful request fires [`onSongInfo`](#onsonginfocallbackinfotype-callback) event. Returns `void`.

#### `getPlaybackStatus()`
Request the playback status of the connected device. Successful request fires [`onPlaybackStatus`](#onplaybackstatuscallbackinfotype-callback) event. Returns `void`.

#### `setIp(String ipString)`
Set the server address of the device to connect to. Returns `void`.

### Events

#### `onVolume(CallbackIntType callback)`
Event fired after requesting volume using [getVolume](#getvolume). Handle by implementing your own callback and passing as paremeter.

Example:
```
void volumeHandler(int volume) {
  // Make use of volume
}
```

#### `onSongInfo(CallbackInfoType callback)`
Event fired after requesting song info using [getSongInfo](#getsonginfo). Handle by implementing your own callback and passing as paremeter.

Example:
```
void songInfoHandler(String songInfoJson) {
  // Make use of song info json.
  // Example: 
  // {
  //   "artist":"Taylor Swift",
  //   "title":"Bejewled",
  //   "album":"Midnights"
  // }
}
```

#### `onPlaybackStatus(CallbackInfoType callback)`
Event fired after requesting playback status using [getPlaybackStatus](#getplaybackstatus). Handle by implementing your own callback and passing as paremeter.

Example:
```
void playbackStatusHandler(String playbackStatus) {
  // Make use of playback status. 
}
```

## Contributing

Here's how you can make changes to this library and eventually contribute those changes back.

To get started, [clone the library from GitHub to your local machine](https://help.github.com/articles/cloning-a-repository/).

Change the name of the library in `library.properties` to something different. You can add your name at then end.

Modify the sources in <src> and <examples> with the new behavior.

To compile an example, use `particle compile examples/usage` command in [Particle CLI](https://docs.particle.io/guide/tools-and-features/cli#update-your-device-remotely) or use our [Desktop IDE](https://docs.particle.io/guide/tools-and-features/dev/#compiling-code).

After your changes are done you can upload them with `particle library upload` or `Upload` command in the IDE. This will create a private (only visible by you) library that you can use in other projects. Do `particle library add sonos_myname` to add the library to a project on your machine or add the sonos_myname library to a project on the Web IDE or Desktop IDE.

At this point, you can create a [GitHub pull request](https://help.github.com/articles/about-pull-requests/) with your changes to the original library. 

If you wish to make your library public, use `particle library publish` or `Publish` command.

## LICENSE
Copyright 2023 David Washington

Licensed under the [MIT license](LICENSE)
