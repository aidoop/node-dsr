"use strict";

const dsrlib = require("bindings")("node-dsr");

var nodeDsr = new dsrlib.NodeDsr("192.168.137.50");

// set callbacks
// function disconnected_callback() {
//   console.log("called disconnected_callback")
// }
// nodeDsr.setOnDisconnected(disconnected_callback)

// openConnection
console.log("openConnection: ", nodeDsr.openConnection());
// getSystemVersion
console.log("sysversion(0): ", nodeDsr.getSystemVersion(0));
// getSystemVersion
console.log("sysversion(1): ", nodeDsr.getSystemVersion(1));
// getSystemVersion
console.log("sysversion(2): ", nodeDsr.getSystemVersion(1));
// movel #1(Absolute)
var pos = [389.48, -167.21, 438.1, 131.13, -177.21, 158.0];
var vel = [30.0, 30.0];
var acc = [40.0, 40.0];
console.log("movel #1: ", nodeDsr.movel(pos, vel, acc));
// movel #2(Relative)
var pos = [10.0, 0.0, 0.0, 0.0, 0.0, 0.0];
var vel = [30.0, 30.0];
var acc = [40.0, 40.0];
console.log("movel #2: ", nodeDsr.movel(pos, vel, acc, 0.0, true));
// movej
var pos = [-0.8, -9.44, 135.92, -5.58, 52.59, 0.66];
var vel = 30.0;
var acc = 40.0;
console.log("movej: ", nodeDsr.movej(pos, vel, acc));
// setDigitalOutput
console.log("setDigitalOutput: ", nodeDsr.setDigitalOutput(1, false))
// getCurrentPos
console.log("getCurrentPos: ", nodeDsr.getCurrentPos(true));
// closeConnection
console.log("closeConnection: ", nodeDsr.closeConnection());
console.log("exit");
