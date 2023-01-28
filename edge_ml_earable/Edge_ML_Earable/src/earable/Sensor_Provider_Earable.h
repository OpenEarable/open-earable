#include "sensors/SensorTypes_Earable.h"
#include "sensors/SensorID_Earable.h"
#include "BLEHandler_Earable.h"

#ifndef SENSORTEC_EARABLE_H
#define SENSORTEC_EARABLE_H



class Sensor_Provider_Earable {
public:
    Sensor_Provider_Earable();

    bool begin();
    void update();
    void configureSensor(SensorConfigurationPacket& config);

    // Add additional methods

    void debug(Stream &stream);

private:
    Sensor sensors[SENSOR_COUNT];
    int ID_type_assignment[SENSOR_COUNT];

    Stream *_debug;
    void init_ID_type_assignment();
    void update_sensor(Sensor &sens);
    void send_sensor_data(int ID);
};

extern Sensor_Provider_Earable sensor_provider_earable;

#endif //SENSORTEC_EARABLE_H
