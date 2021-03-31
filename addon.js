'use strict'

const dsrlib = require("bindings")("node-dsr")
console.log(dsrlib)

function abc(aa) {
  console.log('callback funciton called - ', aa)
}

//var nodeDsr = new dsrlib.NodeDsr()
// nodeDsr.setOnMonitoringState(abc)
// nodeDsr.test()



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


