const dsr = require("bindings")("node-dsr")

function sleep(ms) {
  return new Promise((r) => setTimeout(r, ms))
}

var jointPos = [-27.39, -6.95, 143.81, 0.02, 43.17, 0.01]
dsr.movej(jointPos)

// var initRet = dsr.initialize()
// var libVersion = dsr.getLibraryVersion()
// var openRet = dsr.openConnection("192.168.137.50", 12345)
// sleep(2000)
// var jointPos = [-27.39, -6.95, 143.81, 0.02, 43.17, 0.01]
// var closeRet = dsr.closeConnection()

// console.log(initRet)
// console.log(libVersion)
// console.log(openRet)
// console.log(closeRet)
