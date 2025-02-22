#pragma once
#include <cstring>

class ZaparooToken {
public:
    ZaparooToken()
        : id(nullptr), payload(nullptr), launchAudio(nullptr), removeAudio(nullptr), detectAudio(nullptr), launchJPEG(nullptr) {}

    ~ZaparooToken() {
        clearMemory();
    }

    // Copy constructor
    ZaparooToken(const ZaparooToken& other)
        : id(nullptr), payload(nullptr), launchAudio(nullptr), removeAudio(nullptr), detectAudio(nullptr), launchJPEG(nullptr), valid(other.valid) {
        setId(other.id);
        setPayload(other.payload);
        setLaunchAudio(other.launchAudio);
        setRemoveAudio(other.removeAudio);
        setDetectAudio(other.detectAudio);
        setLaunchJPEG(other.launchJPEG);
    }

    // Move constructor
    ZaparooToken(ZaparooToken&& other) noexcept
        : id(other.id), payload(other.payload), launchAudio(other.launchAudio), removeAudio(other.removeAudio), valid(other.valid), detectAudio(other.detectAudio), launchJPEG(other.launchJPEG) {
        other.id = nullptr;
        other.payload = nullptr;
        other.launchAudio = nullptr;
        other.removeAudio = nullptr;
        other.detectAudio = nullptr;
        other.launchJPEG = nullptr;
    }
    // Copy assignment operator
     ZaparooToken& operator=(const ZaparooToken& other) {
        if (this != &other) { // Prevent self-assignment
            clearMemory(); // Clear existing resources
            valid = other.valid; // Copy non-dynamic member
            setId(other.id);
            setPayload(other.payload);
            setLaunchAudio(other.launchAudio);
            setRemoveAudio(other.removeAudio);
            setDetectAudio(other.detectAudio);
            setLaunchJPEG(other.launchJPEG);
        }
        return *this;
    }

    // Move assignment operator
    ZaparooToken& operator=(ZaparooToken&& other) noexcept {
        if (this != &other) { // Prevent self-assignment
            clearMemory(); // Clear existing resources

            // Transfer ownership of resources
            id = other.id;
            payload = other.payload;
            launchAudio = other.launchAudio;
            removeAudio = other.removeAudio;
            detectAudio = other.detectAudio;
            launchJPEG = other.launchJPEG;
            valid = other.valid;

            // Nullify other's pointers
            other.id = nullptr;
            other.payload = nullptr;
            other.launchAudio = nullptr;
            other.removeAudio = nullptr;
            other.detectAudio = nullptr;
            other.launchJPEG = nullptr;
        }
        return *this;
    }

    const char* getId() const { return id; }
    const char* getPayload() const { return payload; }
    const char* getLaunchAudio() const { return launchAudio; }
    const char* getRemoveAudio() const { return removeAudio; }
    const char* getDetectAudio() const { return detectAudio; }
    const char* getLaunchJPEG() const { return launchJPEG; }
    const bool getValid() const { return valid; }

    void setId(const char* value) { allocateAndCopy(id, value); }
    void setPayload(const char* value) { allocateAndCopy(payload, value); }
    void setLaunchAudio(const char* value) { allocateAndCopy(launchAudio, value); }
    void setRemoveAudio(const char* value) { allocateAndCopy(removeAudio, value); }
    void setDetectAudio(const char* value) { allocateAndCopy(detectAudio, value); }
    void setLaunchJPEG (const char* value) { allocateAndCopy(launchJPEG, value); }
    void setValid(const bool valid){this->valid = valid;}

    bool isIdSet() const { return id != nullptr; }
    bool isPayloadSet() const { return payload != nullptr; }
    bool isLaunchAudioSet() const { return launchAudio != nullptr; }
    bool isRemoveAudioSet() const { return removeAudio != nullptr; }
    bool isDetectAudioSet() const { return detectAudio != nullptr; }
    bool isLaunchJPEGSet() const { return launchJPEG != nullptr; }

    bool isBlankPayload() const { return !payload || payload[0] == '\0'; }

private:
    char* id;
    char* payload;
    char* launchAudio;
    char* removeAudio;
    char* detectAudio;
    char* launchJPEG;
    bool valid = true;

    // Helper function to allocate and copy strings
    void allocateAndCopy(char*& member, const char* value) {
        if (member) {
            delete[] member;
        }
        if (value) {
            size_t len = strlen(value) + 1;
            member = new char[len];
            strcpy(member, value);
        } else {
            member = nullptr;
        }
    }

    // Helper function to clear all allocated memory
    void clearMemory() {
        delete[] id;
        delete[] payload;
        delete[] launchAudio;
        delete[] removeAudio;
        delete[] detectAudio;
        delete[] launchJPEG;
        id = nullptr;
        payload = nullptr;
        launchAudio = nullptr;
        removeAudio = nullptr;
        detectAudio = nullptr;
        launchJPEG = nullptr;
    }
};
