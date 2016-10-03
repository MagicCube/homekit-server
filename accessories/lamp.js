import request from "request-promise";
import { Accessory, Characteristic, Service, uuid } from "hap-nodejs";

let state = 0;
let hsb = { h: 100, s: 100, b: 100 };

const lamp = new Accessory("台灯", uuid.generate("magiccube/homekit-server/Lamp"));
lamp.addService(Service.Lightbulb, "LED")
    .getCharacteristic(Characteristic.On)
    .on("get", callback => {
        callback(null, state);
    })
    .on("set", async (value, callback) => {
        state = value;
        await request({
            uri: `http://192.168.2.113/${state == 1 ? "on" : "off"}`
        });
        callback();
    });
lamp.getService(Service.Lightbulb)
    .addCharacteristic(Characteristic.Hue)
    .on("get", callback => {
        callback(null, hsb.h);
    })
    .on("set", async (value, callback) => {
        hsb.h = value;
        await applyHsb();
        callback();
    });
lamp.getService(Service.Lightbulb)
    .addCharacteristic(Characteristic.Saturation)
    .on("get", callback => {
        callback(null, hsb.s);
    })
    .on("set", async (value, callback) => {
        hsb.s = value;
        await applyHsb();
        callback();
    });
lamp.getService(Service.Lightbulb)
    .addCharacteristic(Characteristic.Brightness)
    .on("get", callback => {
        callback(null, hsb.b);
    })
    .on("set", async (value, callback) => {
        hsb.b = value;
        await applyHsb();
        callback();
    });

async function applyHsb()
{
    const rgb = hsbToRGB(hsb);
    await request({
        uri: `http://192.168.2.113/rgb/${rgb}`
    });
}

function hsbToRGB(hsb)
{
    var rgb = { };
    var h = Math.round(hsb.h);
    var s = Math.round(hsb.s * 255 / 100);
    var v = Math.round(hsb.b * 255 / 100);
    if (s == 0)
    {
        rgb.r = rgb.g = rgb.b = v;
    }
    else
    {
        var t1 = v;
        var t2 = (255 - s) * v / 255;
        var t3 = (t1 - t2) * (h % 60) / 60;

        if (h == 360) h = 0;

        if (h < 60) { rgb.r = t1; rgb.b = t2; rgb.g = t2 + t3 }
        else if (h < 120) { rgb.g = t1; rgb.b = t2; rgb.r = t1 - t3 }
        else if (h < 180) { rgb.g = t1; rgb.r = t2; rgb.b = t2 + t3 }
        else if (h < 240) { rgb.b = t1; rgb.r = t2; rgb.g = t1 - t3 }
        else if (h < 300) { rgb.b = t1; rgb.g = t2; rgb.r = t2 + t3 }
        else if (h < 360) { rgb.r = t1; rgb.g = t2; rgb.b = t1 - t3 }
        else { rgb.r = 0; rgb.g = 0; rgb.b = 0 }
    }
    rgb = { r: Math.round(rgb.r), g: Math.round(rgb.g), b: Math.round(rgb.b) };
    return hex(rgb.r) + hex(rgb.g) + hex(rgb.b);
}

function hex(num)
{
    const str = num.toString(16);
    if (str.length == 1)
    {
        return "0" + str;
    }
    else
    {
        return str;
    }
}

export {
    lamp as accessory
};
