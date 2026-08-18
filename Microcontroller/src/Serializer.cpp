#include "Serializer.h"

Serializer::Serializer()
{}

void Serializer::serialSend(const environment& data)
{
    size_t size=sizeof(data);//Measures dynamically the size of the struct

    const uint8_t* packet = reinterpret_cast<const uint8_t*>(&data);

    for(size_t i=0; i<size;i++)
    {
        Serial.write(packet[i]);
    }
    Serial.write('\n');
}
