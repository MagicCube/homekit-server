import fs from "fs";
import path from "path";

import hap from "hap-nodejs";
import { Accessory, Bridge, uuid } from "hap-nodejs";

function initBridge({ name })
{
    hap.init();
    bridge = new Bridge(name, uuid.generate("magiccube/homekit-server"));
}

function loadAccessoryDirectory({ dir = path.resolve(__dirname, "../accessories/") })
{
    fs.readdirSync(dir).forEach(filename => {
        if (filename.endsWith(".js"))
        {
            const filepath = path.join(dir, filename);
            const moduleExports = require(filepath);
            if (moduleExports && moduleExports.accessory)
            {
                const accessory = moduleExports.accessory;
                bridge.addBridgedAccessory(accessory);
                console.log(`${accessory.displayName} is now loaded.`)
            }
        }
    });
}


function startBridge({ username, port, pincode })
{
    bridge.publish({
        username,
        port,
        pincode,
        category: Accessory.Categories.BRIDGE
    });
}





let bridge = null;
let bridgeConf = require("../conf/bridge");
console.log(`${bridgeConf.name} is now starting...`);
initBridge(bridgeConf);
loadAccessoryDirectory(bridgeConf);
startBridge(bridgeConf);

export {
    bridge
};
