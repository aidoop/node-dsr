'use strict'

function sleep(ms) {
    return new Promise((r) => setTimeout(r, ms))
  }

async function aaaa() {
  await sleep(1000)
}

const dsrlib = require("bindings")("node-dsr")
console.log(dsrlib)

function onMonitoringState(state) {
  console.log('onMonitoringState funciton called - ', state)
  if(state === 3)
  {
    
  }
}

function onMonitoringAccessControl(state) {
    console.log('onMonitoringAccessControl funciton called - ', state)
  }



var nodeDsr = new dsrlib.NodeDsr("192.168.137.100")
nodeDsr.setOnMonitoringState(onMonitoringState)
// nodeDsr.setOnMonitoringAccessControl(onMonitoringAccessControl)
//nodeDsr.test()
nodeDsr.openConnection()

console.log('exit')

// nodeDsr.closeConnection()

// console.log('openConnection: ', nodeDsr.openConnection("192.168.137.100", 12345))
// console.log('getLibraryVersion: ', nodeDsr.getLibraryVersion())

// // movej
// var pos = [111.111, 222.222, 333.333, -444.444, -555.555, -666.666]
// console.log('movej: ', nodeDsr.movej(pos, 77.77, 88.88, 99.99, true, 101.11, true))

// // movel
// var pos = [11.11, -22.22, 33.33, 3.14, 0, -3.14]
// var vel = [30.0, 30.0]
// var acc = [5.0, 6.0]
// console.log('movel: ', nodeDsr.movel(pos, vel, acc, -23.3, true, 0, 5.5, false))

// console.log('closeConnection: ', nodeDsr.closeConnection())


