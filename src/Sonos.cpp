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

  char buf[BUFFER_SIZE];
  char soapmsg[SOAP_MSG_SIZE];
  char cmdtag[CMD_TAG_SIZE];

  if (client.connect(sonosip, SONOS_PORT)) {
        switch (cmd) {
            case PAUSEPLAYBACK:
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%s", SOAP_HEADER, SONOS_PAUSE, SOAP_FOOTER); 
                strncpy(cmdtag, "Pause", sizeof(cmdtag) - 1);
            break;
            case STARTPLAYBACK:
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%s", SOAP_HEADER, SONOS_PLAY, SOAP_FOOTER); 
                strncpy(cmdtag, "Play", sizeof(cmdtag) - 1);
            break;
            case NEXTTRACK:
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%s", SOAP_HEADER, SONOS_NEXT, SOAP_FOOTER); 
                strncpy(cmdtag, "Next", sizeof(cmdtag) - 1);
            break;
            case PREVIOUSTRACK:
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%s", SOAP_HEADER, SONOS_PREVIOUS, SOAP_FOOTER); 
                strncpy(cmdtag, "Previous", sizeof(cmdtag) - 1);
            break;
            case SETVOLUME:
                if (sonosVolume < 0) break;
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%i%s", SOAP_HEADER, SONOS_SETVOLUME_HEADER, sonosVolume, SONOS_SETVOLUME_FOOTER); 
                strncpy(cmdtag, "SetVolume", sizeof(cmdtag) - 1);
            break;
            case GETSONGINFO:
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%s", SOAP_HEADER, SONOS_POSITION_INFO, SOAP_FOOTER); 
                strncpy(cmdtag, "GetPositionInfo", sizeof(cmdtag) - 1);
            break;
            case GETSTATUS:
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%s", SOAP_HEADER, SONOS_TRANSPORT_INFO, SOAP_FOOTER); 
                strncpy(cmdtag, "GetTransportInfo", sizeof(cmdtag) - 1);
            break;
            case GETVOLUME:
                snprintf(soapmsg, sizeof(soapmsg), "%s%s%s", SOAP_HEADER, SONOS_GETVOLUME, SOAP_FOOTER); 
                strncpy(cmdtag, "GetVolume", sizeof(cmdtag) - 1);
            break;
        }
    
        if (strcmp(cmdtag, "SetVolume") == 0 || strcmp(cmdtag, "GetVolume") == 0) {
            out("POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
        } else {
            out("POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n");
        }
 
        snprintf(buf, sizeof(buf), "Host: %d.%d.%d.%d:1400\r\n", sonosip[0], sonosip[1], sonosip[2], sonosip[3]);
        out(buf);
        snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", strlen(soapmsg));
        out(buf);
        out("Content-Type: text/xml; charset=\"utf-8\"\r\n");
        
        if (strcmp(cmdtag, "SetVolume") == 0 || strcmp(cmdtag, "GetVolume") == 0) {
            snprintf(buf, sizeof(buf), "Soapaction: \"urn:schemas-upnp-org:service:RenderingControl:1#%s\"\r\n", cmdtag);
        } else {
            snprintf(buf, sizeof(buf), "Soapaction: \"urn:schemas-upnp-org:service:AVTransport:1#%s\"\r\n", cmdtag);
        }

        out(buf);
        out("\r\n");
        out(soapmsg);
        
        timeout = millis();
        while ((!client.available()) && ((millis() - timeout) < SONOS_CONNECTION_TIMEOUT));
     
        int i = 0;
        char sonosResponse[SONOS_RESPONSE_SIZE];
        while (client.available()) {
            char c = client.read();
            
            if (strcmp(cmdtag, "GetPositionInfo") == 0 || strcmp(cmdtag, "GetTransportInfo") == 0 || strcmp(cmdtag, "GetVolume") == 0) {
                sonosResponse[i] = c;
                i++;
            }
            
        }
    
        if (strcmp(cmdtag, "GetPositionInfo") == 0) {
            sonosResponse[i] = '\0';
    
            char* title = parseResponse(sonosResponse, "dc:title&gt;", "&lt;/dc:title");
            if (title) {
                strncpy(songTitle, title, sizeof(songTitle) - 1);
                free(title);
            }
            char* artist = parseResponse(sonosResponse, "dc:creator&gt;", "&lt;/dc:creator");
            if (artist) {
                strncpy(songArtist, artist, sizeof(songArtist) - 1);
                free(artist);
            }
            char* album = parseResponse(sonosResponse, ";upnp:album&gt;", "&lt;/upnp:album&gt");
            if (album) {
                strncpy(songAlbum, album, sizeof(songAlbum) - 1);
                free(album);
            }
            
            memset(buf, 0, sizeof(buf));

            JSONBufferWriter writer(buf, sizeof(buf));
            writer.beginObject();
                writer.name("artist").value(songArtist);
                writer.name("title").value(songTitle);
                writer.name("album").value(songAlbum);
            writer.endObject();
            
            songInfoCallback(buf);
            
        } else if (strcmp(cmdtag, "GetTransportInfo") == 0) {
            sonosResponse[i] = '\0';
            
            char* status = parseResponse(sonosResponse, "<CurrentTransportState>", "</CurrentTransportState>");
            if (status) {
                strncpy(playingStatus, status, sizeof(playingStatus) - 1);
                free(status);
            }
            
            statusCallback(playingStatus);


        }else if (strcmp(cmdtag, "GetVolume") == 0) {
            sonosResponse[i] = '\0';
            
            char* volume = parseResponse(sonosResponse, "<CurrentVolume>", "</CurrentVolume>");
            if (volume) {
                strncpy(songVolume, volume, sizeof(songVolume) - 1);
                free(volume);
            }
            
            sscanf(songVolume, "%d", &sonosVolume);
            volumeCallback(sonosVolume);
        }
    
        client.stop();    
  }

}

char* Sonos::parseResponse(char* response, const char* startTag, const char* endTag) {
    char* value = NULL;
    char* p1 = strcasestr(response, startTag);
    char* p2 = strcasestr(response, endTag);
    if (p1 && p2) {
        int len = p2 - p1 - strlen(startTag);
        value = (char*)malloc(len + 1);
        if (value) {
            strncpy(value, p1 + strlen(startTag), len);
            value[len] = '\0';
        }
    }
    return value;
}

void Sonos::out(const char *s)
{
    client.write( (const uint8_t*)s, strlen(s) );
}