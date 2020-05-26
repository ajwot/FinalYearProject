"use strict";

var Cylon = require("cylon");

Cylon.robot({
  connections: {
    leap: { adaptor: "leapmotion" },
    
  },

  devices: {
    
    leapmotion: {driver: "leapmotion", connection: "leap"}
  },

  work: function(my) {
    my.leapmotion.on("frame", function(frame) {
      if (frame.hands.length > 0) {
        console.log("hello")
      } else {
        console.log("hell")
      }
    });
  }
}).start()