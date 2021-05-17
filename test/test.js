'use strict'

const dsrlib = require('bindings')('node-dsr')

var nodeDsr = new dsrlib.NodeDsr('192.168.137.50')

// openConnection
console.log('openConnection: ', nodeDsr.openConnection())

// getSystemVersion
console.log('sysversion(0): ', nodeDsr.getSystemVersion(0))
// getSystemVersion
console.log('sysversion(1): ', nodeDsr.getSystemVersion(1))
// getSystemVersion
console.log('sysversion(2): ', nodeDsr.getSystemVersion(1))
// movel #1(Absolute)
var pos = [389.48, -167.21, 438.1, 131.13, -177.21, 158.0]
var vel = [30.0, 30.0]
var acc = [40.0, 40.0]
console.log('movel #1: ', nodeDsr.movel(pos, vel, acc))
// movel #2(Relative)
var pos = [10.0, 0.0, 0.0, 0.0, 0.0, 0.0]
var vel = [30.0, 30.0]
var acc = [40.0, 40.0]
console.log('movel #2: ', nodeDsr.movel(pos, vel, acc, 0.0, true))
// movej
var pos = [-0.8, -9.44, 135.92, -5.58, 52.59, 0.66]
var vel = 30.0
var acc = 40.0
console.log('movej: ', nodeDsr.movej(pos, vel, acc))
// setDigitalOutput
console.log('setDigitalOutput: ', nodeDsr.setDigitalOutput(1, false))
// getCurrentPos
console.log('getCurrentPos: ', nodeDsr.getCurrentPos(true))
// getAnalogInput
console.log('getAnalogInput: ', nodeDsr.getAnalogInput(0), nodeDsr.getAnalogInput(1))
// getRobotStatus
console.log('getRobotStatus: ', nodeDsr.getRobotStatus())
// home
console.log('home: ', nodeDsr.home(true, true))

// set task speed
console.log('setAnalogOutput, ', nodeDsr.setAnalogOutput(0, 1))
console.log('setTaskSpeedLevel, ', nodeDsr.setTaskSpeedLevel(0))
console.log('getTaskSpeedData, ', nodeDsr.getTaskSpeedData())
console.log('setTaskSpeedCustom, ', nodeDsr.setTaskSpeedCustom(50, 100))
console.log('getTaskSpeedData, ', nodeDsr.getTaskSpeedData())
console.log('setTaskSpeedLevel, ', nodeDsr.setTaskSpeedLevel(2))
console.log('getTaskSpeedData, ', nodeDsr.getTaskSpeedData())
console.log('setJointSpeedLevel, ', nodeDsr.setJointSpeedLevel(2))
console.log('getJointSpeedData, ', nodeDsr.getJointSpeedData())
console.log('setJointSpeedCustom, ', nodeDsr.setJointSpeedCustom(40, 55))
console.log('getJointSpeedData, ', nodeDsr.getJointSpeedData())
console.log('setJointSpeedLevel, ', nodeDsr.setJointSpeedLevel(2))
console.log('getJointSpeedData, ', nodeDsr.getJointSpeedData())

// closeConnection
console.log('closeConnection: ', nodeDsr.closeConnection())
console.log('exit')
