"use strict";

const dsrlib = require("@things-factory/node-dsr")

var nodeDsr = new dsrlib.NodeDsr("192.168.137.50");
console.log("openConnection: ", nodeDsr.openConnection());
console.log("closeConnection: ", nodeDsr.closeConnection());
