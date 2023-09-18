/* Sonos library by David Washington
 */

#include "Sonos.h"

byte sonosip[] = { 0, 0, 0, 0 };

// Public 

Sonos::Sonos(){
    
}

Sonos::Sonos(String ipString){
    setIp(ipString);
}

void Sonos::startPlayback(){
    setSonosMode(STARTPLAYBACK);
}

void Sonos::pausePlayback(){
    setSonosMode(PAUSEPLAYBACK);
}

void Sonos::nextTrack(){
    setSonosMode(NEXTTRACK);
}

void Sonos::previousTrack(){
    setSonosMode(PREVIOUSTRACK);
}

void Sonos::setVolume(int val){
    sonos(SETVOLUME, val);
}

void Sonos::getVolume(){
    setSonosMode(GETVOLUME);
}

void Sonos::getSongInfo(){
    setSonosMode(GETSONGINFO);
}

void Sonos::getPlaybackStatus(){
    setSonosMode(GETSTATUS);
}

void Sonos::onVolume(CallbackIntType callback){
    volumeCallback = callback;
}

void Sonos::onSongInfo(CallbackInfoType callback){
    songInfoCallback = callback;
}

void Sonos::onPlaybackStatus(CallbackInfoType callback){
    statusCallback = callback;
}

void Sonos::setIp(String ipString){
    sscanf(ipString.c_str(), "%u.%u.%u.%u", &sonosip[0], &sonosip[1], &sonosip[2], &sonosip[3]); 
}

// Private

void Sonos::setSonosMode(int cmd) {

    sonos(cmd, -1);
    
}

void Sonos::sonos(int cmd, int sonosVolume)
{

  char buf[512];
  char soapmsg[1024];
  char cmdtag[20];

  if (client.connect(sonosip, 1400)) {
        switch (cmd) {
            case PAUSEPLAYBACK:
                sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PAUSE, SOAP_FOOTER); 
                strcpy(cmdtag, "Pause");
            break;
            case STARTPLAYBACK:
                sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PLAY, SOAP_FOOTER); 
                strcpy(cmdtag, "Play");
            break;
            case NEXTTRACK:
                sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_NEXT, SOAP_FOOTER); 
                strcpy(cmdtag, "Next");
            break;
            case PREVIOUSTRACK:
                sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PREVIOUS, SOAP_FOOTER); 
                strcpy(cmdtag, "Previous");
            break;
            case SETVOLUME:
                if (sonosVolume < 0) break;
                sprintf(soapmsg, "%s%s%i%s", SOAP_HEADER, SONOS_SETVOLUME_HEADER, sonosVolume, SONOS_SETVOLUME_FOOTER); 
                strcpy(cmdtag, "SetVolume");
            break;
            case GETSONGINFO:
                sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_POSITION_INFO, SOAP_FOOTER); 
                strcpy(cmdtag, "GetPositionInfo");
            break;
            case GETSTATUS:
                sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_TRANSPORT_INFO, SOAP_FOOTER); 
                strcpy(cmdtag, "GetTransportInfo");
            break;
            case GETVOLUME:
                sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_GETVOLUME, SOAP_FOOTER); 
                strcpy(cmdtag, "GetVolume");
            break;
        }
     
        if (String(cmdtag) == "SetVolume" || String(cmdtag) == "GetVolume") {
            out("POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
        } else {
            out("POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n");
        }
    
        sprintf(buf, "Host: %d.%d.%d.%d:1400\r\n", sonosip[0], sonosip[1], sonosip[2], sonosip[3]);
        out(buf);
        sprintf(buf, "Content-Length: %d\r\n", strlen(soapmsg));
        out(buf);
        out("Content-Type: text/xml; charset=\"utf-8\"\r\n");
        
        if (String(cmdtag) == "SetVolume" || String(cmdtag) == "GetVolume") {
            sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:RenderingControl:1#%s\"\r\n", cmdtag);
        } else {
            sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:AVTransport:1#%s\"\r\n", cmdtag);
        }

        out(buf);
    
        out("\r\n");
        out(soapmsg);
        
        timeout = millis();
        while ((!client.available()) && ((millis() - timeout) < 1000));
     
        int i = 0;
        char sonosResponse[2048];
        while (client.available()) {
            char c = client.read();
            
            if (String(cmdtag) == "GetPositionInfo" || String(cmdtag) == "GetTransportInfo" || String(cmdtag) == "GetVolume" ) {
                sonosResponse[i] = c;
                i++;
            }
            
        }
    
        if (String(cmdtag) == "GetPositionInfo") {
            sonosResponse[i] = '\0';
    
            /* Get the song title */
            char *p1 = strcasestr(sonosResponse,"dc:title&gt;");
            char *p2 = strcasestr(sonosResponse,"&lt;/dc:title");
    
            int c = 0;
            for (p1 = p1 + 12; p1 != p2; p1++) {
                songTitle[c] = *p1;
                c++;
            }
            songTitle[c] = '\0';
    
            /* Get the Artist */
            p1 = strcasestr(sonosResponse,"dc:creator&gt;");
            p2 = strcasestr(sonosResponse,"&lt;/dc:creator");
            c = 0;
            
            for (p1 = p1 + 14; p1 != p2; p1++) {
                songArtist[c] = *p1;
                c++;
            }
            songArtist[c] = '\0';
    
            /* Get the Album */
            p1 = strcasestr(sonosResponse,";upnp:album&gt;");
            p2 = strcasestr(sonosResponse,"&lt;/upnp:album&gt");
            c = 0;
            
            for (p1 = p1 + 15; p1 != p2; p1++) {
                songAlbum[c] = *p1;
                c++;
            }
            songAlbum[c] = '\0';
            String songInfo = String(String(songArtist) + " - " + String(songTitle) +  " - " +  String(songAlbum));
            songInfoCallback(songInfo);
        } else if (String(cmdtag) == "GetTransportInfo") {
            sonosResponse[i] = '\0';
            
            char *p1 = strcasestr(sonosResponse,"<CurrentTransportState>");
            char *p2 = strcasestr(sonosResponse,"</CurrentTransportState>");
    
            int c = 0;
            for (p1 = p1 + 23; p1 != p2; p1++) {
                playingStatus[c] = *p1;
                c++;
            }
            playingStatus[c] = '\0';
            statusCallback(playingStatus);
            
        } else if (String(cmdtag) == "GetVolume") {
            sonosResponse[i] = '\0';
            
            char *p1 = strcasestr(sonosResponse,"<CurrentVolume>");
            char *p2 = strcasestr(sonosResponse,"</CurrentVolume>");
    
            int c = 0;
            for (p1 = p1 + 15; p1 != p2; p1++) {
                songVolume[c] = *p1;
                c++;
            }
            songVolume[c] = '\0';
            sscanf(songVolume, "%d", &sonosVolume);
            
            volumeCallback(sonosVolume);


        }
    
        client.stop();    
  }

}
  
void Sonos::out(const char *s)
{
    client.write( (const uint8_t*)s, strlen(s) );
}