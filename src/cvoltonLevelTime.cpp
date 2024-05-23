#include <string>
#include <Geode/Geode.hpp>
#include <Geode/cocos/support/base64.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>

// all of this code was stolen from CVolton's amazing mod "BetterInfo", as I am too lazy to do all of this myself.

inline int speedToPortalId(int speed) {
    switch(speed) {
    default:
        return 201;
        break;
    case 1:
        return 200;
        break;
    case 2:
        return 202;
        break;
    case 3:
        return 203;
        break;
    case 4:
        return 1334;
        break;
    }
}

inline float travelForPortalId(int speed) {
    switch (speed)
    {
    case 200:
        return 251.16008f;
        break;
    default:
        return 311.58011f;
        break;
    case 202:
        return 387.42014f;
        break;
    case 203:
        return 468.00015f;
        break;
    case 1334:
        return 576.00018f;
        break;
    }
}

inline bool objectIDIsSpeedPortal(int id) {
    return (id == 200 || id == 201 || id == 202 || id == 203 || id == 1334);
}

int stoi(std::string_view str) {
    int result = 0;
    std::from_chars(str.data(), str.data() + str.size(), result);
    return result;
}

std::string decodeBase64Gzip(const std::string& input) {
    return (std::string) cocos2d::ZipUtils::decompressString(input, false, 0);
}


int timeForLevelString(const std::string& levelString) {
        std::string epicString;
    try {
        auto decompressString = decodeBase64Gzip(levelString);
        std::stringstream responseStream(decompressString);
        std::string currentObject;
        std::string currentKey;
        std::string keyID;

        //std::stringstream objectStream;
        float prevPortalX = 0;
        int prevPortalId = 0;

        float timeFull = 0;

        float maxPos = 0;
        while(std::getline(responseStream, currentObject, ';')) {
            size_t i = 0;
            int objID = 0;
            float xPos = 0;
            bool checked = false;

            std::stringstream objectStream(currentObject);
            while(std::getline(objectStream, currentKey, ',')) {
                epicString += currentKey + "\n";

                if(i % 2 == 0) keyID = currentKey;
                else {
                    if(keyID == "1") objID = stoi(currentKey);
                    else if(keyID == "2") xPos = stof(currentKey);
                    else if(keyID == "13") checked = stoi(currentKey);
                    else if(keyID == "kA4") prevPortalId = speedToPortalId(stoi(currentKey));
                }
                i++;

                if(xPos != 0 && objID != 0 && checked == true) break;
            }

            if(maxPos < xPos) maxPos = xPos;
            if(!checked || !objectIDIsSpeedPortal(objID)) continue;

            timeFull += (xPos - prevPortalX) / travelForPortalId(prevPortalId);
            prevPortalId = objID;
            prevPortalX = xPos;
        }
        timeFull += (maxPos - prevPortalX) / travelForPortalId(prevPortalId);
        timeFull /= 60;
        return timeFull;
    } catch(std::exception e) {
        return 0;
    }
}
