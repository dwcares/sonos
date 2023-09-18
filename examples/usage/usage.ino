// Example usage for sonos library by David Washington.

#include "Sonos.h"

String ip = "0.0.0.0";

Sonos sonos = Sonos(ip);

void setup() {
    Particle.function("setIp", setIp);
    Particle.function("startPlayback", startPlayback);
    Particle.function("pausePlayback", pausePlayback);
    Particle.function("nextTrack", nextTrack);
    Particle.function("previousTrack", previousTrack);
    Particle.function("setVolume", setVolume);
    Particle.function("getVolume", getVolume);
    Particle.function("getSongInfo", getSongInfo);
    Particle.function("getPlaybackStatus", getPlaybackStatus);
    
    sonos.onVolume(volumeHandler);
    sonos.onSongInfo(songInfoHandler);
    sonos.onPlaybackStatus(playbackStatusHandler);
}

void loop() {

}

void volumeHandler(int volume) {
    Particle.publish("volume", String(volume));
}

void songInfoHandler(String songInfo) {
    Particle.publish("songInfo", songInfo);
}

void playbackStatusHandler(String playbackStatus) {
    Particle.publish("playbackStatus", playbackStatus);
}

int startPlayback(String nil) {
    sonos.startPlayback();
    return 1;
}

int pausePlayback(String nil) {
    sonos.pausePlayback();
    return 1;
}

int nextTrack(String nil) {
    sonos.nextTrack();
    return 1;
}

int previousTrack(String nil) {
    sonos.previousTrack();
    return 1;
}

int setVolume(String sonosVolume) {
     sonos.setVolume(sonosVolume.toInt());
     return 1;
}

int getVolume(String nil) {
    sonos.getVolume();
    return 1;
}

int getSongInfo(String nil) {
    sonos.getSongInfo();
    return 1;
}

int getPlaybackStatus(String nil) {
    sonos.getPlaybackStatus();
    return 1;
}

int setIp(String sonosIp) {
    ip = sonosIp;
    
    sonos.setIp(ip);
    return 1;
}