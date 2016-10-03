import request from "request-promise";
import { Accessory, Characteristic, Service, uuid } from "hap-nodejs";

let sensorValue = null;

const dht11 = new Accessory("温湿度器", uuid.generate("magiccube/homekit-server/Thermometer"));
dht11
    .addService(Service.TemperatureSensor)
    .getCharacteristic(Characteristic.CurrentTemperature)
    .on("get", callback => {
        if (sensorValue)
        {
            callback(null, sensorValue.temperature)
        }
        else
        {
            callback("Not ready");
        }
    });
dht11
    .addService(Service.HumiditySensor)
    .getCharacteristic(Characteristic.CurrentRelativeHumidity)
    .on("get", callback => {
        if (sensorValue)
        {
            callback(null, sensorValue.humidity)
        }
        else
        {
            callback("Not ready");
        }
    });

async function updateSensor()
{
    sensorValue = await request({
        uri: "http://192.168.2.119/",
        json: true
    });
}

async function updateSensorLoop()
{
    await updateSensor();
    setTimeout(updateSensorLoop, 5 * 1000);
}
updateSensorLoop();

export {
    dht11 as accessory
};
