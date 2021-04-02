'use strict'

const dsrlib = require("bindings")("node-dsr")

var nodeDsr = new dsrlib.NodeDsr("192.168.137.50")

// openConnection
console.log('openConnection: ', nodeDsr.openConnection())
// getSystemVersion
console.log('sysversion(0): ', nodeDsr.getSystemVersion(0))
// getSystemVersion(
console.log('sysversion(1): ', nodeDsr.getSystemVersion(1))
// test to return array
console.log('testReturnArray: ', nodeDsr.testReturnArray())

// movel #1(Absolute)
var pos = [389.48, -167.21, 438.10, 131.13, -177.21, 158.0]
var vel = [30.0, 30.0]
var acc = [40.0, 40.0]
console.log('movel #1: ', nodeDsr.movel(pos, vel, acc))
// movel #2(Relative)
var pos = [10.00, 0.0, 0.0, 0.0, 0.0, 0.0]
var vel = [30.0, 30.0]
var acc = [40.0, 40.0]
console.log('movel #2: ', nodeDsr.movel(pos, vel, acc, 0.0, true))
// movej
var pos = [-0.80, -9.44, 135.92, -5.58, 52.59, 0.66]
var vel = 30.0
var acc = 40.0
console.log('movej: ', nodeDsr.movej(pos, vel, acc))

// closeConnection
console.log('closeConnection: ', nodeDsr.closeConnection())
console.log('exit')
