import request from "request-promise";
import { Accessory, Characteristic, Service, uuid } from "hap-nodejs";

let state = 0;

const lamp = new Accessory("台灯", uuid.generate("magiccube/homekit-server/Lamp"));
lamp
    .addService(Service.Lightbulb, "LED")
    .getCharacteristic(Characteristic.On)
    .on("get", callback => {
        callback(null, state);
    })
    .on("set", async (value, callback) => {
        state = value;
        const command = state == 1 ? "on" : "off";
        const result = await request({
            uri: `http://192.168.2.113/${command}`
        });
        if (command == result)
        {
            callback();
        }
        else
        {
            callback("unknown");
        }
    });

export {
    lamp as accessory
};
