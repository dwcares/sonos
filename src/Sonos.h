#pragma once

/* Sonos library by David Washington
 */

#include "Particle.h"

#define SOAP_HEADER "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>"
#define SOAP_FOOTER "</s:Body></s:Envelope>\r\n"

#define SONOS_PAUSE "<u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Pause>" 
#define SONOS_PLAY "<u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play>"
#define SONOS_NEXT "<u:Next xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Next>"
#define SONOS_PREVIOUS "<u:Previous xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Previous>"
#define SONOS_SETVOLUME_HEADER "<u:SetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>"
#define SONOS_SETVOLUME_FOOTER "<\/DesiredVolume><\/u:SetVolume>\/s:Body><\/s:Envelope>"
#define SONOS_TRANSPORT_INFO "<u:GetTransportInfo xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:GetTransportInfo>"
#define SONOS_POSITION_INFO "<u:GetPositionInfo xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:GetPositionInfo>"
#define SONOS_GETVOLUME "<u:GetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel></u:GetVolume>"

#define PAUSEPLAYBACK 0
#define STARTPLAYBACK 1
#define NEXTTRACK 2
#define PREVIOUSTRACK 3
#define SETVOLUME 4
#define GETVOLUME 5
#define GETSONGINFO 6
#define GETSTATUS 7

typedef void (*CallbackInfoType)(String);
typedef void (*CallbackIntType)(int);

class Sonos {

 public:
  Sonos();
  Sonos(String);

  void
    startPlayback(void),
    pausePlayback(void),
    nextTrack(void),
    previousTrack(void),
    setVolume(int val),
    getVolume(void),
    getSongInfo(void),
    getPlaybackStatus(void),
    onVolume(CallbackIntType callback),
    onSongInfo(CallbackInfoType callback),
    onPlaybackStatus(CallbackInfoType callback),
    setIp(String ip);

 private:
  TCPClient client;
  unsigned long timeout;
  int 
    sonosVolume;
  char 
    songTitle[32],
    songArtist[32],
    songVolume[12],
    songAlbum[32],
    playingStatus[32];
  
  CallbackIntType
    volumeCallback;
    
  CallbackInfoType 
    songInfoCallback,
    statusCallback;
  
  void 
    setSonosMode(int cmd),
    sonos(int cmd, int sonosVolume),
    out(const char *s);
};
