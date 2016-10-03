import path from "path";
import hap from "hap-nodejs";
import { Accessory, AccessoryLoader, Bridge, uuid } from "hap-nodejs";

console.log("MagicCube Homekit Server is now starting...");
hap.init();

const uuname = "MagicCube Homekit";
const bridge = new Bridge(uuname, uuid.generate(uuname));

const dir = path.join(__dirname, "accessories");
AccessoryLoader.loadDirectory(dir).forEach(accessory => {
    bridge.addBridgedAccessory(accessory);
});

bridge.publish({
    username: "CA:23:3A:F3:CD:A6",
    port: 51826,
    pincode: "123-45-678",
    category: Accessory.Categories.BRIDGE
});
